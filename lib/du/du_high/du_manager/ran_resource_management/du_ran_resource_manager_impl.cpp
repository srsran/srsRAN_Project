/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

du_ue_ran_resource_updater_impl::du_ue_ran_resource_updater_impl(du_ue_resource_config* cell_grp_cfg_,
                                                                 const std::optional<ue_capability_summary>& ue_caps_,
                                                                 du_ran_resource_manager_impl&               parent_,
                                                                 du_ue_index_t ue_index_) :
  cell_grp(cell_grp_cfg_), ue_caps(&ue_caps_), parent(&parent_), ue_index(ue_index_)
{
}

du_ue_ran_resource_updater_impl::~du_ue_ran_resource_updater_impl()
{
  parent->deallocate_context(ue_index);
}

du_ue_resource_update_response
du_ue_ran_resource_updater_impl::update(du_cell_index_t                       pcell_index,
                                        const f1ap_ue_context_update_request& upd_req,
                                        const du_ue_resource_config*          reestablished_context,
                                        const ue_capability_summary*          reestablished_ue_caps)
{
  return parent->update_context(ue_index, pcell_index, upd_req, reestablished_context, reestablished_ue_caps);
}

void du_ue_ran_resource_updater_impl::config_applied()
{
  parent->ue_config_applied(ue_index);
}

///////////////////////////

// Helper that resets the PUCCH and SRS configurations in the serving cell configuration.
static void reset_serv_cell_cfg(serving_cell_config& serv_cell_cfg)
{
  srsran_assert(serv_cell_cfg.ul_config.has_value() and
                    serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value() and
                    serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.has_value(),
                "UL configuration in Serving cell config not configured");

  serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.reset();
  if (serv_cell_cfg.csi_meas_cfg.has_value()) {
    serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.clear();
  }

  serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();
}

