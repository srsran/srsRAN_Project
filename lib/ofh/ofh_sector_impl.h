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

#include "ofh_metrics_collector_impl.h"
#include "ofh_sector_controller.h"
#include "support/prach_context_repository.h"
#include "support/uplink_context_repository.h"
#include "support/uplink_cplane_context_repository.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_transmitter.h"
#include "srsran/ofh/ofh_sector.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"

namespace srsran {
namespace ofh {

/// Sector implementation configuration.
struct sector_impl_config {
  unsigned sector_id;
  bool     are_metrics_enabled = false;
};

/// Sector implementation dependencies.
struct sector_impl_dependencies {
  std::unique_ptr<receiver>                  ofh_receiver;
  std::unique_ptr<transmitter>               ofh_transmitter;
  std::shared_ptr<uplink_context_repository> ul_data_repo;
  ether::transmitter&                        eth_transmitter;
  ether::receiver&                           eth_receiver;
};

/// Open Fronthaul sector implementation.
class sector_impl : public sector
{
public:
  sector_impl(const sector_impl_config& config, sector_impl_dependencies&& dependencies) :
    ofh_receiver(std::move(dependencies.ofh_receiver)),
    ofh_transmitter(std::move(dependencies.ofh_transmitter)),
    ofh_sector_controller(*ofh_transmitter, *ofh_receiver, std::move(dependencies.ul_data_repo)),
    ofh_metrics_collector(ofh_receiver->get_metrics_collector(),
                          ofh_transmitter->get_metrics_collector(),
                          config.sector_id)
  {
    srsran_assert(ofh_receiver, "Invalid Open Fronthaul receiver");
    srsran_assert(ofh_transmitter, "Invalid Open Fronthaul transmitter");
  }

  // See interface for documentation.
  receiver& get_receiver() override;

  // See interface for documentation.
  transmitter& get_transmitter() override;

  // See interface for documentation.
  operation_controller& get_operation_controller() override;

  // See interface for documentation.
  metrics_collector* get_metrics_collector() override;

private:
  std::unique_ptr<receiver>    ofh_receiver;
  std::unique_ptr<transmitter> ofh_transmitter;
  sector_controller            ofh_sector_controller;
  metrics_collector_impl       ofh_metrics_collector;
};

} // namespace ofh
} // namespace srsran
