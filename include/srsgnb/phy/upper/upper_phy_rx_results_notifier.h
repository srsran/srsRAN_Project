/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Interface of the upper-PHY notifier in charge of messages carrying the result of uplink detection and
/// decoding.

#pragma once

#include "srsgnb/phy/upper/channel_processors/prach_detector.h"

namespace srsgnb {

/// \brief Interface of the upper-PHY notifier in charge of messages carrying the result of uplink detection and
/// decoding.
class upper_phy_rx_results_notifier
{
public:
  /// Default destructor.
  virtual ~upper_phy_rx_results_notifier() = default;

  /// \brief Notifies the results of a PRACH preamble detection.
  virtual void on_new_prach_results(const prach_detector::detection_result& result) = 0;
};

} // namespace srsgnb
