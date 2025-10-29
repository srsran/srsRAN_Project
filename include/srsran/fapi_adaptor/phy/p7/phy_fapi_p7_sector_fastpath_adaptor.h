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

#include "srsran/fapi_adaptor/phy/p7/phy_fapi_p7_sector_adaptor.h"

namespace srsran {

class upper_phy_error_notifier;
class upper_phy_timing_notifier;
class upper_phy_rx_results_notifier;

namespace fapi_adaptor {

/// \brief PHY-FAPI bidirectional P7 sector fastpath adaptor interface.
///
/// This adaptor is a collection of interfaces to translate FAPI messages into their PHY layer counterpart and vice
/// versa.
class phy_fapi_p7_sector_fastpath_adaptor : public phy_fapi_p7_sector_adaptor
{
public:
  /// Returns a reference to the error notifier used by the adaptor.
  virtual upper_phy_error_notifier& get_error_notifier() = 0;

  /// Returns a reference to the timing notifier used by the adaptor.
  virtual upper_phy_timing_notifier& get_timing_notifier() = 0;

  /// Returns a reference to the results notifier used by the adaptor.
  virtual upper_phy_rx_results_notifier& get_rx_results_notifier() = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
