/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/support/async/async_task.h"

namespace srsran {

struct ric_control_config {
  uint64_t           ue_id;
  optional<unsigned> num_harq_processes;
  optional<unsigned> num_harq_retransmissions;
  optional<unsigned> min_prb_alloc;
  optional<unsigned> max_prb_alloc;
};

struct ric_control_config_response {
  bool harq_processes_result;
  bool min_prb_alloc_result;
  bool max_prb_alloc_result;
};

class e2sm_param_configurator
{
public:
  virtual ~e2sm_param_configurator() = default;

  virtual async_task<ric_control_config_response> configure_ue_mac_scheduler(ric_control_config reconf) = 0;
};

} // namespace srsran
