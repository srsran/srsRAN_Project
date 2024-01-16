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

#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

struct e2sm_rc_control_service_def_t {
  uint32_t    style_id;
  std::string style_name;
  uint32_t    call_process_id_format;
  uint32_t    ctrl_hdr_format;
  uint32_t    ctrl_msg_format;
  uint32_t    ctrl_outcome_format;
};

/// Number of E2SM-RC Control Styles defined in O-RAN.WG3.E2SM-RC-R003-v03.00.
const size_t NOF_E2SM_RC_CTRL_SERVICE_STYLES = 11;

// E2SM-RC Control Styles defined in O-RAN.WG3.E2SM-RC-R003-v03.00, Table 7.9-2
inline bool get_e2sm_rc_control_style_def(uint32_t style_id, e2sm_rc_control_service_def_t& control_service_def)
{
  static const std::array<e2sm_rc_control_service_def_t, NOF_E2SM_RC_CTRL_SERVICE_STYLES> style_defs = {{
      {1, "Radio Bearer Control", 1, 1, 1, 1},
      {2, "Radio Resource Allocation Control", 1, 1, 1, 1},
      {3, "Connected Mode Mobility Control", 1, 1, 1, 1},
      {4, "Radio Access Control", 1, 1, 1, 1},
      {5, "Dual Connectivity (DC) Control", 1, 1, 1, 1},
      {6, "Carrier Aggregation (CA) Control", 1, 1, 1, 1},
      {7, "Idle Mode Mobility Control", 1, 1, 1, 1},
      {8, "UE Information And Assignment", 0, 1, 1, 1},
      {9, "Measurement Reporting Configuration Control", 0, 1, 1, 3},
      {10, "Beamforming Configuration Control", 0, 1, 1, 0},
      {255, "Multiple Actions Control", 1, 2, 2, 2},
  }};

  for (auto& style_def : style_defs) {
    if (style_def.style_id == style_id) {
      control_service_def = style_def;
      return true;
    }
  }
  return false;
}

class e2sm_rc_control_service_base : public e2sm_control_service
{
public:
  e2sm_rc_control_service_base() = delete;
  e2sm_rc_control_service_base(uint32_t style_id_);
  virtual ~e2sm_rc_control_service_base() = default;

  bool fill_ran_function_description(asn1::e2sm_rc::ran_function_definition_ctrl_item_s& ctrl_item);

  /// e2sm_control_service functions.
  uint32_t get_style_type() override;
  bool add_e2sm_rc_control_action_executor(std::unique_ptr<e2sm_control_action_executor> ctrl_act_executor) override;
  bool control_request_supported(const e2sm_ric_control_request& req) override = 0;
  virtual async_task<e2sm_ric_control_response>
  execute_control_request(const e2sm_ric_control_request& req) override = 0;

protected:
  srslog::basic_logger&                                             logger;
  uint32_t                                                          style_id;
  e2sm_rc_control_service_def_t                                     control_service_def;
  std::map<uint32_t, std::unique_ptr<e2sm_control_action_executor>> config_req_executors;
};

/// Class for e2sm_rc_control_service styles 1-10.
class e2sm_rc_control_service : public e2sm_rc_control_service_base
{
public:
  e2sm_rc_control_service() = delete;
  e2sm_rc_control_service(uint32_t style_id_);
  virtual ~e2sm_rc_control_service() = default;

  bool                                  control_request_supported(const e2sm_ric_control_request& req) override;
  async_task<e2sm_ric_control_response> execute_control_request(const e2sm_ric_control_request& req) override;
};

/// Class for e2sm_rc_control_service style 255 (Multiple actions)
class e2sm_rc_control_service_style_255 : public e2sm_rc_control_service_base
{
public:
  e2sm_rc_control_service_style_255();
  virtual ~e2sm_rc_control_service_style_255() = default;

  bool                                  control_request_supported(const e2sm_ric_control_request& req) override;
  async_task<e2sm_ric_control_response> execute_control_request(const e2sm_ric_control_request& req) override;

private:
  // Helper functions.
  e2sm_ric_control_request
  create_req_f1_from_req_f2(const asn1::e2sm_rc::e2_sm_rc_ctrl_hdr_format2_s&                 ctrl_hdr_f2,
                            const asn1::e2sm_rc::e2_sm_rc_ctrl_msg_format2_style_item_s       style,
                            const asn1::e2sm_rc::e2_sm_rc_ctrl_msg_format2_ctrl_action_item_s action);

  std::map<uint64_t, e2sm_control_service*> e2sm_rc_control_services;
};

} // namespace srsran
