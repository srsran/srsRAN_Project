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

#include "du_processor_repository.h"
#include "du_processor_config.h"
#include "du_processor_factory.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/rrc/rrc_config.h"
#include "srsran/support/executors/sync_task_executor.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

du_processor_repository::du_processor_repository(du_repository_config cfg_) :
  cfg(cfg_), logger(cfg.logger), du_cfg_mng(cfg.cu_cp.node.gnb_id, cfg.cu_cp.node.plmn)
{
}

du_index_t du_processor_repository::add_du(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier)
{
  du_index_t du_index = get_next_du_index();
  if (du_index == du_index_t::invalid) {
    logger.warning("DU connection failed - maximum number of DUs connected ({})", cfg.cu_cp.admission.max_nof_dus);
    return du_index_t::invalid;
  }

  // Create DU object
  auto it = du_db.insert(std::make_pair(du_index, du_context{}));
  srsran_assert(it.second, "Unable to insert DU in map");
  du_context& du_ctxt = it.first->second;
  du_ctxt.du_to_cu_cp_notifier.connect_cu_cp(cfg.cu_cp_du_handler, cfg.ue_removal_handler, cfg.ue_context_handler);
  du_ctxt.f1ap_tx_pdu_notifier = std::move(f1ap_tx_pdu_notifier);

  du_processor_config_t du_cfg     = {du_index, cfg.cu_cp, logger, &cfg.du_conn_notif, du_cfg_mng.create_du_handler()};
  std::unique_ptr<du_processor> du = create_du_processor(std::move(du_cfg),
                                                         du_ctxt.du_to_cu_cp_notifier,
                                                         *du_ctxt.f1ap_tx_pdu_notifier,
                                                         cfg.ue_nas_pdu_notifier,
                                                         cfg.ue_ngap_ctrl_notifier,
                                                         cfg.meas_config_notifier,
                                                         cfg.common_task_sched,
                                                         cfg.ue_mng);

  srsran_assert(du != nullptr, "Failed to create DU processor");
  du_ctxt.processor = std::move(du);

  return du_index;
}

async_task<void> du_processor_repository::remove_du(du_index_t du_index)
{
  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  logger.debug("Removing DU {}...", du_index);

  return launch_async([this, du_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Remove DU
    if (du_db.find(du_index) == du_db.end()) {
      logger.warning("Remove DU called for inexistent du_index={}", du_index);
      return;
    }

    // Stop DU activity, eliminating pending transactions for the DU and respective UEs.
    CORO_AWAIT(du_db.find(du_index)->second.processor->get_f1ap_handler().stop());

    // Notify the CU-CP about the removal of the DU processor.
    cfg.cu_cp_du_handler.handle_du_processor_removal(du_index);

    // Remove DU
    du_db.erase(du_index);
    logger.info("Removed DU {}", du_index);

    CORO_RETURN();
  });
}

du_index_t du_processor_repository::get_next_du_index()
{
  for (unsigned du_idx_int = du_index_to_uint(du_index_t::min); du_idx_int < cfg.cu_cp.admission.max_nof_dus;
       du_idx_int++) {
    du_index_t du_idx = uint_to_du_index(du_idx_int);
    if (du_db.find(du_idx) == du_db.end()) {
      return du_idx;
    }
  }
  return du_index_t::invalid;
}

du_processor& du_processor_repository::find_du(du_index_t du_index)
{
  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  srsran_assert(du_db.find(du_index) != du_db.end(), "DU not found du_index={}", du_index);
  return *du_db.at(du_index).processor;
}

du_index_t du_processor_repository::find_du(pci_t pci)
{
  du_index_t index = du_index_t::invalid;
  for (const auto& du : du_db) {
    if (du.second.processor->has_cell(pci)) {
      return du.first;
    }
  }

  return index;
}

du_index_t du_processor_repository::find_du(const nr_cell_global_id_t& cgi)
{
  du_index_t index = du_index_t::invalid;
  for (const auto& du : du_db) {
    if (du.second.processor->has_cell(cgi)) {
      return du.first;
    }
  }

  return index;
}

du_processor& du_processor_repository::get_du_processor(du_index_t du_index)
{
  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  srsran_assert(du_db.find(du_index) != du_db.end(), "DU not found du_index={}", du_index);
  return *du_db.at(du_index).processor;
}

std::vector<metrics_report::du_info> du_processor_repository::handle_du_metrics_report_request() const
{
  std::vector<metrics_report::du_info> du_reports;
  for (auto& du : du_db) {
    du_reports.emplace_back(du.second.processor->get_metrics_handler().handle_du_metrics_report_request());
  }
  return du_reports;
}

size_t du_processor_repository::get_nof_f1ap_ues()
{
  size_t nof_ues = 0;
  for (auto& du : du_db) {
    nof_ues += du.second.processor->get_f1ap_handler().get_nof_ues();
  }
  return nof_ues;
}
