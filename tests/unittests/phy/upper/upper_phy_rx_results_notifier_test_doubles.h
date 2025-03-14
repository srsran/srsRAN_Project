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
