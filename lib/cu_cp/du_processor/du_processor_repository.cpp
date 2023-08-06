/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "du_processor_factory.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/du_processor_config.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

class f1ap_rx_pdu_notifier final : public f1ap_message_notifier
{
public:
  f1ap_rx_pdu_notifier(du_repository& parent_, du_index_t du_index_) :
    parent(&parent_), du_index(du_index_), cached_msg_handler(parent->get_du(du_index).get_f1ap_message_handler())
  {
  }
  f1ap_rx_pdu_notifier(const f1ap_rx_pdu_notifier&)            = delete;
  f1ap_rx_pdu_notifier(f1ap_rx_pdu_notifier&&)                 = delete;
  f1ap_rx_pdu_notifier& operator=(const f1ap_rx_pdu_notifier&) = delete;
  f1ap_rx_pdu_notifier& operator=(f1ap_rx_pdu_notifier&&)      = delete;

  ~f1ap_rx_pdu_notifier()
  {
    if (parent != nullptr) {
      parent->handle_du_remove_request(du_index);
    }
  }

  void on_new_message(const f1ap_message& msg) override { cached_msg_handler.handle_message(msg); }

private:
  du_repository*        parent;
  du_index_t            du_index;
  f1ap_message_handler& cached_msg_handler;
};

} // namespace

du_processor_repository::du_processor_repository(du_repository_config cfg_) :
  cfg(cfg_), logger(cfg.logger), du_task_sched(cfg.timers, *cfg.cu_cp.cu_cp_executor)
{
  f1ap_ev_notifier.connect_cu_cp(*this);
}

std::unique_ptr<f1ap_message_notifier>
du_processor_repository::handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier)
{
  du_index_t du_index = add_du(std::move(f1ap_tx_pdu_notifier));
  if (du_index == du_index_t::invalid) {
    logger.error("Rejecting new DU connection. Cause: Failed to create a new DU.");
    return nullptr;
  }

  logger.info("Added DU {}", du_index);
  if (cfg.amf_connected) {
    du_db.at(du_index).du_processor->get_rrc_amf_connection_handler().handle_amf_connection();
  }

  return std::make_unique<f1ap_rx_pdu_notifier>(*this, du_index);
}

void du_processor_repository::handle_du_remove_request(du_index_t du_index)
{
  logger.debug("removing DU {}", du_index);
  remove_du(du_index);
}

du_index_t du_processor_repository::add_du(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier)
{
  du_index_t du_index = get_next_du_index();
  if (du_index == du_index_t::invalid) {
    logger.error("DU connection failed - maximum number of DUs connected ({})", MAX_NOF_DUS);
    return du_index_t::invalid;
  }

  // Create DU object
  auto it = du_db.insert(std::make_pair(du_index, du_context{}));
  srsran_assert(it.second, "Unable to insert DU in map");
  du_context& du_ctxt = it.first->second;
  du_ctxt.du_to_cu_cp_notifier.connect_cu_cp(cfg.cu_cp_du_handler, du_ctxt.ngap_du_processor_notifier);
  du_ctxt.f1ap_tx_pdu_notifier = std::move(f1ap_tx_pdu_notifier);

  // TODO: use real config
  du_processor_config_t du_cfg = {};
  du_cfg.du_index              = du_index;
  du_cfg.rrc_cfg               = cfg.cu_cp.rrc_config;

  std::unique_ptr<du_processor_interface> du = create_du_processor(du_cfg,
                                                                   du_ctxt.du_to_cu_cp_notifier,
                                                                   f1ap_ev_notifier,
                                                                   *du_ctxt.f1ap_tx_pdu_notifier,
                                                                   cfg.e1ap_ctrl_notifier,
                                                                   cfg.ngap_ctrl_notifier,
                                                                   cfg.ue_nas_pdu_notifier,
                                                                   cfg.ue_ngap_ctrl_notifier,
                                                                   cfg.rrc_ue_cu_cp_notifier,
                                                                   cfg.ue_task_sched,
                                                                   cfg.ue_manager,
                                                                   cfg.cell_meas_mng,
                                                                   *cfg.cu_cp.cu_cp_executor);

  srsran_assert(du != nullptr, "Failed to create DU processor");
  du_ctxt.du_processor = std::move(du);

  // Create connection DU processor to NGAP.
  du_ctxt.ngap_du_processor_notifier.connect_du_processor(du_ctxt.du_processor.get());

  return du_index;
}

du_index_t du_processor_repository::get_next_du_index()
{
  for (int du_idx_int = du_index_to_uint(du_index_t::min); du_idx_int < MAX_NOF_DUS; du_idx_int++) {
    du_index_t du_idx = uint_to_du_index(du_idx_int);
    if (du_db.find(du_idx) == du_db.end()) {
      return du_idx;
    }
  }
  return du_index_t::invalid;
}

