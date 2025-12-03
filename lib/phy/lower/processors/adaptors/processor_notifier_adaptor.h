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
  processor_notifier_adaptor(lower_phy_error_notifier&     err_notifier,
                             lower_phy_metrics_notifier&   metrics_notifier,
                             lower_phy_rx_symbol_notifier& rx_symbol_notifier,
                             lower_phy_timing_notifier&    timing_notifier) :
    downlink(metrics_notifier, timing_notifier),
    pdxch(err_notifier),
    uplink(metrics_notifier, timing_notifier),
    prach(rx_symbol_notifier, err_notifier),
    puxch(rx_symbol_notifier, err_notifier)
  {
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
    downlink_adaptor(lower_phy_metrics_notifier& metric_notifier_, lower_phy_timing_notifier& timing_notifier_) :
      metric_notifier(metric_notifier_), timing_notifier(timing_notifier_)
    {
    }

    // See interface for documentation.
    void on_tti_boundary(const lower_phy_timing_context& context) override;

    // See interface for documentation.
    void on_new_metrics(const lower_phy_baseband_metrics& metrics) override;

  private:
    /// Metrics notifier.
    lower_phy_metrics_notifier& metric_notifier;
    /// Timing notifier.
    lower_phy_timing_notifier& timing_notifier;
  };

  /// Internal PRACH processor adaptor.
  class uplink_adaptor : public uplink_processor_notifier
  {
  public:
    uplink_adaptor(lower_phy_metrics_notifier& metric_notifier_, lower_phy_timing_notifier& timing_notifier_) :
      metric_notifier(metric_notifier_), timing_notifier(timing_notifier_)
    {
    }

    // See interface for documentation.
    void on_half_slot(const lower_phy_timing_context& context) override;

    // See interface for documentation.
    void on_full_slot(const lower_phy_timing_context& context) override;

    // See interface for documentation.
    void on_new_metrics(const lower_phy_baseband_metrics& metrics) override;

  private:
    /// Metrics notifier.
    lower_phy_metrics_notifier& metric_notifier;
    /// Timing notifier.
    lower_phy_timing_notifier& timing_notifier;
  };

  /// Internal PRACH processor adaptor.
  class pdxch_adaptor : public pdxch_processor_notifier
  {
  public:
    explicit pdxch_adaptor(lower_phy_error_notifier& error_notifier_) : error_notifier(error_notifier_) {}

    // See interface for documentation.
    void on_pdxch_request_late(const resource_grid_context& context) override;

  private:
    /// Error notifier.
    lower_phy_error_notifier& error_notifier;
  };

  /// Internal PRACH processor adaptor.
  class prach_adaptor : public prach_processor_notifier
  {
  public:
    prach_adaptor(lower_phy_rx_symbol_notifier& rx_notifier_, lower_phy_error_notifier& error_notifier_) :
      rx_notifier(rx_notifier_), error_notifier(error_notifier_)
    {
    }

    // See interface for documentation.
    void on_prach_request_late(const prach_buffer_context& context) override;

    // See interface for documentation.
    void on_prach_request_overflow(const prach_buffer_context& context) override;

    // See interface for documentation.
    void on_rx_prach_window(shared_prach_buffer buffer, const prach_buffer_context& context) override;

  private:
    /// Receive notifier.
    lower_phy_rx_symbol_notifier& rx_notifier;
    /// Error notifier.
    lower_phy_error_notifier& error_notifier;
  };

  /// Internal PUxCH processor adaptor.
  class puxch_adaptor : public puxch_processor_notifier
  {
  public:
    puxch_adaptor(lower_phy_rx_symbol_notifier& rx_notifier_, lower_phy_error_notifier& error_notifier_) :
      rx_notifier(rx_notifier_), error_notifier(error_notifier_)
    {
    }

    // See interface for documentation.
    void on_puxch_request_late(const resource_grid_context& context) override;

    // See interface for documentation.
    void
    on_rx_symbol(const shared_resource_grid& grid, const lower_phy_rx_symbol_context& context, bool is_valid) override;

  private:
    /// Receive notifier.
    lower_phy_rx_symbol_notifier& rx_notifier;
    /// Error notifier.
    lower_phy_error_notifier& error_notifier;
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
