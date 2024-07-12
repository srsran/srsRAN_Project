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

namespace srsran {

namespace srs_cu_cp {

struct ngap_message;

} // namespace srs_cu_cp

namespace test_helpers {

bool is_valid_init_ue_message(const srs_cu_cp::ngap_message& msg);

bool is_valid_initial_context_setup_response(const srs_cu_cp::ngap_message& msg);

bool is_valid_initial_context_setup_failure(const srs_cu_cp::ngap_message& msg);

bool is_valid_ue_context_release_request(const srs_cu_cp::ngap_message& msg);

bool is_valid_ue_radio_capability_info_indication(const srs_cu_cp::ngap_message& msg);

} // namespace test_helpers
} // namespace srsran