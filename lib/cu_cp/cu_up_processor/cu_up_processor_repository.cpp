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

namespace {

class e1ap_rx_pdu_notifier final : public e1ap_message_notifier
{
public:
  e1ap_rx_pdu_notifier(cu_cp_e1_handler& parent_, cu_up_index_t cu_up_index_) :
    parent(&parent_),
    cu_up_index(cu_up_index_),
    cached_msg_handler(parent->get_cu_up(cu_up_index).get_e1ap_message_handler())
  {
  }
  e1ap_rx_pdu_notifier(const e1ap_rx_pdu_notifier&)            = delete;
  e1ap_rx_pdu_notifier(e1ap_rx_pdu_notifier&&)                 = delete;
  e1ap_rx_pdu_notifier& operator=(const e1ap_rx_pdu_notifier&) = delete;
  e1ap_rx_pdu_notifier& operator=(e1ap_rx_pdu_notifier&&)      = delete;

  ~e1ap_rx_pdu_notifier()
  {
    if (parent != nullptr) {
      parent->handle_cu_up_remove_request(cu_up_index);
    }
  }

  void on_new_message(const e1ap_message& msg) override { cached_msg_handler.handle_message(msg); }

private:
  cu_cp_e1_handler*     parent;
  cu_up_index_t         cu_up_index;
  e1ap_message_handler& cached_msg_handler;
};

} // namespace

cu_up_processor_repository::cu_up_processor_repository(cu_up_repository_config cfg_) :
  cfg(cfg_),
  logger(cfg.logger),
  cu_up_task_sched(*cfg.cu_cp.timers, *cfg.cu_cp.cu_cp_executor, cfg.cu_cp.max_nof_cu_ups, logger)
{
}

std::unique_ptr<e1ap_message_notifier>
cu_up_processor_repository::handle_new_cu_up_connection(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier)
{
  cu_up_index_t cu_up_index = add_cu_up(std::move(e1ap_tx_pdu_notifier));
  if (cu_up_index == cu_up_index_t::invalid) {
    logger.warning("Rejecting new CU-UP connection. Cause: Failed to create a new CU-UP");
    return nullptr;
  }

  logger.info("Added CU-UP {}", cu_up_index);

  return std::make_unique<e1ap_rx_pdu_notifier>(*this, cu_up_index);
}

void cu_up_processor_repository::handle_cu_up_remove_request(cu_up_index_t cu_up_index)
{
  logger.debug("Removing CU-UP {}...", cu_up_index);
  remove_cu_up(cu_up_index);
}

cu_up_index_t cu_up_processor_repository::add_cu_up(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier)
{
  cu_up_index_t cu_up_index = allocate_cu_up_index();
  if (cu_up_index == cu_up_index_t::invalid) {
    logger.warning("CU-UP connection failed - maximum number of CU-UPs connected ({})", cfg.cu_cp.max_nof_cu_ups);
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
  cu_up_cfg.max_nof_supported_ues    = cfg.cu_cp.max_nof_dus * MAX_NOF_UES_PER_DU;

  std::unique_ptr<cu_up_processor_impl_interface> cu_up = create_cu_up_processor(std::move(cu_up_cfg),
                                                                                 *cu_up_ctxt.e1ap_tx_pdu_notifier,
                                                                                 cfg.e1ap_ev_notifier,
                                                                                 cu_up_task_sched,
                                                                                 *cfg.cu_cp.cu_cp_executor);

  srsran_assert(cu_up != nullptr, "Failed to create CU-UP processor");
  cu_up_ctxt.cu_up_processor = std::move(cu_up);

  // Notify CU-CP about E1AP creation
  cfg.e1ap_ev_notifier.on_e1ap_created(cu_up_ctxt.cu_up_processor->get_e1ap_bearer_context_manager(),
                                       cu_up_ctxt.cu_up_processor->get_e1ap_bearer_context_removal_handler(),
                                       cu_up_ctxt.cu_up_processor->get_e1ap_statistics_handler());

  return cu_up_index;
}

cu_up_index_t cu_up_processor_repository::allocate_cu_up_index()
{
  for (unsigned cu_up_index_int = cu_up_index_to_uint(cu_up_index_t::min); cu_up_index_int < cfg.cu_cp.max_nof_cu_ups;
       cu_up_index_int++) {
    cu_up_index_t cu_up_index = uint_to_cu_up_index(cu_up_index_int);
    if (cu_up_db.find(cu_up_index) == cu_up_db.end()) {
      return cu_up_index;
    }
  }
  return cu_up_index_t::invalid;
}

void cu_up_processor_repository::remove_cu_up(cu_up_index_t cu_up_index)
{
  // Note: The caller of this function can be a CU-UP procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the DU. This is achieved via a scheduled async task

  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  logger.debug("Scheduling cu_up_index={} deletion", cu_up_index);

  // Schedule CU-UP removal task
  cu_up_task_sched.handle_cu_up_async_task(
      cu_up_index, launch_async([this, cu_up_index](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        auto du_it = cu_up_db.find(cu_up_index);
        if (du_it == cu_up_db.end()) {
          logger.warning("Remove CU-UP called for inexistent cu_up_index={}", cu_up_index);
          CORO_EARLY_RETURN();
        }

        // Remove DU
        // TODO
        removed_cu_up_db.insert(std::make_pair(cu_up_index, std::move(cu_up_db.at(cu_up_index))));
        cu_up_db.erase(cu_up_index);

        logger.info("Removed CU-UP {}", cu_up_index);

        CORO_RETURN();
      }));
}

cu_up_processor_impl_interface& cu_up_processor_repository::find_cu_up(cu_up_index_t cu_up_index)
{
  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  srsran_assert(cu_up_db.find(cu_up_index) != cu_up_db.end(), "CU-UP not found cu_up_index={}", cu_up_index);
  return *cu_up_db.at(cu_up_index).cu_up_processor;
}

cu_up_handler& cu_up_processor_repository::get_cu_up(cu_up_index_t cu_up_index)
{
  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  srsran_assert(cu_up_db.find(cu_up_index) != cu_up_db.end(), "CU-UP not found cu_up_index={}", cu_up_index);
  return cu_up_db.at(cu_up_index);
}

e1ap_message_handler& cu_up_processor_repository::cu_up_context::get_e1ap_message_handler()
{
  return cu_up_processor->get_e1ap_message_handler();
}

void cu_up_processor_repository::cu_up_context::update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index)
{
  return cu_up_processor->update_ue_index(ue_index, old_ue_index);
}
