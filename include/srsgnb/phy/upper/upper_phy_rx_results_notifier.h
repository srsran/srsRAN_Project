/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processor/prach_detector.h"

namespace srsgnb {

/// \brief Upper PHY interface to notify rx decoding results events.
class upper_phy_rx_results_notifier
{
public:
  virtual ~upper_phy_rx_results_notifier() = default;

  /// \brief Notifies the results of a PRACH detection.
  ///
  /// \param result Contains the results of the detected PRACH preambles.
  virtual void on_new_prach_results(const prach_detector::detection_result& result) = 0;
};

} // namespace srsgnb
