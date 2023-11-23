/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/du_manager/du_configurator.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
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
  uint32_t                     get_action_id() override;
  bool                         ric_control_action_supported(const e2_sm_ric_control_request_s& req) override = 0;
  e2_sm_ric_control_response_s execute_ric_control_action(const e2_sm_ric_control_request_s& req) override   = 0;

protected:
  srslog::basic_logger&           logger;
  uint32_t                        action_id;
  std::string                     action_name;
  std::map<uint32_t, std::string> action_params;
  du_configurator&                du_param_configurator;
  fifo_async_task_scheduler       async_tasks;
};

class e2sm_rc_control_action_2_6_du_executor : public e2sm_rc_control_action_du_executor_base
{
public:
  e2sm_rc_control_action_2_6_du_executor(du_configurator& du_configurator_);
  virtual ~e2sm_rc_control_action_2_6_du_executor() = default;

  /// e2sm_control_request_executor functions.
  bool                         ric_control_action_supported(const e2_sm_ric_control_request_s& req) override;
  e2_sm_ric_control_response_s execute_ric_control_action(const e2_sm_ric_control_request_s& req) override;

  // Helper variables.
  du_mac_sched_control_config ctrl_config;
};

} // namespace srsran
