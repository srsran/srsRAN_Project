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

#include "cu_up_processor_repository.h"
#include "cu_up_processor_config.h"
#include "cu_up_processor_factory.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_processor_repository::cu_up_processor_repository(cu_up_repository_config cfg_) :
  cfg(cfg_),
  logger(cfg.logger),
  cu_up_task_sched(*cfg.cu_cp.services.timers,
                   *cfg.cu_cp.services.cu_cp_executor,
                   cfg.cu_cp.admission.max_nof_cu_ups,
                   logger)
{
}

cu_up_index_t cu_up_processor_repository::add_cu_up(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier)
{
  cu_up_index_t cu_up_index = allocate_cu_up_index();
  if (cu_up_index == cu_up_index_t::invalid) {
    logger.warning("CU-UP connection failed - maximum number of CU-UPs connected ({})",
                   cfg.cu_cp.admission.max_nof_cu_ups);
    return cu_up_index_t::invalid;
  }

  // Create CU-UP object
  auto it = cu_up_db.insert(std::make_pair(cu_up_index, cu_up_context{}));
  srsran_assert(it.second, "Unable to insert CU-UP in map");
  cu_up_context& cu_up_ctxt       = it.first->second;
  cu_up_ctxt.e1ap_tx_pdu_notifier = std::move(e1ap_tx_pdu_notifier);

  // TODO: use real config
  cu_up_processor_config_t cu_up_cfg = {};
  cu_up_cfg.cu_up_index              = cu_up_index;
  cu_up_cfg.max_nof_supported_ues    = cfg.cu_cp.admission.max_nof_ues;

  std::unique_ptr<cu_up_processor_impl_interface> cu_up = create_cu_up_processor(std::move(cu_up_cfg),
                                                                                 *cu_up_ctxt.e1ap_tx_pdu_notifier,
                                                                                 cfg.e1ap_ev_notifier,
                                                                                 cu_up_task_sched,
                                                                                 *cfg.cu_cp.services.cu_cp_executor);

  srsran_assert(cu_up != nullptr, "Failed to create CU-UP processor");
  cu_up_ctxt.processor = std::move(cu_up);

  return cu_up_index;
}

cu_up_index_t cu_up_processor_repository::allocate_cu_up_index()
{
  for (unsigned cu_up_index_int = cu_up_index_to_uint(cu_up_index_t::min);
       cu_up_index_int < cfg.cu_cp.admission.max_nof_cu_ups;
       cu_up_index_int++) {
    cu_up_index_t cu_up_index = uint_to_cu_up_index(cu_up_index_int);
    if (cu_up_db.find(cu_up_index) == cu_up_db.end()) {
      return cu_up_index;
    }
  }
  return cu_up_index_t::invalid;
}

async_task<void> cu_up_processor_repository::remove_cu_up(cu_up_index_t cu_up_index)
{
  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  logger.debug("Removing CU-UP {}...", cu_up_index);

  return launch_async([this, cu_up_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Remove CU-UP
    if (cu_up_db.find(cu_up_index) == cu_up_db.end()) {
      logger.warning("Remove CU-UP called for non-existent cu_up_index={}", cu_up_index);
      return;
    }

    // Stop CU-UP activity, eliminating pending transactions for the CU-UP and respective UEs.
    // TODO

    // Remove CU-UP
    removed_cu_up_db.insert(std::make_pair(cu_up_index, std::move(cu_up_db.at(cu_up_index))));
    cu_up_db.erase(cu_up_index);

    // Remove CU-UP
    logger.info("Removed CU-UP {}", cu_up_index);

    CORO_RETURN();
  });
}

cu_up_processor_e1ap_interface& cu_up_processor_repository::get_cu_up(cu_up_index_t cu_up_index)
{
  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  srsran_assert(cu_up_db.find(cu_up_index) != cu_up_db.end(), "CU-UP not found cu_up_index={}", cu_up_index);
  return *cu_up_db.at(cu_up_index).processor;
}

cu_up_processor_impl_interface* cu_up_processor_repository::find_cu_up_processor(cu_up_index_t cu_up_index)
{
  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  if (cu_up_db.find(cu_up_index) == cu_up_db.end()) {
    return nullptr;
  }
  return cu_up_db.at(cu_up_index).processor.get();
}

size_t cu_up_processor_repository::get_nof_e1ap_ues()
{
  size_t nof_ues = 0;
  for (auto& cu_up : cu_up_db) {
    nof_ues += cu_up.second.processor->get_e1ap_statistics_handler().get_nof_ues();
  }
  return nof_ues;
}
