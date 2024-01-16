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

#pragma once

#include "srsran/cu_up/cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_up {

/// Adapter between E1AP and CU-UP
class e1ap_cu_up_adapter : public e1ap_cu_up_notifier
{
public:
  void connect_cu_up(cu_up_e1ap_interface& cu_up_handler_) { cu_up_handler = &cu_up_handler_; }

  e1ap_bearer_context_setup_response
  on_bearer_context_setup_request_received(const e1ap_bearer_context_setup_request& msg) override
  {
    srsran_assert(cu_up_handler != nullptr, "CU-UP handler must not be nullptr");
    return cu_up_handler->handle_bearer_context_setup_request(msg);
  }

  e1ap_bearer_context_modification_response
  on_bearer_context_modification_request_received(const e1ap_bearer_context_modification_request& msg) override
  {
    srsran_assert(cu_up_handler != nullptr, "CU-UP handler must not be nullptr");
    return cu_up_handler->handle_bearer_context_modification_request(msg);
  }

  void on_bearer_context_release_command_received(const e1ap_bearer_context_release_command& msg) override
  {
    srsran_assert(cu_up_handler != nullptr, "CU-UP handler must not be nullptr");
    return cu_up_handler->handle_bearer_context_release_command(msg);
  }

private:
  cu_up_e1ap_interface* cu_up_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsran
