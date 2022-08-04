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

#include "srsgnb/phy/upper/upper_phy_rx_results_notifier.h"

namespace srsgnb {

class upper_phy_rx_results_notifier_spy : public upper_phy_rx_results_notifier
{
  bool prach_results_notified = false;

public:
  void on_new_prach_results(const prach_detector::detection_result& result) override { prach_results_notified = true; }

  bool has_prach_result_been_notified() const { return prach_results_notified; }
};

} // namespace srsgnb
