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

#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"

namespace srsran {

namespace ether {
class receiver;
} // namespace ether

namespace ofh {

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

  /// Returns the Ethernet receiver of this Open Fronthaul message receiver.
  virtual ether::receiver& get_ethernet_receiver() = 0;

  /// Returns the message receiver performance metrics collector.
  virtual message_receiver_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace srsran
