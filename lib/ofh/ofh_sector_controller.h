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
