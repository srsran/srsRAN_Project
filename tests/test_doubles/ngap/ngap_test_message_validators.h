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

namespace srsran {

namespace srs_cu_cp {

struct ngap_message;

} // namespace srs_cu_cp

namespace test_helpers {

bool is_valid_init_ue_message(const srs_cu_cp::ngap_message& msg);

bool is_valid_initial_context_setup_response(const srs_cu_cp::ngap_message& msg);

bool is_valid_ue_context_release_request(const srs_cu_cp::ngap_message& msg);

bool is_valid_ue_radio_capability_info_indication(const srs_cu_cp::ngap_message& msg);

} // namespace test_helpers
} // namespace srsran