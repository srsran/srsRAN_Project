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

#pragma once

#include "../cu_cp_impl_interface.h"
#include "../task_schedulers/ue_task_scheduler.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/support/srsran_assert.h"
#include <cstddef>

namespace srsran {
namespace srs_cu_cp {

/// Adapter between DU processor and CU-CP task scheduler
class du_processor_to_cu_cp_task_scheduler : public du_processor_ue_task_scheduler
{
public:
  du_processor_to_cu_cp_task_scheduler() {}

  void connect_cu_cp(ue_task_scheduler& cu_cp_task_sched_) { cu_cp_task_sched = &cu_cp_task_sched_; }

  void schedule_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    srsran_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    cu_cp_task_sched->handle_ue_async_task(ue_index, std::move(task));
  }

  unique_timer make_unique_timer() override
  {
    srsran_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->make_unique_timer();
  }
  timer_manager& get_timer_manager() override
  {
    srsran_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->get_timer_manager();
  }

private:
  ue_task_scheduler* cu_cp_task_sched = nullptr;
};

/// Adapter between DU processor and CU-CP
class du_processor_cu_cp_adapter : public du_processor_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_du_event_handler& cu_cp_mng_, ngap_du_processor_control_notifier& ngap_du_notifier_)
  {
    cu_cp_handler    = &cu_cp_mng_;
    ngap_du_notifier = &ngap_du_notifier_;
  }

  void on_rrc_ue_created(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface& rrc_ue) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_rrc_ue_creation(du_index, ue_index, rrc_ue, *ngap_du_notifier);
  }

private:
  cu_cp_du_event_handler*             cu_cp_handler    = nullptr;
  ngap_du_processor_control_notifier* ngap_du_notifier = nullptr;
};

/// Adapter between DU processor and E1AP
class du_processor_e1ap_adapter : public du_processor_e1ap_control_notifier
{
public:
  void connect_e1ap(e1ap_bearer_context_manager& e1ap_bearer_context_mng_)
  {
    e1ap_bearer_context_mng = &e1ap_bearer_context_mng_;
  }

  async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override
  {
    srsran_assert(e1ap_bearer_context_mng != nullptr, "E1AP handler must not be nullptr");

    return e1ap_bearer_context_mng->handle_bearer_context_setup_request(msg);
  }

  async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override
  {
    srsran_assert(e1ap_bearer_context_mng != nullptr, "E1AP handler must not be nullptr");

    return e1ap_bearer_context_mng->handle_bearer_context_modification_request(msg);
  }

  async_task<void> on_bearer_context_release_command(const e1ap_bearer_context_release_command& cmd) override
  {
    srsran_assert(e1ap_bearer_context_mng != nullptr, "E1AP handler must not be nullptr");

    return e1ap_bearer_context_mng->handle_bearer_context_release_command(cmd);
  }

private:
  e1ap_bearer_context_manager* e1ap_bearer_context_mng = nullptr;
};

// Adapter between DU processor and F1AP for UE context
class du_processor_f1ap_ue_context_adapter : public du_processor_f1ap_ue_context_notifier
{
public:
  du_processor_f1ap_ue_context_adapter() = default;

  void connect_f1(f1ap_ue_context_manager& handler_) { handler = &handler_; }

  async_task<f1ap_ue_context_setup_response>
  on_ue_context_setup_request(const f1ap_ue_context_setup_request& request) override
  {
    srsran_assert(handler != nullptr, "F1AP handler must not be nullptr");
    return handler->handle_ue_context_setup_request(request);
  }

  async_task<ue_index_t> on_ue_context_release_command(const f1ap_ue_context_release_command& msg) override
  {
    srsran_assert(handler != nullptr, "F1AP handler must not be nullptr");
    return handler->handle_ue_context_release_command(msg);
  }

  virtual async_task<f1ap_ue_context_modification_response>
  on_ue_context_modification_request(const f1ap_ue_context_modification_request& request) override
  {
    srsran_assert(handler != nullptr, "F1AP handler must not be nullptr");
    return handler->handle_ue_context_modification_request(request);
  }

private:
  f1ap_ue_context_manager* handler = nullptr;
};

