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

} // namespace test_helpers
} // namespace srsran
