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

#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/du_manager/du_configurator.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

class e2sm_rc_control_action_du_executor_base : public e2sm_control_action_executor
{
public:
  e2sm_rc_control_action_du_executor_base() = delete;
  e2sm_rc_control_action_du_executor_base(du_configurator& du_configurator_, uint32_t action_id_);
  virtual ~e2sm_rc_control_action_du_executor_base() = default;

  bool fill_ran_function_description(asn1::e2sm_rc::ran_function_definition_ctrl_action_item_s& action_item);

  /// e2sm_control_request_executor functions.
  uint32_t                              get_action_id() override;
  bool                                  ric_control_action_supported(const e2sm_ric_control_request& req) override = 0;
  async_task<e2sm_ric_control_response> execute_ric_control_action(const e2sm_ric_control_request& req) override   = 0;
  async_task<e2sm_ric_control_response> return_ctrl_failure(const e2sm_ric_control_request& req);

protected:
  srslog::basic_logger&           logger;
  uint32_t                        action_id;
  std::string                     action_name;
  std::map<uint32_t, std::string> action_params;
  du_configurator&                du_param_configurator;
};

class e2sm_rc_control_action_2_6_du_executor : public e2sm_rc_control_action_du_executor_base
{
public:
  e2sm_rc_control_action_2_6_du_executor(du_configurator& du_configurator_);
  virtual ~e2sm_rc_control_action_2_6_du_executor() = default;

  /// e2sm_control_request_executor functions.
  bool                                  ric_control_action_supported(const e2sm_ric_control_request& req) override;
  async_task<e2sm_ric_control_response> execute_ric_control_action(const e2sm_ric_control_request& req) override;

private:
  du_mac_sched_control_config convert_to_du_config_request(const e2sm_ric_control_request& e2sm_req_);
  e2sm_ric_control_response   convert_to_e2sm_response(const du_mac_sched_control_config&          du_config_req_,
                                                       const du_mac_sched_control_config_response& du_reponse_);
};

} // namespace srsran
