/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../task_schedulers/ue_task_scheduler.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/e1/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsgnb/rrc/rrc_du.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between DU processor and CU-CP task scheduler
class du_processor_to_cu_cp_task_scheduler : public du_processor_ue_task_scheduler
{
public:
  du_processor_to_cu_cp_task_scheduler() {}

  void connect_cu_cp(ue_task_scheduler& cu_cp_task_sched_) { cu_cp_task_sched = &cu_cp_task_sched_; }

  void schedule_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    cu_cp_task_sched->handle_ue_async_task(ue_index, std::move(task));
  }

  unique_timer make_unique_timer() override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->make_unique_timer();
  }
  timer_manager& get_timer_manager() override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->get_timer_manager();
  }

private:
  ue_task_scheduler* cu_cp_task_sched = nullptr;
};

/// Adapter between DU processor and CU-CP
class du_processor_cu_cp_adapter : public du_processor_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_du_handler& cu_cp_mng_) { cu_cp_handler = &cu_cp_mng_; }

  void on_new_du_connection() override
  {
    srsgnb_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_new_du_connection();
  }

  void on_rrc_ue_created(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface* rrc_ue) override
  {
    srsgnb_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_rrc_ue_creation(du_index, ue_index, rrc_ue);
  }

private:
  cu_cp_du_handler* cu_cp_handler = nullptr;
};

/// Adapter between DU processor and E1AP
class du_processor_e1ap_adapter : public du_processor_e1ap_control_notifier
{
public:
  void connect_e1ap(e1_bearer_context_manager& e1_bearer_context_mng_)
  {
    e1_bearer_context_mng = &e1_bearer_context_mng_;
  }

  async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override
  {
    srsgnb_assert(e1_bearer_context_mng != nullptr, "e1_bearer_context_mng must not be nullptr");

    return e1_bearer_context_mng->handle_bearer_context_setup_request(msg);
  }

  async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override
  {
    srsgnb_assert(e1_bearer_context_mng != nullptr, "e1_bearer_context_mng must not be nullptr");

    return e1_bearer_context_mng->handle_bearer_context_modification_request(msg);
  }

  async_task<void> on_bearer_context_release_command(const e1ap_bearer_context_release_command& cmd) override
  {
    srsgnb_assert(e1_bearer_context_mng != nullptr, "e1_bearer_context_mng must not be nullptr");

    return e1_bearer_context_mng->handle_bearer_context_release_command(cmd);
  }

private:
  e1_bearer_context_manager* e1_bearer_context_mng = nullptr;
};

// Adapter between DU processor and F1C
class du_processor_f1c_adapter : public du_processor_f1ap_ue_context_notifier
{
public:
  du_processor_f1c_adapter() = default;

  void connect_f1(f1c_ue_context_manager& handler_) { handler = &handler_; }

  async_task<f1ap_ue_context_setup_response>
  on_ue_context_setup_request(const f1ap_ue_context_setup_request& request) override
  {
    srsgnb_assert(handler != nullptr, "F1C handler must not be nullptr");
    return handler->handle_ue_context_setup_request(request);
  }

  async_task<ue_index_t> on_ue_context_release_command(const f1ap_ue_context_release_command& msg) override
  {
    srsgnb_assert(handler != nullptr, "F1C handler must not be nullptr");
    return handler->handle_ue_context_release_command(msg);
  }

  virtual async_task<cu_cp_ue_context_modification_response>
  on_ue_context_modification_request(const cu_cp_ue_context_modification_request& request) override
  {
    srsgnb_assert(handler != nullptr, "F1C handler must not be nullptr");
    return handler->handle_ue_context_modification_request(request);
  }

private:
  f1c_ue_context_manager* handler = nullptr;
};

// Adapter between DU processor and RRC DU
class du_processor_rrc_du_adapter : public du_processor_rrc_du_ue_notifier
{
public:
  du_processor_rrc_du_adapter() = default;

  void connect_rrc_du(rrc_du_ue_repository& rrc_du_handler_) { rrc_du_handler = &rrc_du_handler_; }

  virtual rrc_ue_interface* on_ue_creation_request(rrc_ue_creation_message msg) override
  {
    srsgnb_assert(rrc_du_handler != nullptr, "RRC DU handler must not be nullptr");
    return rrc_du_handler->add_ue(msg);
  }

  virtual void on_ue_context_release_command(ue_index_t ue_index) override
  {
    srsgnb_assert(rrc_du_handler != nullptr, "RRC DU handler must not be nullptr");
    return rrc_du_handler->remove_ue(ue_index);
  }

  virtual void on_release_ues() override
  {
    srsgnb_assert(rrc_du_handler != nullptr, "RRC DU handler must not be nullptr");
    return rrc_du_handler->release_ues();
  }

private:
  rrc_du_ue_repository* rrc_du_handler = nullptr;
};

// Adapter between DU processor and RRC UE
class du_processor_rrc_ue_adapter : public du_processor_rrc_ue_control_message_notifier
{
public:
  du_processor_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_ue_control_message_handler& rrc_ue_handler_) { rrc_ue_handler = &rrc_ue_handler_; }

  virtual void on_new_guami(const guami& msg) override
  {
    srsgnb_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_new_guami(msg);
  }

  virtual async_task<bool>
  on_rrc_reconfiguration_request(const cu_cp_rrc_reconfiguration_procedure_request& msg) override
  {
    srsgnb_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_rrc_reconfiguration_request(msg);
  }

  virtual void on_rrc_ue_release() override
  {
    srsgnb_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_rrc_ue_release();
  }

private:
  rrc_ue_control_message_handler* rrc_ue_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
