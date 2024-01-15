/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "upper_phy_rx_results_notifier_wrapper.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

void upper_phy_rx_results_notifier_wrapper::on_new_prach_results(const ul_prach_results& result)
{
  report_fatal_error_if_not(rx_results_notifier, "Invalid results notifier object.");
  rx_results_notifier->on_new_prach_results(result);
}

void upper_phy_rx_results_notifier_wrapper::on_new_pusch_results_control(const ul_pusch_results_control& result)
{
  report_fatal_error_if_not(rx_results_notifier, "Invalid results notifier object.");
  rx_results_notifier->on_new_pusch_results_control(result);
}

void upper_phy_rx_results_notifier_wrapper::on_new_pusch_results_data(const ul_pusch_results_data& result)
{
  report_fatal_error_if_not(rx_results_notifier, "Invalid results notifier object.");
  rx_results_notifier->on_new_pusch_results_data(result);
}

void upper_phy_rx_results_notifier_wrapper::on_new_pucch_results(const ul_pucch_results& result)
{
  report_fatal_error_if_not(rx_results_notifier, "Invalid results notifier object.");
  rx_results_notifier->on_new_pucch_results(result);
}

void upper_phy_rx_results_notifier_wrapper::connect(upper_phy_rx_results_notifier& n)
{
  report_fatal_error_if_not(!rx_results_notifier, "Results notifier already configured.");
  rx_results_notifier = &n;
}
