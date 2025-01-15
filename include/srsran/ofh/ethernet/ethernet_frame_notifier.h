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

#include "srsran/adt/span.h"
#include "srsran/ofh/ethernet/ethernet_unique_buffer.h"

namespace srsran {
namespace ether {

/// Describes an Ethernet frame notifier.
class frame_notifier
{
public:
  /// Default destructor.
  virtual ~frame_notifier() = default;

  /// Notifies the reception of an Ethernet frame coming from the underlying Ethernet link.
  virtual void on_new_frame(unique_rx_buffer buffer) = 0;
};

} // namespace ether
} // namespace srsran
