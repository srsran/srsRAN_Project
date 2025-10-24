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

namespace srsran {

struct e1ap_message;
class byte_buffer;

namespace test_helpers {

/// \brief Check if an E1AP message is a valid Bearer Context Setup Request message.
bool is_valid_bearer_context_setup_request(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid Bearer Context Modification Request message.
bool is_valid_bearer_context_modification_request(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid Bearer Context Modification Request message.
bool is_valid_bearer_context_modification_request_with_ue_security_info(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid Bearer Context Release Command message.
bool is_valid_bearer_context_release_command(const e1ap_message& msg);

/// \brief Check if an E1AP message is a valid E1 Reset message.
bool is_valid_e1_reset(const e1ap_message& msg);

} // namespace test_helpers
} // namespace srsran
