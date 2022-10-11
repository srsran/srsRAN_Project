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
  // See interface for documentation.
  void on_new_prach_results(const ul_prach_results& result) override;

  // See interface for documentation.
  void on_new_pusch_results(const ul_pusch_results& result) override;

  // See interface for documentation.
  void on_new_pucch_results(const ul_pucch_results& result) override;

  /// Connects this proxy to the notifier the notifications of new upper PHY receive results are forwarded to.
  ///
  /// \param notif RX results notifier to which the notifications will be forwarded.
  void connect(upper_phy_rx_results_notifier& notif);

private:
  /// Pointer to the upper PHY receive results notifier.
  upper_phy_rx_results_notifier* notifier = nullptr;
};

} // namespace srsgnb
