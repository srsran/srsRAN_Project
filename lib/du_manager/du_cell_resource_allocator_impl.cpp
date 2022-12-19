/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_cell_resource_allocator_impl.h"
#include "srsgnb/mac/config/mac_cell_group_config_factory.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"

using namespace srsgnb;
using namespace srs_du;

static lcid_t find_empty_lcid(const std::vector<rlc_bearer_config>& rlc_bearers)
{
  static_vector<lcid_t, MAX_NOF_DRBS> used_lcids;
  for (const auto& bearer : rlc_bearers) {
    if (bearer.drb_id.has_value()) {
      used_lcids.push_back(bearer.lcid);
    }
  }
  std::sort(used_lcids.begin(), used_lcids.end());
  if (used_lcids.empty() or used_lcids[0] > LCID_MIN_DRB) {
    return LCID_MIN_DRB;
  }
  auto it = std::adjacent_find(used_lcids.begin(), used_lcids.end(), [](lcid_t l, lcid_t r) { return l + 1 < r; });
  if (it == used_lcids.end()) {
    // no gaps found. Use the last value + 1.
    --it;
  }
  // beginning of the gap + 1.
  lcid_t lcid = uint_to_lcid(static_cast<unsigned>(*it) + 1U);
  if (lcid > LCID_MAX_DRB) {
    return INVALID_LCID;
  }
  return lcid;
}

du_cell_resource_allocator_impl::du_cell_resource_allocator_impl(span<const du_cell_config> cell_cfg_list_,
                                                                 const serving_cell_config& default_ue_cell_cfg_) :
  cell_cfg_list(cell_cfg_list_),
  default_ue_cell_cfg(default_ue_cell_cfg_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  sr_offset_free_list(cell_cfg_list.size())
{
  // Setup resource free lists.
  const unsigned period =
      sr_periodicity_to_slot(default_ue_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].period);
  for (unsigned i = 0; i != cell_cfg_list.size(); ++i) {
    sr_offset_free_list[i].resize(period);
    for (unsigned offset = 0; offset != period; ++offset) {
      sr_offset_free_list[i][offset] = offset;
    }
  }
}

const cell_group_config* du_cell_resource_allocator_impl::update_context(du_ue_index_t   ue_index,
                                                                         du_cell_index_t pcell_idx,
                                                                         const f1ap_ue_context_update_request& upd_req)
{
  if (not ue_res_list.contains(ue_index)) {
    // > If UE is not yet allocated, create entry in pool.
    ue_res_list.emplace(ue_index);
    ue_res_list[ue_index].spcell_cfg.cell_index = INVALID_DU_CELL_INDEX;
  } else {
    // > If UE is already allocated, deallocate resources for previously configured cells that have now been removed or
    // changed.
    cell_group_config& ue_res = ue_res_list[ue_index];

    if (pcell_idx != ue_res.spcell_cfg.cell_index) {
      // >> PCell changed. Deallocate PCell resources.
      deallocate_cell_resources(ue_index, SERVING_CELL_PCELL_IDX);
    }
    for (serv_cell_index_t scell_idx : upd_req.scells_to_rem) {
      // >> SCells to be removed. Deallocate them.
      deallocate_cell_resources(ue_index, scell_idx);
    }
    for (const scell_to_setup& scell : upd_req.scells_to_setup) {
      // >> If SCells to be modified changed DU Cell Index.
      if (ue_res.scells.contains(scell.serv_cell_index) and
          ue_res.scells[scell.serv_cell_index].cell_index != scell.cell_index) {
        deallocate_cell_resources(ue_index, scell.serv_cell_index);
      }
    }
  }
  cell_group_config& ue_res = ue_res_list[ue_index];

  // > Allocate new or modified bearers.
  for (srb_id_t srb_id : upd_req.srbs_to_setup) {
    lcid_t lcid = srb_id_to_lcid(srb_id);
    ue_res.rlc_bearers.emplace_back();
    ue_res.rlc_bearers.back().lcid    = lcid;
    ue_res.rlc_bearers.back().rlc_cfg = make_default_srb_rlc_config();
    // TODO: Parameterize SRB config.
  }
  for (const drb_to_setup& drb : upd_req.drbs_to_setup) {
    lcid_t lcid;
    if (drb.lcid.has_value()) {
      // >> CU assigned LCID.
      lcid = *drb.lcid;
      if (std::any_of(ue_res.rlc_bearers.begin(), ue_res.rlc_bearers.end(), [&drb](const auto& item) {
            return *drb.lcid == item.lcid;
          })) {
        logger.warning("Failed to allocate DRB-Id={}. Cause: Specified LCID={} already exists", drb.drb_id, lcid);
        return nullptr;
      }
    } else {
      // >> Allocate LCID if not specified by F1AP.
      lcid = find_empty_lcid(ue_res.rlc_bearers);
      if (lcid > LCID_MAX_DRB) {
        logger.warning("Failed to allocate DRB-Id={}. Cause: No available LCIDs", drb.drb_id);
        return nullptr;
      }
    }
    ue_res.rlc_bearers.emplace_back();
    ue_res.rlc_bearers.back().lcid    = lcid;
    ue_res.rlc_bearers.back().drb_id  = drb.drb_id;
    ue_res.rlc_bearers.back().rlc_cfg = create_rlc_config(drb);
  }

  // > Allocate resources for new or modified cells.
  if (pcell_idx != ue_res.spcell_cfg.cell_index) {
    // >> PCell changed. Allocate new PCell resources.
    allocate_cell_resources(ue_index, pcell_idx, SERVING_CELL_PCELL_IDX);
  }
  for (const scell_to_setup& sc : upd_req.scells_to_setup) {
    // >> SCells Added/Modified. Allocate new SCell resources.
    allocate_cell_resources(ue_index, sc.cell_index, sc.serv_cell_index);
  }

  return &ue_res;
}

