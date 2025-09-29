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

#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

struct e2sm_ccc_control_service_def_t {
  uint32_t    style_id;
  std::string style_name;
  uint32_t    ctrl_hdr_format;
  uint32_t    ctrl_msg_format;
  uint32_t    ctrl_outcome_format;
};

/// Number of E2SM-CCC Control Styles defined in O-RAN.WG3.TS.E2SM-CCC-R004-v06.00.
const size_t NOF_E2SM_CCC_CTRL_SERVICE_STYLES = 2;

// E2SM-RC Control Styles defined in O-RAN.WG3.E2SM-RC-R003-v03.00, Table 7.9-2
inline bool get_e2sm_ccc_control_style_def(uint32_t style_id, e2sm_ccc_control_service_def_t& control_service_def)
{
  static const std::array<e2sm_ccc_control_service_def_t, NOF_E2SM_CCC_CTRL_SERVICE_STYLES> style_defs = {{
      {1, "Node Configuration and Control", 1, 1, 1},
      {2, "Cell Configuration and Control", 1, 2, 2},
  }};

  for (auto& style_def : style_defs) {
    if (style_def.style_id == style_id) {
      control_service_def = style_def;
      return true;
    }
  }
  return false;
}

class e2sm_ccc_control_service : public e2sm_control_service
{
public:
  e2sm_ccc_control_service() = delete;
  e2sm_ccc_control_service(uint32_t style_id_);
  ~e2sm_ccc_control_service() override = default;

  asn1::e2sm::ran_function_definition_ctrl_item_s get_control_style_definition() override;

  /// e2sm_control_service functions.
  uint32_t get_style_type() override;
  bool add_e2sm_rc_control_action_executor(std::unique_ptr<e2sm_control_action_executor> ctrl_act_executor) override;
  bool control_request_supported(const e2sm_ric_control_request& req) override                                = 0;
  async_task<e2sm_ric_control_response> execute_control_request(const e2sm_ric_control_request& req) override = 0;

protected:
  srslog::basic_logger&                                                logger;
  uint32_t                                                             style_id;
  e2sm_ccc_control_service_def_t                                       control_service_def;
  std::map<std::string, std::unique_ptr<e2sm_control_action_executor>> config_req_executors;
};

class e2sm_ccc_control_service_style_2 : public e2sm_ccc_control_service
{
public:
  e2sm_ccc_control_service_style_2();
  ~e2sm_ccc_control_service_style_2() override = default;

  bool                                  control_request_supported(const e2sm_ric_control_request& req) override;
  async_task<e2sm_ric_control_response> execute_control_request(const e2sm_ric_control_request& req) override;
};

} // namespace srsran
