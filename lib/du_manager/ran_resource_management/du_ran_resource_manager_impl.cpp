/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ran_resource_manager_impl.h"
#include "srsran/mac/config/mac_cell_group_config_factory.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/rlc/rlc_srb_config_factory.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

/// \brief Finds an unused LCID for DRBs given a list of UE configured RLC bearers.
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

du_ue_ran_resource_updater_impl::du_ue_ran_resource_updater_impl(cell_group_config*            cell_grp_cfg_,
                                                                 du_ran_resource_manager_impl& parent_,
                                                                 du_ue_index_t                 ue_index_) :
  cell_grp(cell_grp_cfg_), parent(&parent_), ue_index(ue_index_)
{
}

du_ue_ran_resource_updater_impl::~du_ue_ran_resource_updater_impl()
{
  parent->deallocate_context(ue_index);
}

du_ue_resource_update_response du_ue_ran_resource_updater_impl::update(du_cell_index_t pcell_index,
                                                                       const f1ap_ue_context_update_request& upd_req,
                                                                       const cell_group_config* reestablished_context)
{
  return parent->update_context(ue_index, pcell_index, upd_req, reestablished_context);
}

///////////////////////////

du_ran_resource_manager_impl::du_ran_resource_manager_impl(span<const du_cell_config>                cell_cfg_list_,
                                                           const scheduler_expert_config&            scheduler_cfg,
                                                           const std::map<srb_id_t, du_srb_config>&  srbs_,
                                                           const std::map<five_qi_t, du_qos_config>& qos_) :
  cell_cfg_list(cell_cfg_list_),
  srb_config(srbs_),
  qos_config(qos_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  pucch_res_mng(cell_cfg_list, scheduler_cfg.ue.max_pucchs_per_slot)
{
}

ue_ran_resource_configurator du_ran_resource_manager_impl::create_ue_resource_configurator(du_ue_index_t   ue_index,
                                                                                           du_cell_index_t pcell_index)
{
  if (ue_res_pool.contains(ue_index)) {
    return ue_ran_resource_configurator{std::unique_ptr<du_ue_ran_resource_updater_impl>{nullptr},
                                        std::string("Double allocation of same not supported")};
  }
  ue_res_pool.emplace(ue_index);
  auto& mcg = ue_res_pool[ue_index].cg_cfg;

  // UE initialized PCell.
  error_type<std::string> err = allocate_cell_resources(ue_index, pcell_index, SERVING_CELL_PCELL_IDX);
  if (not err.has_value()) {
    ue_res_pool.erase(ue_index);
    return ue_ran_resource_configurator{std::unique_ptr<du_ue_ran_resource_updater_impl>{nullptr}, err.error()};
  }

  return ue_ran_resource_configurator{std::make_unique<du_ue_ran_resource_updater_impl>(&mcg, *this, ue_index)};
}

static error_type<std::string> validate_drb_modification_request(const f1ap_drb_to_modify&     drb,
                                                                 span<const rlc_bearer_config> rlc_bearers)
{
  // Search for established DRB with matching DRB-Id.
  auto prev_drb_it = std::find_if(rlc_bearers.begin(), rlc_bearers.end(), [&drb](const rlc_bearer_config& item) {
    return item.drb_id.has_value() and item.drb_id.value() == drb.drb_id;
  });
  if (prev_drb_it == rlc_bearers.end()) {
    return make_unexpected(fmt::format("Failed to modify {}. Cause: DRB not found", drb.drb_id));
  }
  return {};
}

static error_type<std::string> validate_drb_setup_request(const f1ap_drb_to_setup&                  drb,
                                                          span<const rlc_bearer_config>             rlc_bearers,
                                                          const std::map<five_qi_t, du_qos_config>& qos_config)
{
  // Validate QOS config.
  five_qi_t fiveqi = drb.qos_info.drb_qos.qos_characteristics.get_five_qi();
  auto      qos_it = qos_config.find(fiveqi);
  if (qos_it == qos_config.end()) {
    return make_unexpected(fmt::format("Failed to allocate {}. Cause: No {} 5QI configured", drb.drb_id, fiveqi));
  }
  const du_qos_config& qos = qos_it->second;
  if (qos.rlc.mode != drb.mode) {
    return make_unexpected(
        fmt::format("RLC mode mismatch for {}. QoS config for {} configures {} but CU-CP requested {}",
                    drb.drb_id,
                    fiveqi,
                    qos.rlc.mode,
                    drb.mode));
  }

  // Search for established DRB with matching DRB-Id.
  auto prev_drb_it = std::find_if(rlc_bearers.begin(), rlc_bearers.end(), [&drb](const rlc_bearer_config& item) {
    return item.drb_id.has_value() and item.drb_id.value() == drb.drb_id;
  });
  if (prev_drb_it != rlc_bearers.end()) {
    return make_unexpected(fmt::format("Failed to allocate {}. Cause: DRB already exists", drb.drb_id));
  }

  return {};
}

static void reestablish_context(cell_group_config& new_ue_cfg, const cell_group_config& old_ue_cfg)
{
  for (const rlc_bearer_config& old_bearer : old_ue_cfg.rlc_bearers) {
    auto it = std::find_if(
        new_ue_cfg.rlc_bearers.begin(), new_ue_cfg.rlc_bearers.end(), [&old_bearer](const rlc_bearer_config& item) {
          return item.drb_id == old_bearer.drb_id and (item.drb_id.has_value() or (item.lcid == old_bearer.lcid));
        });
    if (it == new_ue_cfg.rlc_bearers.end()) {
      // Bearer not found in new context. Add it.
      new_ue_cfg.rlc_bearers.push_back(old_bearer);
    }
  }
}

du_ue_resource_update_response
du_ran_resource_manager_impl::update_context(du_ue_index_t                         ue_index,
                                             du_cell_index_t                       pcell_idx,
                                             const f1ap_ue_context_update_request& upd_req,
                                             const cell_group_config*              reestablished_context)
{
  srsran_assert(ue_res_pool.contains(ue_index), "This function should only be called for an already allocated UE");
  cell_group_config&             ue_mcg = ue_res_pool[ue_index].cg_cfg;
  du_ue_resource_update_response resp;

  // > Deallocate resources for previously configured cells that have now been removed or changed.
  if (ue_mcg.cells.contains(0) and ue_mcg.cells[0].serv_cell_cfg.cell_index != pcell_idx) {
    // >> PCell changed. Deallocate PCell resources.
    deallocate_cell_resources(ue_index, SERVING_CELL_PCELL_IDX);
  }
  for (serv_cell_index_t scell_idx : upd_req.scells_to_rem) {
    // >> SCells to be removed. Deallocate them.
    deallocate_cell_resources(ue_index, scell_idx);
  }
  for (const f1ap_scell_to_setup& scell : upd_req.scells_to_setup) {
    // >> If SCells to be modified changed DU Cell Index.
    if (ue_mcg.cells.contains(scell.serv_cell_index) and
        ue_mcg.cells[scell.serv_cell_index].serv_cell_cfg.cell_index != scell.cell_index) {
      deallocate_cell_resources(ue_index, scell.serv_cell_index);
    }
  }

  // > In case of RRC Reestablishment, retrieve old DRB context, to be considered in the config update.
  if (reestablished_context != nullptr) {
    reestablish_context(ue_mcg, *reestablished_context);
  }

  // > Deallocate removed SRBs / DRBs.
  for (drb_id_t drb_id : upd_req.drbs_to_rem) {
    auto it = std::find_if(ue_mcg.rlc_bearers.begin(), ue_mcg.rlc_bearers.end(), [drb_id](const rlc_bearer_config& b) {
      return b.drb_id == drb_id;
    });
    if (it != ue_mcg.rlc_bearers.end()) {
      ue_mcg.rlc_bearers.erase(it);
      continue;
    } else {
      logger.warning("Failed to release {}. Cause: DRB not found", drb_id);
    }
  }

  // > Allocate new SRBs.
  for (srb_id_t srb_id : upd_req.srbs_to_setup) {
    // >> New or Modified SRB.
    lcid_t lcid = srb_id_to_lcid(srb_id);
    if (std::any_of(ue_mcg.rlc_bearers.begin(), ue_mcg.rlc_bearers.end(), [lcid](const auto& item) {
          return item.lcid == lcid;
        })) {
      // The SRB is already setup (e.g. SRB1 gets setup automatically).
      continue;
    }
    ue_mcg.rlc_bearers.emplace_back();
    ue_mcg.rlc_bearers.back().lcid = lcid;

    auto srb_it = srb_config.find(srb_id);
    if (srb_it != srb_config.end()) {
      ue_mcg.rlc_bearers.back().rlc_cfg = srb_it->second.rlc;
      ue_mcg.rlc_bearers.back().mac_cfg = srb_it->second.mac;
    } else {
      ue_mcg.rlc_bearers.back().rlc_cfg = make_default_srb_rlc_config();
      ue_mcg.rlc_bearers.back().mac_cfg = make_default_srb_mac_lc_config(lcid);
    }
  }

  // > Create new DRBs.
  for (const f1ap_drb_to_setup& drb : upd_req.drbs_to_setup) {
    auto res = validate_drb_setup_request(drb, ue_mcg.rlc_bearers, qos_config);
    if (not res.has_value()) {
      resp.failed_drbs.push_back(drb.drb_id);
      continue;
    }

    // > Allocate LCID.
    lcid_t lcid = find_empty_lcid(ue_mcg.rlc_bearers);
    if (lcid > LCID_MAX_DRB) {
      logger.warning("Failed to allocate {}. Cause: No available LCIDs", drb.drb_id);
      resp.failed_drbs.push_back(drb.drb_id);
      continue;
    }

    // >> Get RLC config from 5QI
    five_qi_t            fiveqi = drb.qos_info.drb_qos.qos_characteristics.get_five_qi();
    const du_qos_config& qos    = qos_config.at(fiveqi);
    ue_mcg.rlc_bearers.emplace_back();
    ue_mcg.rlc_bearers.back().lcid    = lcid;
    ue_mcg.rlc_bearers.back().drb_id  = drb.drb_id;
    ue_mcg.rlc_bearers.back().rlc_cfg = qos.rlc;
    ue_mcg.rlc_bearers.back().mac_cfg = qos.mac;

    // Update pdcp_sn_len in RLC config
    auto& rlc_cfg = ue_mcg.rlc_bearers.back().rlc_cfg;
    switch (rlc_cfg.mode) {
      case rlc_mode::am:
        rlc_cfg.am.tx.pdcp_sn_len = drb.pdcp_sn_len;
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        rlc_cfg.um.tx.pdcp_sn_len = drb.pdcp_sn_len;
        break;
      default:
        break;
    }
  }

  // > Modify existing DRBs.
  for (const f1ap_drb_to_modify& drb : upd_req.drbs_to_mod) {
    auto res = validate_drb_modification_request(drb, ue_mcg.rlc_bearers);
    if (not res.has_value()) {
      resp.failed_drbs.push_back(drb.drb_id);
      continue;
    }
  }

  // > Sort bearers by LCID.
  std::sort(ue_mcg.rlc_bearers.begin(), ue_mcg.rlc_bearers.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.lcid < rhs.lcid;
  });

  // > Allocate resources for new or modified cells.
  if (not ue_mcg.cells.contains(0) or ue_mcg.cells[0].serv_cell_cfg.cell_index != pcell_idx) {
    // >> PCell changed. Allocate new PCell resources.
    error_type<std::string> outcome = allocate_cell_resources(ue_index, pcell_idx, SERVING_CELL_PCELL_IDX);
    if (not outcome.has_value()) {
      resp.procedure_error = outcome;
      return resp;
    }
  }
  for (const f1ap_scell_to_setup& sc : upd_req.scells_to_setup) {
    // >> SCells Added/Modified. Allocate new SCell resources.
    if (not allocate_cell_resources(ue_index, sc.cell_index, sc.serv_cell_index).has_value()) {
      resp.failed_scells.push_back(sc.serv_cell_index);
    }
  }

  return resp;
}

