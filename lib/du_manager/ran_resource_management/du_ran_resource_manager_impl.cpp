/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "du_ran_resource_manager_impl.h"
#include "srsran/mac/config/mac_cell_group_config_factory.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/rlc/rlc_srb_config_factory.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"

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
                                                                       const f1ap_ue_context_update_request& upd_req)
{
  return parent->update_context(ue_index, pcell_index, upd_req);
}

///////////////////////////

du_ran_resource_manager_impl::du_ran_resource_manager_impl(span<const du_cell_config>                cell_cfg_list_,
                                                           const scheduler_expert_config&            scheduler_cfg,
                                                           const std::map<srb_id_t, du_srb_config>&  srbs_,
                                                           const std::map<five_qi_t, du_qos_config>& qos_) :
  cell_cfg_list(cell_cfg_list_),
  sched_cfg(scheduler_cfg),
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
  if (err.is_error()) {
    ue_res_pool.erase(ue_index);
    return ue_ran_resource_configurator{std::unique_ptr<du_ue_ran_resource_updater_impl>{nullptr}, err.error()};
  }

  return ue_ran_resource_configurator{std::make_unique<du_ue_ran_resource_updater_impl>(&mcg, *this, ue_index)};
}

du_ue_resource_update_response
du_ran_resource_manager_impl::update_context(du_ue_index_t                         ue_index,
                                             du_cell_index_t                       pcell_idx,
                                             const f1ap_ue_context_update_request& upd_req)
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

  // > Deallocate removed SRBs / DRBs.
  for (drb_id_t drb_id : upd_req.drbs_to_rem) {
    auto it = std::find_if(ue_mcg.rlc_bearers.begin(), ue_mcg.rlc_bearers.end(), [drb_id](const rlc_bearer_config& b) {
      return b.drb_id == drb_id;
    });
    ue_mcg.rlc_bearers.erase(it);
  }

  // > Allocate new or modified bearers.
  for (srb_id_t srb_id : upd_req.srbs_to_setup) {
    // >> New or Modified SRB.
    lcid_t lcid = srb_id_to_lcid(srb_id);
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
  for (const f1ap_drb_to_setup& drb : upd_req.drbs_to_setup) {
    // >> New or Modified DRB.
    lcid_t lcid;
    if (drb.lcid.has_value()) {
      // >> CU-assigned LCID.
      lcid = *drb.lcid;
      if (std::any_of(ue_mcg.rlc_bearers.begin(), ue_mcg.rlc_bearers.end(), [&drb](const auto& item) {
            return *drb.lcid == item.lcid;
          })) {
        logger.warning("Failed to allocate {}. Cause: Specified lcid={} already exists", drb.drb_id, lcid);
        resp.failed_drbs.push_back(drb.drb_id);
        continue;
      }
    } else {
      // >> Allocate LCID if not specified by F1AP.
      lcid = find_empty_lcid(ue_mcg.rlc_bearers);
      if (lcid > LCID_MAX_DRB) {
        logger.warning("Failed to allocate {}. Cause: No available LCIDs", drb.drb_id);
        resp.failed_drbs.push_back(drb.drb_id);
        continue;
      }
    }

    // >> Get RLC config from 5QI
    if (qos_config.find(drb.five_qi) == qos_config.end()) {
      logger.warning("Failed to allocate {}. Cause: No {} configured", drb.drb_id, drb.five_qi);
      resp.failed_drbs.push_back(drb.drb_id);
      continue;
    }
    logger.debug("Getting RLC and MAC config for {} from {}", drb.drb_id, drb.five_qi);
    const du_qos_config& qos = qos_config.at(drb.five_qi);

    ue_mcg.rlc_bearers.emplace_back();
    ue_mcg.rlc_bearers.back().lcid    = lcid;
    ue_mcg.rlc_bearers.back().drb_id  = drb.drb_id;
    ue_mcg.rlc_bearers.back().rlc_cfg = qos.rlc;
    ue_mcg.rlc_bearers.back().mac_cfg = qos.mac;
  }
  // >> Sort by LCID.
  std::sort(ue_mcg.rlc_bearers.begin(), ue_mcg.rlc_bearers.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.lcid < rhs.lcid;
  });

  // > Allocate resources for new or modified cells.
  if (not ue_mcg.cells.contains(0) or ue_mcg.cells[0].serv_cell_cfg.cell_index != pcell_idx) {
    // >> PCell changed. Allocate new PCell resources.
    error_type<std::string> outcome = allocate_cell_resources(ue_index, pcell_idx, SERVING_CELL_PCELL_IDX);
    if (outcome.is_error()) {
      resp.procedure_error = outcome;
      return resp;
    }
  }
  for (const f1ap_scell_to_setup& sc : upd_req.scells_to_setup) {
    // >> SCells Added/Modified. Allocate new SCell resources.
    if (allocate_cell_resources(ue_index, sc.cell_index, sc.serv_cell_index).is_error()) {
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

  if (serv_cell_index == SERVING_CELL_PCELL_IDX) {
    // It is a PCell.
    srsran_assert(not ue_res.cells.contains(0), "Reallocation of PCell detected");
    ue_res.cells.emplace(to_du_cell_index(0));
    ue_res.cells[0].serv_cell_idx            = SERVING_CELL_PCELL_IDX;
    ue_res.cells[0].serv_cell_cfg            = cell_cfg_list[0].ue_ded_serv_cell_cfg;
    ue_res.cells[0].serv_cell_cfg.cell_index = cell_index;
    ue_res.mcg_cfg = config_helpers::make_initial_mac_cell_group_config(cell_cfg_list[0].mcg_params);
    // TODO: Move to helper.
    if (cell_cfg_list[0].pcg_params.p_nr_fr1.has_value()) {
      ue_res.pcg_cfg.p_nr_fr1 = cell_cfg_list[0].pcg_params.p_nr_fr1->to_int();
    }
    ue_res.pcg_cfg.pdsch_harq_codebook = pdsch_harq_ack_codebook::dynamic;

    if (not pucch_res_mng.alloc_resources(ue_res)) {
      return error_type<std::string>(
          fmt::format("Unable to allocate dedicated PUCCH resources for cell={}", cell_index));
    }
  } else {
    srsran_assert(not ue_res.cells.contains(serv_cell_index), "Reallocation of SCell detected");
    ue_res.cells.emplace(serv_cell_index);
    ue_res.cells[serv_cell_index].serv_cell_idx            = serv_cell_index;
    ue_res.cells[serv_cell_index].serv_cell_cfg            = cell_cfg_list[serv_cell_index].ue_ded_serv_cell_cfg;
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
