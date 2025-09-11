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

#include "ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/security_manager_config.h"

using namespace srsran;
using namespace srs_cu_cp;

void cu_cp_ue::stop()
{
  task_sched.stop();
}

ue_manager::ue_manager(const cu_cp_configuration& cu_cp_cfg) :
  cu_cp_config(cu_cp_cfg),
  ue_config(cu_cp_cfg.ue),
  up_config(up_resource_manager_cfg{cu_cp_cfg.bearers.drb_config, cu_cp_cfg.admission.max_nof_drbs_per_ue}),
  sec_config(security_manager_config{cu_cp_cfg.security.int_algo_pref_list, cu_cp_cfg.security.enc_algo_pref_list}),
  max_nof_ues(cu_cp_cfg.admission.max_nof_ues),
  ue_task_scheds(*cu_cp_cfg.services.timers, *cu_cp_cfg.services.cu_cp_executor, logger)
{
}

void ue_manager::stop()
{
  ue_task_scheds.stop();
}

ue_index_t ue_manager::add_ue(du_index_t                     du_index,
                              plmn_identity                  plmn,
                              std::optional<gnb_du_id_t>     du_id,
                              std::optional<pci_t>           pci,
                              std::optional<rnti_t>          rnti,
                              std::optional<du_cell_index_t> pcell_index)
{
  if (blocked_plmns.find(plmn) != blocked_plmns.end()) {
    logger.warning("CU-CP UE creation Failed. Cause: UE connections for PLMN {} are currently not allowed", plmn);
    return ue_index_t::invalid;
  }

  if (du_index == du_index_t::invalid) {
    logger.warning("CU-CP UE creation Failed. Cause: Invalid DU index={}", du_index);
    return ue_index_t::invalid;
  }

  if (du_id.has_value() && du_id.value() == gnb_du_id_t::invalid) {
    logger.warning("CU-CP UE creation Failed. Cause: Invalid gNB-DU ID={}", fmt::underlying(du_id.value()));
    return ue_index_t::invalid;
  }

  if (pci.has_value() && pci.value() == INVALID_PCI) {
    logger.warning("CU-CP UE creation Failed. Cause: Invalid pci={}", pci.value());
    return ue_index_t::invalid;
  }

  if (rnti.has_value() && rnti.value() == rnti_t::INVALID_RNTI) {
    logger.warning("CU-CP UE creation Failed. Cause: Invalid rnti={}", rnti.value());
    return ue_index_t::invalid;
  }

  if (pcell_index.has_value() && pcell_index.value() == du_cell_index_t::invalid) {
    logger.warning("CU-CP UE creation Failed. Cause: Invalid pcell_index={}", pcell_index.value());
    return ue_index_t::invalid;
  }

  if (ues.size() == max_nof_ues) {
    logger.warning(
        "CU-CP UE creation Failed. Cause: Maximum number of UEs supported by the CU-CP ({}) has been reached",
        max_nof_ues);
    fmt::print("CU-CP UE creation failed. Cause: Maximum number of UEs supported by the CU-CP ({}) has been reached. "
               "To increase the number of supported "
               "UEs change the \"--max_nof_ues\" in the CU-CP configuration\n",
               max_nof_ues);
    return ue_index_t::invalid;
  }

  ue_index_t new_ue_index = allocate_ue_index();
  if (new_ue_index == ue_index_t::invalid) {
    logger.warning("CU-CP UE creation Failed. Cause: No free UE index available");
    return ue_index_t::invalid;
  }

  // Create a dedicated task scheduler for the UE.
  ue_task_scheduler_impl ue_sched = ue_task_scheds.create_ue_task_sched(new_ue_index);

  // Create UE object
  ues.emplace(std::piecewise_construct,
              std::forward_as_tuple(new_ue_index),
              std::forward_as_tuple(new_ue_index,
                                    du_index,
                                    *cu_cp_config.services.timers,
                                    *cu_cp_config.services.cu_cp_executor,
                                    up_config,
                                    sec_config,
                                    std::move(ue_sched),
                                    plmn,
                                    du_id,
                                    pci,
                                    rnti,
                                    pcell_index));

  // Add PCI and RNTI to lookup.
  if (pci.has_value() && rnti.has_value()) {
    pci_rnti_to_ue_index.emplace(std::make_tuple(pci.value(), rnti.value()), new_ue_index);
  }

  logger.info("ue={} du_index={} plmn={}{}{}{}{}: Created new CU-CP UE",
              new_ue_index,
              du_index,
              plmn,
              du_id.has_value() ? fmt::format(" gnb_du_id={}", fmt::underlying(du_id.value())) : "",
              pci.has_value() ? fmt::format(" pci={}", pci.value()) : "",
              rnti.has_value() ? fmt::format(" rnti={}", rnti.value()) : "",
              pcell_index.has_value() ? fmt::format(" pcell_index={}", pcell_index.value()) : "");

  return new_ue_index;
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  if (ue_index == ue_index_t::invalid) {
    logger.warning("Can't remove UE with invalid UE index");
    return;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Remove UE called for inexistent UE", ue_index);
    return;
  }

  // remove UE from lookups
  pci_t pci = ues.at(ue_index).get_pci();
  if (pci != INVALID_PCI) {
    rnti_t c_rnti = ues.at(ue_index).get_c_rnti();
    if (c_rnti != rnti_t::INVALID_RNTI) {
      pci_rnti_to_ue_index.erase(std::make_tuple(pci, c_rnti));
    } else {
      logger.warning("ue={}: RNTI not found", ue_index);
    }
  } else {
    logger.debug("ue={}: PCI not found", ue_index);
  }

  // Remove CU-CP UE from database
  ues.erase(ue_index);

  logger.debug("ue={}: Removed", ue_index);
}

