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

#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"

namespace srsran {

class rx_softbuffer_pool;

/// \brief Wrapper class of the upper PHY receive results notifier.
///
/// This wrapper object performs maintenance tasks before propagating the result events to the configured notifier
/// object.
class upper_phy_rx_results_notifier_wrapper : public upper_phy_rx_results_notifier
{
public:
  // See interface for documentation.
  void on_new_prach_results(const ul_prach_results& result) override;

  // See interface for documentation.
  void on_new_pusch_results(const ul_pusch_results& result) override;

  // See interface for documentation.
  void on_new_pucch_results(const ul_pucch_results& result) override;

  /// Connects this wrapper to the notifier the notifications of new upper PHY receive results are forwarded to.
  ///
  /// \param[in] n RX results notifier to which the notifications will be forwarded.
  void connect(upper_phy_rx_results_notifier& n);

private:
  /// Pointer to the upper PHY receive results notifier.
  upper_phy_rx_results_notifier* rx_results_notifier = nullptr;
};

} // namespace srsran
