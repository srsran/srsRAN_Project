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

#include "support/uplink_context_repository.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Open Fronthaul controller for the sector.
class sector_controller : public operation_controller
{
public:
  sector_controller(transmitter& ofh_tx_, receiver& ofh_rx_, std::shared_ptr<uplink_context_repository> slot_repo_) :
    ofh_tx(ofh_tx_), ofh_rx(ofh_rx_), slot_repo(std::move(slot_repo_))
  {
  }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  transmitter&                               ofh_tx;
  receiver&                                  ofh_rx;
  std::shared_ptr<uplink_context_repository> slot_repo;
};

} // namespace ofh
} // namespace srsran
