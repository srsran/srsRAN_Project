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

#include "ofh_sector_controller.h"
#include "support/prach_context_repository.h"
#include "support/uplink_context_repository.h"
#include "support/uplink_cplane_context_repository.h"
#include "srsran/ofh/ofh_sector.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul sector implementation dependencies.
struct sector_impl_dependencies {
  std::unique_ptr<receiver>                         ofh_receiver;
  std::unique_ptr<transmitter>                      ofh_transmitter;
  std::shared_ptr<uplink_cplane_context_repository> cp_repo;
  std::shared_ptr<uplink_cplane_context_repository> cp_prach_repo;
  std::shared_ptr<prach_context_repository>         prach_repo;
  std::shared_ptr<uplink_context_repository>        slot_repo;
};

/// Open Fronthaul sector implementation.
class sector_impl : public sector
{
public:
  sector_impl(sector_impl_dependencies&& dependencies) :
    cp_repo(std::move(dependencies.cp_repo)),
    cp_prach_repo(std::move(dependencies.cp_prach_repo)),
    prach_repo(std::move(dependencies.prach_repo)),
    slot_repo(std::move(dependencies.slot_repo)),
    ofh_receiver(std::move(dependencies.ofh_receiver)),
    ofh_transmitter(std::move(dependencies.ofh_transmitter)),
    ofh_sector_controller(*ofh_receiver)
  {
    srsran_assert(ofh_receiver, "Invalid Open Fronthaul receiver");
    srsran_assert(ofh_transmitter, "Invalid Open Fronthaul transmitter");
    srsran_assert(cp_repo, "Invalid Control-Plane uplink context repository");
    srsran_assert(cp_prach_repo, "Invalid Control-Plane PRACH context repository");
    srsran_assert(prach_repo, "Invalid PRACH context repository");
    srsran_assert(slot_repo, "Invalid UL slot context repository");
  }

  // See interface for documentation.
  receiver& get_receiver() override;

  // See interface for documentation.
  transmitter& get_transmitter() override;

  // See interface for documentation.
  controller& get_controller() override;

  // See interface for documentation.
  void set_error_notifier(error_notifier& notifier) override;

private:
  std::shared_ptr<uplink_cplane_context_repository> cp_repo;
  std::shared_ptr<uplink_cplane_context_repository> cp_prach_repo;
  std::shared_ptr<prach_context_repository>         prach_repo;
  std::shared_ptr<uplink_context_repository>        slot_repo;
  std::unique_ptr<receiver>                         ofh_receiver;
  std::unique_ptr<transmitter>                      ofh_transmitter;
  sector_controller                                 ofh_sector_controller;
};

} // namespace ofh
} // namespace srsran
