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

#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/logging/scheduler_metrics_handler.h"
#include "srsran/scheduler/config/sched_cell_config_helpers.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"

namespace srsran {

class sched_metrics_ue_configurator;

namespace test_helpers {

/// Helper class to manage cell and UE configurations of a scheduler test.
class test_sched_config_manager
{
public:
  test_sched_config_manager(const cell_config_builder_params& builder_params,
                            const scheduler_expert_config&    expert_cfg_ = {});
  ~test_sched_config_manager();

  const sched_cell_configuration_request_message& get_default_cell_config_request() const { return default_cell_req; }
  const sched_ue_creation_request_message&        get_default_ue_config_request() const { return default_ue_req; }

  const cell_configuration* add_cell(const sched_cell_configuration_request_message& msg);

  const cell_configuration& get_cell(du_cell_index_t cell_idx) const { return *cfg_mng.common_cell_list()[cell_idx]; }

  const ue_configuration* add_ue(const sched_ue_creation_request_message& cfg_req);
  bool                    rem_ue(du_ue_index_t ue_index);

private:
  const cell_config_builder_params               builder_params;
  scheduler_expert_config                        expert_cfg;
  std::unique_ptr<sched_configuration_notifier>  cfg_notifier;
  std::unique_ptr<scheduler_metrics_notifier>    metric_notifier;
  std::unique_ptr<sched_metrics_ue_configurator> ue_metrics_configurator;
  scheduler_metrics_handler                      metrics_handler;

  sched_cell_configuration_request_message default_cell_req;
  sched_ue_creation_request_message        default_ue_req;

  sched_config_manager cfg_mng;
};

} // namespace test_helpers
} // namespace srsran
