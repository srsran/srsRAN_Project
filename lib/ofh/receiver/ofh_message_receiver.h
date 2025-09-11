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

#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"

namespace srsran {
namespace ofh {

class operation_controller;

class message_receiver_metrics_collector;

/// Open Fronthaul message receiver interface.
///
/// This class listens to incoming Ethernet frames and decodes them as Open Fronthaul messages. Once a new message is
/// detected, is it handled to the corresponding data flow for further processing.
class message_receiver : public ether::frame_notifier
{
public:
  /// Default destructor.
  virtual ~message_receiver() = default;

  /// Returns the operation controller of this Open Fronthaul message receiver.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the message receiver performance metrics collector.
  virtual message_receiver_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace srsran
