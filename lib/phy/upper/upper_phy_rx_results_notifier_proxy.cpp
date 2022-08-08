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

using namespace srsgnb;

namespace {

class upper_phy_rx_results_notifier_dummy : public upper_phy_rx_results_notifier
{
public:
  void on_new_prach_results(const prach_detector::detection_result& result) override {}
};

} // namespace

static upper_phy_rx_results_notifier_dummy dummy_notifier;

upper_phy_rx_results_notifier_proxy::upper_phy_rx_results_notifier_proxy() : notifier(dummy_notifier) {}

void upper_phy_rx_results_notifier_proxy::on_new_prach_results(const prach_detector::detection_result& result)
{
  notifier.get().on_new_prach_results(result);
}

void upper_phy_rx_results_notifier_proxy::connect(upper_phy_rx_results_notifier& notif)
{
  notifier = std::ref(notif);
}
