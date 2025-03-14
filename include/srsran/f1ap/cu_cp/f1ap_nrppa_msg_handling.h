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

#pragma once

#include "srsran/cu_cp/positioning_messages.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// Handles incoming NRPPa messages to be transmitted over the F1 interface.
class f1ap_nrppa_message_handler
{
public:
  virtual ~f1ap_nrppa_message_handler() = default;

  /// Handles the given TRP information request message.
  virtual async_task<expected<trp_information_response_t, trp_information_failure_t>>
  handle_trp_information_request(const trp_information_request_t& request) = 0;

  /// Handles the given positioning information request message.
  virtual async_task<expected<positioning_information_response_t, positioning_information_failure_t>>
  handle_positioning_information_request(const positioning_information_request_t& request) = 0;

  /// Handles the given positioning activation request message.
  virtual async_task<expected<positioning_activation_response_t, positioning_activation_failure_t>>
  handle_positioning_activation_request(const positioning_activation_request_t& request) = 0;

  /// Handles the given measurement information request message.
  virtual async_task<expected<measurement_response_t, measurement_failure_t>>
  handle_measurement_information_request(const measurement_request_t& request) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
