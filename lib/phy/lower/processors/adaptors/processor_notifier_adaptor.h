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

#include "srsgnb/phy/lower/lower_phy_error_notifier.h"
#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor_notifier.h"

namespace srsgnb {

/// \brief Lower physical layer processor notification adaptor.
///
/// Adapts processor notifications to lower physical layer interfaces.
class processor_notifier_adaptor
{
public:
  /// Connects the adaptor with a lower physical layer reception notifier.
  void connect_rx_symbol_notifier(lower_phy_rx_symbol_notifier& notifier)
  {
    prach.connect_rx_symbol_notifier(notifier);
  }

  /// Connects the adaptor to a lower physical layer error notifier.
  void connect_error_notifier(lower_phy_error_notifier& notifier) { prach.connect_error_notifier(notifier); }

  /// Gets the PRACH processor notifier adaptor.
  prach_processor_notifier& get_prach_notifier() { return prach; }

private:
  /// Internal PRACH processor adaptor.
  class prach_adaptor : public prach_processor_notifier
  {
  public:
    /// Connects the adaptor with a lower physical layer reception notifier.
    void connect_rx_symbol_notifier(lower_phy_rx_symbol_notifier& notifier) { rx_notifier = &notifier; }

    /// Connects the adaptor to a lower physical layer error notifier.
    void connect_error_notifier(lower_phy_error_notifier& notifier) { error_notifier = &notifier; }

    // See interface for documentation.
    void on_prach_request_late(const prach_buffer_context& context) override;

    // See interface for documentation.
    void on_prach_request_overflow(const prach_buffer_context& context) override;

    // See interface for documentation.
    void on_rx_prach_window(const prach_buffer& buffer, const prach_buffer_context& context) override;

  private:
    /// Receive notifier.
    lower_phy_rx_symbol_notifier* rx_notifier = nullptr;
    /// Error notifier.
    lower_phy_error_notifier* error_notifier = nullptr;
  };

  /// PRACH adaptor.
  prach_adaptor prach;
};

} // namespace srsgnb