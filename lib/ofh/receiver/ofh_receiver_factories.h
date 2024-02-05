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
#include "ofh_receiver_impl.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/receiver/ofh_receiver_configuration.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Creates a receiver with the given configuration and dependencies.
std::unique_ptr<receiver> create_receiver(const receiver_config&                            receiver_cfg,
                                          srslog::basic_logger&                             logger,
                                          std::unique_ptr<ether::receiver>                  eth_receiver,
                                          std::shared_ptr<uplane_rx_symbol_notifier>        notifier,
                                          std::shared_ptr<prach_context_repository>         prach_context_repo,
                                          std::shared_ptr<uplink_context_repository>        ul_slot_context_repo,
                                          std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo);

} // namespace ofh
} // namespace srsran
