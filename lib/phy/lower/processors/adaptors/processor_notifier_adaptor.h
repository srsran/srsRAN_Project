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

#include "srsran/phy/lower/lower_phy_error_notifier.h"
#include "srsran/phy/lower/lower_phy_metrics_notifier.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsran/phy/lower/lower_phy_timing_notifier.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_notifier.h"

namespace srsran {

/// \brief Lower physical layer processor notification adaptor.
///
/// Adapts processor notifications to lower physical layer interfaces.
class processor_notifier_adaptor
{
public:
  /// Connects the adaptor to a lower physical layer error notifier.
  void connect_error_notifier(lower_phy_error_notifier& notifier)
  {
    pdxch.connect_error_notifier(notifier);
    prach.connect_error_notifier(notifier);
    puxch.connect_error_notifier(notifier);
  }

  /// Connects the adaptor with a lower physical layer metrics notifier.
  void connect_metrics_notifier(lower_phy_metrics_notifier& notifier)
  {
    downlink.connect_metrics_notifier(notifier);
    uplink.connect_metrics_notifier(notifier);
  }

  /// Connects the adaptor with a lower physical layer reception notifier.
  void connect_rx_symbol_notifier(lower_phy_rx_symbol_notifier& notifier)
  {
    prach.connect_rx_symbol_notifier(notifier);
    puxch.connect_rx_symbol_notifier(notifier);
  }

  /// Connects the adaptor with a lower physical layer timing notifier.
  void connect_timing_notifier(lower_phy_timing_notifier& notifier)
  {
    downlink.connect_timing_notifier(notifier);
    uplink.connect_timing_notifier(notifier);
  }

  /// Gets the downlink processor notifier adaptor.
  downlink_processor_notifier& get_downlink_notifier() { return downlink; }

  /// Gets the PDxCH processor notifier adaptor.
  pdxch_processor_notifier& get_pdxch_notifier() { return pdxch; }

  /// Gets the Uplink processor notifier adaptor.
  uplink_processor_notifier& get_uplink_notifier() { return uplink; }

  /// Gets the PRACH processor notifier adaptor.
  prach_processor_notifier& get_prach_notifier() { return prach; }

  /// Gets the PUxCH processor notifier adaptor.
  puxch_processor_notifier& get_puxch_notifier() { return puxch; }

private:
  /// Internal downlink processor adaptor.
  class downlink_adaptor : public downlink_processor_notifier
  {
  public:
    /// Connects the adaptor with a lower physical layer metrics notifier.
    void connect_metrics_notifier(lower_phy_metrics_notifier& notifier) { metric_notifier = &notifier; }

    /// Connects the adaptor with a lower physical layer timing notifier.
    void connect_timing_notifier(lower_phy_timing_notifier& notifier) { timing_notifier = &notifier; }

    // See interface for documentation.
    void on_tti_boundary(const lower_phy_timing_context& context) override;

    // See interface for documentation.
    void on_new_metrics(const lower_phy_baseband_metrics& metrics) override;

  private:
    /// Metrics notifier.
    lower_phy_metrics_notifier* metric_notifier = nullptr;
    /// Timing notifier.
    lower_phy_timing_notifier* timing_notifier = nullptr;
  };

  /// Internal PRACH processor adaptor.
  class uplink_adaptor : public uplink_processor_notifier
  {
  public:
    /// Connects the adaptor with a lower physical layer metrics notifier.
    void connect_metrics_notifier(lower_phy_metrics_notifier& notifier) { metric_notifier = &notifier; }

    /// Connects the adaptor with a lower physical layer timing notifier.
    void connect_timing_notifier(lower_phy_timing_notifier& notifier) { timing_notifier = &notifier; }

    // See interface for documentation.
    void on_half_slot(const lower_phy_timing_context& context) override;

    // See interface for documentation.
    void on_full_slot(const lower_phy_timing_context& context) override;

    // See interface for documentation.
    void on_new_metrics(const lower_phy_baseband_metrics& metrics) override;

  private:
    /// Metrics notifier.
    lower_phy_metrics_notifier* metric_notifier = nullptr;
    /// Timing notifier.
    lower_phy_timing_notifier* timing_notifier = nullptr;
  };

  /// Internal PRACH processor adaptor.
  class pdxch_adaptor : public pdxch_processor_notifier
  {
  public:
    /// Connects the adaptor with a lower physical layer error notifier.
    void connect_error_notifier(lower_phy_error_notifier& notifier) { error_notifier = &notifier; }

    // See interface for documentation.
    void on_pdxch_request_late(const resource_grid_context& context) override;

  private:
    /// Error notifier.
    lower_phy_error_notifier* error_notifier = nullptr;
  };

  /// Internal PRACH processor adaptor.
  class prach_adaptor : public prach_processor_notifier
  {
  public:
    /// Connects the adaptor with a lower physical layer reception notifier.
    void connect_rx_symbol_notifier(lower_phy_rx_symbol_notifier& notifier) { rx_notifier = &notifier; }

    /// Connects the adaptor with a lower physical layer error notifier.
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

  /// Internal PUxCH processor adaptor.
  class puxch_adaptor : public puxch_processor_notifier
  {
  public:
    /// Connects the adaptor with a lower physical layer reception notifier.
    void connect_rx_symbol_notifier(lower_phy_rx_symbol_notifier& notifier) { rx_notifier = &notifier; }

    /// Connects the adaptor with a lower physical layer error notifier.
    void connect_error_notifier(lower_phy_error_notifier& notifier) { error_notifier = &notifier; }

    // See interface for documentation.
    void on_puxch_request_late(const resource_grid_context& context) override;

    // See interface for documentation.
    void on_rx_symbol(const shared_resource_grid& grid, const lower_phy_rx_symbol_context& context) override;

  private:
    /// Receive notifier.
    lower_phy_rx_symbol_notifier* rx_notifier = nullptr;
    /// Error notifier.
    lower_phy_error_notifier* error_notifier = nullptr;
  };

  /// Downlink processor adaptor.
  downlink_adaptor downlink;
  /// PDxCH processor adaptor.
  pdxch_adaptor pdxch;
  /// Uplink processor adaptor.
  uplink_adaptor uplink;
  /// PRACH processor adaptor.
  prach_adaptor prach;
  /// PUxCH processor adaptor.
  puxch_adaptor puxch;
};

} // namespace srsran
