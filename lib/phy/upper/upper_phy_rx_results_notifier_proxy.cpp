/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_rx_results_notifier_proxy.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

void upper_phy_rx_results_notifier_proxy::on_new_prach_results(const ul_prach_results& result)
{
  report_fatal_error_if_not(notifier, "Invalid results notifier object.");
  notifier->on_new_prach_results(result);
}

void upper_phy_rx_results_notifier_proxy::on_new_pusch_results(const ul_pusch_results& result)
{
  report_fatal_error_if_not(notifier, "Invalid results notifier object.");
  notifier->on_new_pusch_results(result);
}

void upper_phy_rx_results_notifier_proxy::on_new_pucch_results(const ul_pucch_results& result)
{
  report_fatal_error_if_not(notifier, "Invalid results notifier object.");
  notifier->on_new_pucch_results(result);
}

void upper_phy_rx_results_notifier_proxy::connect(upper_phy_rx_results_notifier& notif)
{
  report_fatal_error_if_not(!notifier, "Results notifier already configured.");
  notifier = &notif;
}
