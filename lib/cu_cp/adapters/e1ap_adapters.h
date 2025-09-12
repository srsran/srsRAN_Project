/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cu_cp_impl_interface.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between E1AP and CU-CP
class e1ap_cu_cp_adapter : public e1ap_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_e1ap_event_handler& cu_cp_handler_) { cu_cp_handler = &cu_cp_handler_; }

  bool schedule_async_task(ue_index_t ue_index, async_task<void> task) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_handler->schedule_ue_task(ue_index, std::move(task));
  }

  void on_bearer_context_release_request_received(const cu_cp_bearer_context_release_request& msg) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_bearer_context_release_request(msg);
  }

  void on_bearer_context_inactivity_notification_received(const cu_cp_inactivity_notification& msg) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_bearer_context_inactivity_notification(msg);
  }

  async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_handler->handle_ue_context_release(request);
  }

  void on_e1_release_request_received(cu_up_index_t cu_up_index) override
  {
    srsran_assert(cu_cp_handler != nullptr, "E1AP handler must not be nullptr");
    cu_cp_handler->handle_e1_release_request(cu_up_index);
  }

  async_task<void> on_transaction_info_loss(const ue_transaction_info_loss_event& ev) override
  {
    srsran_assert(cu_cp_handler != nullptr, "E1AP handler must not be nullptr");
    return cu_cp_handler->handle_transaction_info_loss(ev);
  }

private:
  cu_cp_e1ap_event_handler* cu_cp_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
