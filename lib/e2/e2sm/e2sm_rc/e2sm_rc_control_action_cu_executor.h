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

#include "srsran/asn1/asn1_utils.h"
#include "srsran/cu_cp/cu_configurator.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

class e2sm_rc_control_action_cu_executor_base : public e2sm_control_action_executor
{
public:
  e2sm_rc_control_action_cu_executor_base() = delete;
  e2sm_rc_control_action_cu_executor_base(cu_configurator& cu_configurator_, uint32_t action_id_);
  virtual ~e2sm_rc_control_action_cu_executor_base() = default;

  asn1::e2sm::ran_function_definition_ctrl_action_item_s get_control_action_definition() override;

  /// e2sm_control_request_executor functions.
  uint32_t                              get_action_id() override;
  std::string                           get_action_name() override;
  bool                                  ric_control_action_supported(const e2sm_ric_control_request& req) override = 0;
  async_task<e2sm_ric_control_response> execute_ric_control_action(const e2sm_ric_control_request& req) override   = 0;
  async_task<e2sm_ric_control_response> return_ctrl_failure(const e2sm_ric_control_request& req);

protected:
  srslog::basic_logger& logger;
  uint32_t              action_id;
  std::string           action_name;
  cu_configurator&      cu_param_configurator;
};

class e2sm_rc_control_action_3_1_cu_executor : public e2sm_rc_control_action_cu_executor_base
{
public:
  e2sm_rc_control_action_3_1_cu_executor(cu_configurator& du_configurator_);
  virtual ~e2sm_rc_control_action_3_1_cu_executor() = default;

  /// e2sm_control_request_executor functions.
  bool                                  ric_control_action_supported(const e2sm_ric_control_request& req) override;
  async_task<e2sm_ric_control_response> execute_ric_control_action(const e2sm_ric_control_request& req) override;
  void parse_action_ran_parameter_value(const asn1::e2sm::ran_param_value_type_c& ran_p,
                                        uint64_t                                  ran_param_id,
                                        uint64_t                                  ue_id,
                                        cu_handover_control_config&               ctrl_cfg);
  using T = cu_handover_control_config;

private:
};

} // namespace srsran
