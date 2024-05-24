/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_up/cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"

namespace srsran {
namespace srs_cu_up {

/// Adapter between E1AP and CU-UP
class e1ap_cu_up_adapter : public e1ap_cu_up_notifier
{
public:
  void connect_cu_up(cu_up_e1ap_interface& cu_up_handler_) { cu_up_handler = &cu_up_handler_; }

  void disconnect() { cu_up_handler = nullptr; }

  e1ap_bearer_context_setup_response
  on_bearer_context_setup_request_received(const e1ap_bearer_context_setup_request& msg) override
  {
    if (cu_up_handler == nullptr) {
      return {}; // return failure to setup bearer context
    }
    return cu_up_handler->handle_bearer_context_setup_request(msg);
  }

  e1ap_bearer_context_modification_response
  on_bearer_context_modification_request_received(const e1ap_bearer_context_modification_request& msg) override
  {
    if (cu_up_handler == nullptr) {
      return {}; // return failure to modify bearer context
    }
    return cu_up_handler->handle_bearer_context_modification_request(msg);
  }

  void on_bearer_context_release_command_received(const e1ap_bearer_context_release_command& msg) override
  {
    if (cu_up_handler == nullptr) {
      return;
    }
    return cu_up_handler->handle_bearer_context_release_command(msg);
  }

private:
  cu_up_e1ap_interface* cu_up_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsran
