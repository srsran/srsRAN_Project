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

#include "ofh_sector_controller.h"
#include "support/prach_context_repository.h"
#include "support/uplink_context_repository.h"
#include "support/uplink_cplane_context_repository.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ofh_sector.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul sector implementation.
class sector_impl : public sector
{
public:
  sector_impl(std::unique_ptr<receiver>                         receiver_,
              std::unique_ptr<transmitter>                      transmitter_,
              std::shared_ptr<uplink_cplane_context_repository> cp_repo_,
              std::shared_ptr<prach_context_repository>         prach_repo_,
              std::shared_ptr<uplink_context_repository>        slot_repo_) :
    cp_repo(std::move(cp_repo_)),
    prach_repo(std::move(prach_repo_)),
    slot_repo(std::move(slot_repo_)),
    ofh_receiver(std::move(receiver_)),
    ofh_transmitter(std::move(transmitter_)),
    ofh_sector_controller(*ofh_receiver)
  {
    srsran_assert(ofh_receiver, "Invalid Open Fronthaul receiver");
    srsran_assert(ofh_transmitter, "Invalid Open Fronthaul transmitter");
    srsran_assert(cp_repo, "Invalid Control-Plane context repository");
    srsran_assert(prach_repo, "Invalid PRACH context repository");
    srsran_assert(slot_repo, "Invalid UL slot context repository");
  }

  // See interface for documentation.
  receiver& get_receiver() override;

  // See interface for documentation.
  transmitter& get_transmitter() override;

  // See interface for documentation.
  controller& get_controller() override;

private:
  std::shared_ptr<uplink_cplane_context_repository> cp_repo;
  std::shared_ptr<prach_context_repository>         prach_repo;
  std::shared_ptr<uplink_context_repository>        slot_repo;
  std::unique_ptr<receiver>                         ofh_receiver;
  std::unique_ptr<transmitter>                      ofh_transmitter;
  sector_controller                                 ofh_sector_controller;
};

} // namespace ofh
} // namespace srsran