// Adapter between DU processor and F1AP for Paging
class du_processor_f1ap_paging_adapter : public du_processor_f1ap_paging_notifier
{
public:
  du_processor_f1ap_paging_adapter() = default;

  void connect_f1(f1ap_paging_manager& handler_) { handler = &handler_; }

  void on_paging_message(const cu_cp_paging_message& msg) override
  {
    if (handler == nullptr) {
      srslog::fetch_basic_logger("CU-CP").info("DU not connected - dropping paging message");
      return;
    }
    handler->handle_paging(msg);
  }

private:
  f1ap_paging_manager* handler = nullptr;
};

// Adapter between DU processor and RRC DU
class du_processor_rrc_du_adapter : public du_processor_rrc_du_ue_notifier
{
public:
  du_processor_rrc_du_adapter() = default;

  void connect_rrc_du(rrc_du_cell_manager& rrc_du_cell_handler_, rrc_du_ue_repository& rrc_du_handler_)
  {
    rrc_du_cell_handler = &rrc_du_cell_handler_;
    rrc_du_handler      = &rrc_du_handler_;
  }

  virtual bool on_new_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) override
  {
    srsran_assert(rrc_du_cell_handler != nullptr, "RRC DU cell handler must not be nullptr");
    return rrc_du_cell_handler->handle_served_cell_list(served_cell_list);
  }

  virtual rrc_ue_interface* on_ue_creation_request(up_resource_manager&           resource_mng,
                                                   const rrc_ue_creation_message& msg) override
  {
    srsran_assert(rrc_du_handler != nullptr, "RRC DU UE handler must not be nullptr");
    return rrc_du_handler->add_ue(resource_mng, msg);
  }

  virtual void on_ue_context_release_command(ue_index_t ue_index) override
  {
    srsran_assert(rrc_du_handler != nullptr, "RRC DU UE handler must not be nullptr");
    return rrc_du_handler->remove_ue(ue_index);
  }

  virtual void on_release_ues() override
  {
    srsran_assert(rrc_du_handler != nullptr, "RRC DU UE handler must not be nullptr");
    return rrc_du_handler->release_ues();
  }

private:
  rrc_du_cell_manager*  rrc_du_cell_handler = nullptr;
  rrc_du_ue_repository* rrc_du_handler      = nullptr;
};

// Adapter between DU processor and RRC UE
class du_processor_rrc_ue_adapter : public du_processor_rrc_ue_control_message_notifier
{
public:
  du_processor_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_ue_control_message_handler& rrc_ue_handler_) { rrc_ue_handler = &rrc_ue_handler_; }

  virtual async_task<bool> on_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_rrc_ue_capability_transfer_request(msg);
  }

  virtual async_task<bool> on_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_rrc_reconfiguration_request(msg);
  }

  virtual uint8_t on_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_handover_reconfiguration_request(msg);
  }

  virtual async_task<bool> on_handover_reconfiguration_complete_expected(uint8_t transaction_id) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_handover_reconfiguration_complete_expected(transaction_id);
  }

  virtual rrc_ue_release_context get_rrc_ue_release_context() override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_rrc_ue_release_context();
  }

  virtual optional<rrc_meas_cfg> get_rrc_ue_meas_config() override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_rrc_ue_meas_config();
  }

private:
  rrc_ue_control_message_handler* rrc_ue_handler = nullptr;
};

// Adapter between DU processor and NGAP
class du_processor_ngap_adapter : public du_processor_ngap_control_notifier
{
public:
  du_processor_ngap_adapter() = default;

  void connect_ngap(ngap_control_message_handler& ngap_handler_) { ngap_handler = &ngap_handler_; }

  void on_ue_context_release_request(const cu_cp_ue_context_release_request& msg) override
  {
    srsran_assert(ngap_handler != nullptr, "NGAP handler must not be nullptr");
    return ngap_handler->handle_ue_context_release_request(msg);
  }

  async_task<ngap_handover_preparation_response>
  on_ngap_handover_preparation_request(const ngap_handover_preparation_request& msg) override
  {
    srsran_assert(ngap_handler != nullptr, "NGAP handler must not be nullptr");
    return ngap_handler->handle_handover_preparation_request(msg);
  }

private:
  ngap_control_message_handler* ngap_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
