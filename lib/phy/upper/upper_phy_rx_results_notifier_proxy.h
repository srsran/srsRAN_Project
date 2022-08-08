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

/// \brief Proxy implementation of the upper PHY receive results notifier.
///
/// It allows the client to access multiple upper PHY receive results notifier without acquiring their ownership.
class upper_phy_rx_results_notifier_proxy : public upper_phy_rx_results_notifier
{
public:
  upper_phy_rx_results_notifier_proxy();

  // See interface for documentation.
  void on_new_prach_results(const prach_detector::detection_result& result) override;

  /// Connects this proxy to the notifier the notifications of new upper PHY receive results are forwarded to.
  ///
  /// \param notif RX results notifier to which the notifications will be forwarded.
  void connect(upper_phy_rx_results_notifier& notif);

private:
  /// Reference to the upper PHY receive results notifier.
  std::reference_wrapper<upper_phy_rx_results_notifier> notifier;
};

} // namespace srsgnb