bool du_cell_resource_allocator_impl::allocate_cell_resources(du_ue_index_t     ue_index,
                                                              du_cell_index_t   cell_index,
                                                              serv_cell_index_t serv_cell_index)
{
  cell_group_config& ue_res = ue_res_list[ue_index];

  if (serv_cell_index == SERVING_CELL_PCELL_IDX) {
    // It is a PCell.
    srsgnb_assert(ue_res.spcell_cfg.cell_index == INVALID_DU_CELL_INDEX, "Reallocation of PCell detected");
    ue_res.spcell_cfg.cell_index     = cell_index;
    ue_res.spcell_cfg.serv_cell_idx  = SERVING_CELL_PCELL_IDX;
    ue_res.spcell_cfg.spcell_cfg_ded = default_ue_cell_cfg;
    ue_res.mcg_cfg                   = config_helpers::make_initial_mac_cell_group_config();
    // TODO: Move to helper.
    ue_res.pcg_cfg.p_nr_fr1            = 10;
    ue_res.pcg_cfg.pdsch_harq_codebook = pdsch_harq_ack_codebook::dynamic;

    for (auto& sr : ue_res.spcell_cfg.spcell_cfg_ded.ul_config->init_ul_bwp.pucch_cfg->sr_res_list) {
      if (sr_offset_free_list[cell_index].empty()) {
        logger.warning("Unable to allocate dedicated PUCCH SR resource for UE={}, cell={}", ue_index, cell_index);
        return false;
      }
      sr.offset = sr_offset_free_list[cell_index].back();
      sr_offset_free_list[cell_index].pop_back();
    }
  } else {
    srsgnb_assert(not ue_res.scells.contains(serv_cell_index), "Reallocation of SCell detected");
    ue_res.scells.emplace(serv_cell_index);
    ue_res.scells[serv_cell_index].cell_index      = cell_index;
    ue_res.scells[serv_cell_index].serv_cell_index = serv_cell_index;
    // TODO: Allocate SCell params.
  }
  return true;
}

void du_cell_resource_allocator_impl::deallocate_cell_resources(du_ue_index_t     ue_index,
                                                                serv_cell_index_t serv_cell_index)
{
  cell_group_config& ue_res = ue_res_list[ue_index];

  // Return resources back to free lists.
  if (serv_cell_index == SERVING_CELL_PCELL_IDX) {
    srsgnb_assert(ue_res.spcell_cfg.cell_index != INVALID_DU_CELL_INDEX,
                  "Double deallocation of same UE cell resources detected");
    for (auto& sr : ue_res.spcell_cfg.spcell_cfg_ded.ul_config->init_ul_bwp.pucch_cfg->sr_res_list) {
      sr_offset_free_list[ue_res.spcell_cfg.cell_index].push_back(sr.offset);
    }
    ue_res.spcell_cfg.cell_index = INVALID_DU_CELL_INDEX;
  } else {
    // TODO: Remove of SCell params.
    ue_res.scells.erase(serv_cell_index);
  }
}