void ue_manager::add_blocked_plmns(const std::vector<plmn_identity>& plmns)
{
  for (const auto& plmn : plmns) {
    if (blocked_plmns.insert(plmn).second) {
      logger.info("Blocking new UE connections for PLMN {}", plmn);
    }
  }
}

void ue_manager::remove_blocked_plmns(const std::vector<plmn_identity>& plmns)
{
  for (const auto& plmn : plmns) {
    if (blocked_plmns.erase(plmn)) {
      logger.info("Re-allowing new UE connections for PLMN {}", plmn);
    }
  }
}

std::vector<cu_cp_ue*> ue_manager::find_ues(plmn_identity plmn)
{
  std::vector<cu_cp_ue*> found_ues;
  for (auto& ue : ues) {
    if (ue.second.get_ue_context().plmn == plmn) {
      found_ues.push_back(&ue.second);
    }
  }

  return found_ues;
}

ue_index_t ue_manager::get_ue_index(pci_t pci, rnti_t rnti)
{
  if (pci_rnti_to_ue_index.find(std::make_tuple(pci, rnti)) != pci_rnti_to_ue_index.end()) {
    return pci_rnti_to_ue_index.at(std::make_tuple(pci, rnti));
  }
  logger.debug("UE index for pci={} and rnti={} not found", pci, rnti);
  return ue_index_t::invalid;
}

// common

cu_cp_ue* ue_manager::find_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end()) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

ue_task_scheduler* ue_manager::find_ue_task_scheduler(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).du_ue_created()) {
    return &ues.at(ue_index).get_task_sched();
  }
  return nullptr;
}

// du processor

cu_cp_ue* ue_manager::set_ue_du_context(ue_index_t      ue_index,
                                        gnb_du_id_t     du_id,
                                        pci_t           pci,
                                        rnti_t          rnti,
                                        du_cell_index_t pcell_index)
{
  srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  srsran_assert(pci != INVALID_PCI, "Invalid pci={}", pci);
  srsran_assert(rnti != rnti_t::INVALID_RNTI, "Invalid rnti={}", rnti);
  srsran_assert(pcell_index != du_cell_index_t::invalid, "Invalid pcell_index={}", pcell_index);

  // check if ue_index is in db
  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: UE not found", ue_index);
    return nullptr;
  }

  // check if the UE is already present
  if (get_ue_index(pci, rnti) != ue_index_t::invalid) {
    logger.warning("UE with pci={} and rnti={} already exists", pci, rnti);
    return nullptr;
  }

  auto& ue = ues.at(ue_index);
  ue.update_du_ue(du_id, pci, rnti, pcell_index);

  // Add PCI and RNTI to lookup.
  pci_rnti_to_ue_index.emplace(std::make_tuple(pci, rnti), ue_index);

  logger.debug("ue={}: Updated UE with gnb_du_id={} pci={} rnti={} pcell_index={}",
               fmt::underlying(ue_index),
               fmt::underlying(du_id),
               pci,
               rnti,
               fmt::underlying(pcell_index));

  return &ue;
}

cu_cp_ue* ue_manager::find_du_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).du_ue_created()) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

size_t ue_manager::get_nof_du_ues(du_index_t du_index)
{
  unsigned ue_count = 0;
  // Count UEs connected to the DU
  for (const auto& ue : ues) {
    if (ue.second.get_du_index() == du_index) {
      ue_count++;
    }
  }

  return ue_count;
}

std::vector<cu_cp_metrics_report::ue_info> ue_manager::handle_ue_metrics_report_request() const
{
  if (!cu_cp_config.metrics.layers_cfg.enable_rrc) {
    return {};
  }

  std::vector<cu_cp_metrics_report::ue_info> report;
  report.reserve(ues.size());

  for (const auto& ue : ues) {
    report.emplace_back();
    cu_cp_metrics_report::ue_info& ue_report = report.back();

    ue_report.rnti  = ue.second.get_c_rnti();
    ue_report.du_id = ue.second.get_du_id();
    ue_report.pci   = ue.second.get_pci();

    if (ue.second.get_rrc_ue() == nullptr) {
      ue_report.rrc_connection_state = rrc_state::idle;
    } else {
      ue_report.rrc_connection_state = ue.second.get_rrc_ue()->get_rrc_ue_control_message_handler().get_rrc_state();
    }
  }

  return report;
}

// private functions

ue_index_t ue_manager::allocate_ue_index()
{
  // return invalid when no UE index is available
  if (ues.size() == max_nof_ues) {
    return ue_index_t::invalid;
  }

  // Check if the next_ue_index is available
  if (ues.find(next_ue_index) == ues.end()) {
    ue_index_t ret = next_ue_index;
    // increase the next_ue_index
    increase_next_ue_index();
    return ret;
  }

  // Find holes in the allocated IDs by iterating over all ids starting with the next_ue_index to find the
  // available id
  while (true) {
    // increase the next_ue_index and try again
    increase_next_ue_index();

    // return the id if it is not already used
    if (ues.find(next_ue_index) == ues.end()) {
      ue_index_t ret = next_ue_index;
      // increase the next_ue_index
      increase_next_ue_index();
      return ret;
    }
  }

  return ue_index_t::invalid;
}
