/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/upper_phy_rx_results_notifier.h"

namespace srsran {

class upper_phy_rx_results_notifier_spy : public upper_phy_rx_results_notifier
{
  bool prach_results_notified = false;

public:
  void on_new_prach_results(const ul_prach_results& result) override { prach_results_notified = true; }

  void on_new_pusch_results(const ul_pusch_results& result) override {}

  void on_new_pucch_results(const ul_pucch_results& result) override {}

  bool has_prach_result_been_notified() const { return prach_results_notified; }
};

} // namespace srsran