du_ran_resource_manager_impl::du_ran_resource_manager_impl(span<const du_cell_config>                cell_cfg_list_,
                                                           const scheduler_expert_config&            scheduler_cfg,
                                                           const std::map<srb_id_t, du_srb_config>&  srb_config,
                                                           const std::map<five_qi_t, du_qos_config>& qos_config,
                                                           const du_test_mode_config&                test_cfg_) :
  cell_cfg_list(cell_cfg_list_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  test_cfg(test_cfg_),
  pucch_res_mng(cell_cfg_list, scheduler_cfg.ue.max_pucchs_per_slot),
  bearer_res_mng(srb_config, qos_config, logger),
  srs_res_mng(std::make_unique<du_srs_policy_max_ul_rate>(cell_cfg_list)),
  meas_cfg_mng(cell_cfg_list),
  drx_res_mng(cell_cfg_list),
  ra_res_alloc(cell_cfg_list)
{
  for (const auto& cell : cell_cfg_list) {
    const du_cell_index_t cell_idx  = cell.ue_ded_serv_cell_cfg.cell_index;
    unsigned              sr_limit  = pucch_res_mng.get_nof_sr_free_res_offsets(cell_idx);
    unsigned              csi_limit = 0;
    unsigned              srs_limit = 0;

    unsigned max_nof_ues = sr_limit;
    if (cell.ue_ded_serv_cell_cfg.csi_meas_cfg.has_value()) {
      csi_limit   = pucch_res_mng.get_nof_csi_free_res_offsets(cell_idx);
      max_nof_ues = std::min(max_nof_ues, csi_limit);
    }
    if (cell.srs_cfg.srs_period.has_value()) {
      srs_limit   = srs_res_mng->get_nof_srs_free_res_offsets(cell_idx);
      max_nof_ues = std::min(max_nof_ues, srs_limit);
    }

    logger.info("The upper-bound on the number of UEs supported by cell {{pci={}, du_cell_index={}}} is {} (the actual "
                "number might be lower than that). This is determined by the lowest of the following limits: SR ({}), "
                "CSI ({}) and SRS ({}).",
                cell.pci,
                fmt::underlying(cell_idx),
                max_nof_ues,
                sr_limit,
                cell.ue_ded_serv_cell_cfg.csi_meas_cfg.has_value() ? fmt::to_string(csi_limit) : "n/a",
                cell.srs_cfg.srs_period.has_value() ? fmt::to_string(srs_limit) : "n/a");
  }
}

expected<ue_ran_resource_configurator, std::string>
du_ran_resource_manager_impl::create_ue_resource_configurator(du_ue_index_t   ue_index,
                                                              du_cell_index_t pcell_index,
                                                              bool            has_tc_rnti)
{
  if (ue_res_pool.contains(ue_index)) {
    return make_unexpected(std::string("Double allocation of same UE not supported"));
  }
  ue_res_pool.emplace(ue_index, *this);
  auto& ue_res = ue_res_pool[ue_index];
  auto& mcg    = ue_res.cg_cfg;

  // UE initialized PCell.
  // Note: In case of lack of RAN resource availability, the return will be error type.
  error_type<std::string> err = allocate_cell_resources(ue_index, pcell_index, SERVING_CELL_PCELL_IDX);
  if (not err.has_value()) {
    logger.info("Failed to create a configuration for ue={}. Cause: {}", static_cast<unsigned>(ue_index), err.error());
  }

  // Initialize correct defaults for UE RAN resources dependent on UE capabilities.
  ue_res.ue_cap_manager.handle_ue_creation(ue_res.cg_cfg);

  // Allocate CFRA resources when TC-RNTI was not yet assigned (e.g. during for Handover).
  if (not has_tc_rnti) {
    ra_res_alloc.allocate_cfra_resources(ue_res.cg_cfg);
  }

  return ue_ran_resource_configurator{
      std::make_unique<du_ue_ran_resource_updater_impl>(&mcg, ue_res.ue_cap_manager.summary(), *this, ue_index),
      err.has_value() ? std::string{} : err.error()};
}

du_ue_resource_update_response
du_ran_resource_manager_impl::update_context(du_ue_index_t                         ue_index,
                                             du_cell_index_t                       pcell_idx,
                                             const f1ap_ue_context_update_request& upd_req,
                                             const du_ue_resource_config*          reestablished_context,
                                             const ue_capability_summary*          reestablished_ue_caps)
{
  srsran_assert(ue_res_pool.contains(ue_index), "This function should only be called for an already allocated UE");
  ue_resource_context&           u      = ue_res_pool[ue_index];
  du_ue_resource_config&         ue_mcg = u.cg_cfg;
  du_ue_resource_update_response resp;

  // > Deallocate resources for previously configured cells that have now been removed or changed.
  if (ue_mcg.cell_group.cells.contains(0) and ue_mcg.cell_group.cells[0].serv_cell_cfg.cell_index != pcell_idx) {
    // >> PCell changed. Deallocate PCell resources.
    deallocate_cell_resources(ue_index, SERVING_CELL_PCELL_IDX);
  }
  for (serv_cell_index_t scell_idx : upd_req.scells_to_rem) {
    // >> SCells to be removed. Deallocate them.
    deallocate_cell_resources(ue_index, scell_idx);
  }
  for (const f1ap_scell_to_setup& scell : upd_req.scells_to_setup) {
    // >> If SCells to be modified changed DU Cell Index.
    if (ue_mcg.cell_group.cells.contains(scell.serv_cell_index) and
        ue_mcg.cell_group.cells[scell.serv_cell_index].serv_cell_cfg.cell_index != scell.cell_index) {
      deallocate_cell_resources(ue_index, scell.serv_cell_index);
    }
  }

  // > Allocate resources for new or modified cells.
  if (not ue_mcg.cell_group.cells.contains(0) or ue_mcg.cell_group.cells[0].serv_cell_cfg.cell_index != pcell_idx) {
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

  // Update measGaps based on the UE measConfig.
  meas_cfg_mng.update(ue_mcg, upd_req.meas_cfg);

  // > Process UE NR capabilities and update UE dedicated configuration only if test mode is not configured.
  if (not test_cfg.test_ue.has_value() or test_cfg.test_ue->rnti == rnti_t::INVALID_RNTI) {
    if (reestablished_ue_caps != nullptr) {
      u.ue_cap_manager.update(ue_mcg, *reestablished_ue_caps);
    }
    u.ue_cap_manager.update(ue_mcg, upd_req.ue_cap_rat_list);
  }

  // > Update UE SRBs and DRBs.
  du_ue_bearer_resource_update_response bearer_resp =
      bearer_res_mng.update(ue_mcg,
                            du_ue_bearer_resource_update_request{
                                upd_req.srbs_to_setup, upd_req.drbs_to_setup, upd_req.drbs_to_mod, upd_req.drbs_to_rem},
                            reestablished_context);
  resp.failed_drbs = std::move(bearer_resp.drbs_failed_to_setup);
  resp.failed_drbs.insert(
      resp.failed_drbs.end(), bearer_resp.drbs_failed_to_mod.begin(), bearer_resp.drbs_failed_to_mod.end());

  return resp;
}

void du_ran_resource_manager_impl::deallocate_context(du_ue_index_t ue_index)
{
  srsran_assert(ue_res_pool.contains(ue_index), "This function should only be called for an already allocated UE");
  ue_resource_context&   ue_res = ue_res_pool[ue_index];
  du_ue_resource_config& ue_mcg = ue_res.cg_cfg;

  ra_res_alloc.deallocate_cfra_resources(ue_mcg);

  ue_res.ue_cap_manager.release(ue_mcg);

  for (const auto& sc : ue_mcg.cell_group.cells) {
    deallocate_cell_resources(ue_index, sc.serv_cell_idx);
  }

  ue_res_pool.erase(ue_index);
}

void du_ran_resource_manager_impl::ue_config_applied(du_ue_index_t ue_index)
{
  srsran_assert(ue_res_pool.contains(ue_index), "This function should only be called for an already allocated UE");
  ue_resource_context&   ue_res = ue_res_pool[ue_index];
  du_ue_resource_config& ue_mcg = ue_res.cg_cfg;

  // We can remove previously used CFRA resources, if any.
  ra_res_alloc.deallocate_cfra_resources(ue_mcg);
}

error_type<std::string> du_ran_resource_manager_impl::allocate_cell_resources(du_ue_index_t     ue_index,
                                                                              du_cell_index_t   cell_index,
                                                                              serv_cell_index_t serv_cell_index)
{
  du_ue_resource_config& ue_res = ue_res_pool[ue_index].cg_cfg;

  const du_cell_config& cell_cfg_cmn = cell_cfg_list[cell_index];

  if (serv_cell_index == SERVING_CELL_PCELL_IDX) {
    // It is a PCell.
    srsran_assert(not ue_res.cell_group.cells.contains(SERVING_CELL_PCELL_IDX), "Reallocation of PCell detected");
    ue_res.cell_group.cells.emplace(SERVING_CELL_PCELL_IDX);
    ue_res.cell_group.cells[0].serv_cell_idx            = SERVING_CELL_PCELL_IDX;
    ue_res.cell_group.cells[0].serv_cell_cfg            = cell_cfg_cmn.ue_ded_serv_cell_cfg;
    ue_res.cell_group.cells[0].serv_cell_cfg.cell_index = cell_index;
    ue_res.cell_group.mcg_cfg = config_helpers::make_initial_mac_cell_group_config(cell_cfg_cmn.mcg_params);
    // TODO: Move to helper.
    if (cell_cfg_cmn.pcg_params.p_nr_fr1.has_value()) {
      ue_res.cell_group.pcg_cfg.p_nr_fr1 = cell_cfg_cmn.pcg_params.p_nr_fr1->to_int();
    }
    ue_res.cell_group.pcg_cfg.pdsch_harq_codebook = pdsch_harq_ack_codebook::dynamic;

    // Start with removing PUCCH and SRS configurations. This step simplifies the handling of the allocation failure
    // path.
    reset_serv_cell_cfg(ue_res.cell_group.cells[0].serv_cell_cfg);

    if (not srs_res_mng->alloc_resources(ue_res.cell_group)) {
      // Deallocate dedicated Search Spaces.
      ue_res.cell_group.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces.clear();
      return make_unexpected(fmt::format("Unable to allocate SRS resources for cell={}", fmt::underlying(cell_index)));
    }

    if (not pucch_res_mng.alloc_resources(ue_res.cell_group)) {
      // Deallocate previously allocated SRS + dedicated Search Spaces.
      srs_res_mng->dealloc_resources(ue_res.cell_group);
      ue_res.cell_group.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces.clear();
      return make_unexpected(
          fmt::format("Unable to allocate dedicated PUCCH resources for cell={}", fmt::underlying(cell_index)));
    }

  } else {
    srsran_assert(not ue_res.cell_group.cells.contains(serv_cell_index), "Reallocation of SCell detected");
    ue_res.cell_group.cells.emplace(serv_cell_index);
    ue_res.cell_group.cells[serv_cell_index].serv_cell_idx            = serv_cell_index;
    ue_res.cell_group.cells[serv_cell_index].serv_cell_cfg            = cell_cfg_cmn.ue_ded_serv_cell_cfg;
    ue_res.cell_group.cells[serv_cell_index].serv_cell_cfg.cell_index = cell_index;
    // TODO: Allocate SCell params.
  }
  return {};
}

void du_ran_resource_manager_impl::deallocate_cell_resources(du_ue_index_t ue_index, serv_cell_index_t serv_cell_index)
{
  ue_resource_context&   ue_res_updater = ue_res_pool[ue_index];
  du_ue_resource_config& ue_res         = ue_res_updater.cg_cfg;

  // Return resources back to free lists.
  if (serv_cell_index == SERVING_CELL_PCELL_IDX) {
    srsran_assert(not ue_res.cell_group.cells.empty() and
                      ue_res.cell_group.cells[0].serv_cell_cfg.cell_index != INVALID_DU_CELL_INDEX,
                  "Double deallocation of same UE cell resources detected");
    pucch_res_mng.dealloc_resources(ue_res.cell_group);
    srs_res_mng->dealloc_resources(ue_res.cell_group);
    ue_res.cell_group.cells[0].serv_cell_cfg.cell_index = INVALID_DU_CELL_INDEX;
  } else {
    // TODO: Remove of SCell params.
    ue_res.cell_group.cells.erase(serv_cell_index);
  }
}

du_ran_resource_manager_impl::ue_resource_context::ue_resource_context(du_ran_resource_manager_impl& parent) :
  ue_cap_manager(parent.cell_cfg_list, parent.drx_res_mng, parent.logger, parent.test_cfg)
{
}
