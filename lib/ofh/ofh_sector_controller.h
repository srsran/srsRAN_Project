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

#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ofh_controller.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul controller for the sector.
class sector_controller : public controller
{
public:
  explicit sector_controller(ether::receiver& receiver_) : receiver(receiver_) {}

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  ether::receiver& receiver;
};

} // namespace ofh
} // namespace srsran
