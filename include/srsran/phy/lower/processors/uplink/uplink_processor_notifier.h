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

namespace srsran {

struct lower_phy_baseband_metrics;
struct lower_phy_timing_context;

/// \brief Lower physical layer uplink processor - Notifier interface.
///
/// Notifies time boundary events.
class uplink_processor_notifier
{
public:
  /// Default destructor.
  virtual ~uplink_processor_notifier() = default;

  /// \brief Notifies that an uplink half slot has been received and processed by the lower PHY.
  ///
  /// See \ref lower_phy_timing_notifier::on_ul_half_slot_boundary for more information.
  /// \param[in] context Timing context.
  virtual void on_half_slot(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies that an uplink full slot has been received and processed by the lower PHY.
  ///
  /// See \ref lower_phy_timing_notifier::on_ul_full_slot_boundary for more information.
  /// \param[in] context Timing context.
  virtual void on_full_slot(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies a new measurement of uplink baseband metrics.
  virtual void on_new_metrics(const lower_phy_baseband_metrics& metrics) = 0;
};

} // namespace srsran
