/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/receiver/ofh_receiver.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul controller for the sector.
class sector_controller : public controller
{
public:
  explicit sector_controller(receiver& ofh_rx_) : ofh_rx(ofh_rx_) {}

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  receiver& ofh_rx;
};

} // namespace ofh
} // namespace srsran
