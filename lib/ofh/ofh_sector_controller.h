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

#include "support/uplink_context_repository.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Open Fronthaul controller for the sector.
class sector_controller : public operation_controller
{
public:
  sector_controller(receiver& ofh_rx_, std::shared_ptr<uplink_context_repository> slot_repo_) :
    ofh_rx(ofh_rx_), slot_repo(std::move(slot_repo_))
  {
  }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  receiver&                                  ofh_rx;
  std::shared_ptr<uplink_context_repository> slot_repo;
};

} // namespace ofh
} // namespace srsran
