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

#include "../support/prach_context_repository.h"
#include "../support/uplink_context_repository.h"
#include "../support/uplink_cplane_context_repository.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"

namespace srsran {

class task_executor;

namespace ether {
class eth_frame_pool;
}

namespace ofh {

/// Creates a transmitter with the given configuration and dependencies.
std::unique_ptr<transmitter> create_transmitter(const transmitter_config&                         transmitter_cfg,
                                                srslog::basic_logger&                             logger,
                                                task_executor&                                    tx_executor,
                                                task_executor&                                    downlink_executor,
                                                std::unique_ptr<ether::gateway>                   eth_gateway,
                                                std::shared_ptr<prach_context_repository>         prach_context_repo,
                                                std::shared_ptr<uplink_context_repository>        ul_slot_context_repo,
                                                std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo);

} // namespace ofh
} // namespace srsran