void du_ran_resource_manager_impl::deallocate_context(du_ue_index_t ue_index)
{
  srsran_assert(ue_res_pool.contains(ue_index), "This function should only be called for an already allocated UE");
  cell_group_config& ue_mcg = ue_res_pool[ue_index].cg_cfg;

  for (const auto& sc : ue_mcg.cells) {
    deallocate_cell_resources(ue_index, sc.serv_cell_idx);
  }
  ue_res_pool.erase(ue_index);
}

error_type<std::string> du_ran_resource_manager_impl::allocate_cell_resources(du_ue_index_t     ue_index,
                                                                              du_cell_index_t   cell_index,
                                                                              serv_cell_index_t serv_cell_index)
{
  cell_group_config& ue_res = ue_res_pool[ue_index].cg_cfg;

  const du_cell_config& cell_cfg_cmn = cell_cfg_list[cell_index];

  if (serv_cell_index == SERVING_CELL_PCELL_IDX) {
    // It is a PCell.
    srsran_assert(not ue_res.cells.contains(SERVING_CELL_PCELL_IDX), "Reallocation of PCell detected");
    ue_res.cells.emplace(SERVING_CELL_PCELL_IDX);
    ue_res.cells[0].serv_cell_idx            = SERVING_CELL_PCELL_IDX;
    ue_res.cells[0].serv_cell_cfg            = cell_cfg_cmn.ue_ded_serv_cell_cfg;
    ue_res.cells[0].serv_cell_cfg.cell_index = cell_index;
    ue_res.mcg_cfg = config_helpers::make_initial_mac_cell_group_config(cell_cfg_cmn.mcg_params);
    // TODO: Move to helper.
    if (cell_cfg_cmn.pcg_params.p_nr_fr1.has_value()) {
      ue_res.pcg_cfg.p_nr_fr1 = cell_cfg_cmn.pcg_params.p_nr_fr1->to_int();
    }
    ue_res.pcg_cfg.pdsch_harq_codebook = pdsch_harq_ack_codebook::dynamic;

    if (not pucch_res_mng.alloc_resources(ue_res)) {
      return make_unexpected(fmt::format("Unable to allocate dedicated PUCCH resources for cell={}", cell_index));
    }
  } else {
    srsran_assert(not ue_res.cells.contains(serv_cell_index), "Reallocation of SCell detected");
    ue_res.cells.emplace(serv_cell_index);
    ue_res.cells[serv_cell_index].serv_cell_idx            = serv_cell_index;
    ue_res.cells[serv_cell_index].serv_cell_cfg            = cell_cfg_cmn.ue_ded_serv_cell_cfg;
    ue_res.cells[serv_cell_index].serv_cell_cfg.cell_index = cell_index;
    // TODO: Allocate SCell params.
  }
  return {};
}

void du_ran_resource_manager_impl::deallocate_cell_resources(du_ue_index_t ue_index, serv_cell_index_t serv_cell_index)
{
  cell_group_config& ue_res = ue_res_pool[ue_index].cg_cfg;

  // Return resources back to free lists.
  if (serv_cell_index == SERVING_CELL_PCELL_IDX) {
    srsran_assert(not ue_res.cells.empty() and ue_res.cells[0].serv_cell_cfg.cell_index != INVALID_DU_CELL_INDEX,
                  "Double deallocation of same UE cell resources detected");
    pucch_res_mng.dealloc_resources(ue_res);
    ue_res.cells[0].serv_cell_cfg.cell_index = INVALID_DU_CELL_INDEX;
  } else {
    // TODO: Remove of SCell params.
    ue_res.cells.erase(serv_cell_index);
  }
}
