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

#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"

namespace srsran {

class upper_phy_rx_results_notifier_spy : public upper_phy_rx_results_notifier
{
public:
  void on_new_prach_results(const ul_prach_results& result) override { prach_results_notified = true; }

  void on_new_pusch_results_control(const ul_pusch_results_control& result) override
  {
    pusch_uci_results_notified = true;
  }
  void on_new_pusch_results_data(const ul_pusch_results_data& result) override { pusch_data_results_notified = true; }

  void on_new_pucch_results(const ul_pucch_results& result) override { pucch_results_notified = true; }

  void on_new_srs_results(const ul_srs_results& result) override { srs_results_notified = true; }

  bool has_prach_result_been_notified() const { return prach_results_notified; }

  bool has_pusch_uci_result_been_notified() const { return pusch_uci_results_notified; }

  bool has_pusch_data_result_been_notified() const { return pusch_data_results_notified; }

  bool has_pucch_result_been_notified() const { return pucch_results_notified; }

  bool has_srs_result_been_notified() const { return srs_results_notified; }

  void clear()
  {
    prach_results_notified      = false;
    pusch_data_results_notified = false;
    pusch_uci_results_notified  = false;
    pucch_results_notified      = false;
    srs_results_notified        = false;
  }

private:
  bool prach_results_notified      = false;
  bool pusch_data_results_notified = false;
  bool pusch_uci_results_notified  = false;
  bool pucch_results_notified      = false;
  bool srs_results_notified        = false;
};

} // namespace srsran
