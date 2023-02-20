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

#include "mac.h"
#include "mac_cell_result.h"
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/mac/mac_config.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"
#include <memory>

namespace srsran {

std::unique_ptr<mac_interface> create_mac(const mac_config& mac_cfg);

} // namespace srsran