void du_processor_repository::remove_du(du_index_t du_index)
{
  // Note: The caller of this function can be a DU procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the DU. This is achieved via a scheduled async task

  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  logger.debug("Scheduling du_index={} deletion", du_index);

  // Schedule DU removal task
  du_task_sched.handle_du_async_task(du_index, launch_async([this, du_index](coro_context<async_task<void>>& ctx) {
                                       CORO_BEGIN(ctx);
                                       auto du_it = du_db.find(du_index);
                                       if (du_it == du_db.end()) {
                                         logger.error("Remove DU called for inexistent du_index={}", du_index);
                                         CORO_EARLY_RETURN();
                                       }

                                       // Remove DU
                                       // TODO
                                       removed_du_db.insert(std::make_pair(du_index, std::move(du_db.at(du_index))));
                                       du_db.erase(du_index);

                                       logger.info("Removed DU {}", du_index);

                                       CORO_RETURN();
                                     }));
}

du_processor_interface& du_processor_repository::find_du(du_index_t du_index)
{
  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  srsran_assert(du_db.find(du_index) != du_db.end(), "DU not found du_index={}", du_index);
  return *du_db.at(du_index).du_processor;
}

du_index_t du_processor_repository::find_du(pci_t pci)
{
  du_index_t index = du_index_t::invalid;
  for (const auto& du : du_db) {
    if (du.second.du_processor->has_cell(pci))
      return du.first;
  }

  return index;
}

size_t du_processor_repository::get_nof_dus() const
{
  return du_db.size();
}

size_t du_processor_repository::get_nof_ues() const
{
  size_t nof_ues = 0;
  for (auto& du : du_db) {
    nof_ues += du.second.du_processor->get_nof_ues();
  }
  return nof_ues;
}

du_handler& du_processor_repository::get_du(du_index_t du_index)
{
  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  srsran_assert(du_db.find(du_index) != du_db.end(), "DU not found du_index={}", du_index);
  return du_db.at(du_index);
}

f1ap_statistics_handler& du_processor_repository::du_context::get_f1ap_statistics_handler()
{
  return du_processor->get_f1ap_statistics_handler();
}

f1ap_message_handler& du_processor_repository::du_context::get_f1ap_message_handler()
{
  return du_processor->get_f1ap_message_handler();
}

du_processor_mobility_handler& du_processor_repository::du_context::get_mobility_handler()
{
  return du_processor->get_du_processor_mobility_handler();
}

du_processor_ue_task_handler& du_processor_repository::du_context::get_du_processor_ue_task_handler()
{
  return du_processor->get_du_processor_ue_task_handler();
}

du_processor_f1ap_ue_context_notifier& du_processor_repository::du_context::get_f1ap_ue_context_notifier()
{
  return du_processor->get_du_processor_f1ap_ue_context_notifier();
}

void du_processor_repository::handle_amf_connection()
{
  // inform all connected DU objects about the new connection
  for (auto& du : du_db) {
    du.second.du_processor->get_rrc_amf_connection_handler().handle_amf_connection();
  }
}

void du_processor_repository::handle_amf_connection_drop()
{
  // inform all DU objects about the AMF connection drop
  for (auto& du : du_db) {
    du.second.du_processor->get_rrc_amf_connection_handler().handle_amf_connection_drop();
  }
}

void du_processor_repository::handle_paging_message(cu_cp_paging_message& msg)
{
  // Forward paging message to all DU processors
  for (auto& du : du_db) {
    du.second.du_processor->get_du_processor_paging_handler().handle_paging_message(msg);
  }
}

ue_index_t du_processor_repository::handle_ue_index_allocation_request(const nr_cell_global_id_t& cgi)
{
  for (auto& du : du_db) {
    if (du.second.du_processor->has_cell(cgi)) {
      return du.second.du_processor->get_du_processor_ngap_interface().get_new_ue_index();
    }
  }
  return ue_index_t::invalid;
}

async_task<ngap_handover_resource_allocation_response>
du_processor_repository::handle_ngap_handover_request(const ngap_handover_request& request)
{
  auto& du = du_db.at(get_du_index_from_ue_index(request.ue_index));

  du_processor_mobility_handler& mob = du.get_mobility_handler();

  return mob.handle_ngap_handover_request(request);
}

void du_processor_repository::request_ue_removal(du_index_t du_index, ue_index_t ue_index)
{
  du_db.at(du_index).du_processor->get_du_processor_ue_handler().remove_ue(ue_index);
}

void du_processor_repository::handle_inactivity_notification(du_index_t                           du_index,
                                                             const cu_cp_inactivity_notification& msg)
{
  // Forward message to DU processor
  du_db.at(du_index).du_processor->handle_inactivity_notification(msg);
}
