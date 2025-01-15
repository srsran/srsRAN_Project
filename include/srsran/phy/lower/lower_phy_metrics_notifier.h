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

#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

struct lower_phy_baseband_metrics;

/// Lower physical layer interface used to notify basebend measurements.
class lower_phy_metrics_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_metrics_notifier() = default;

  /// \brief Notifies a new transmit symbol measurement.
  ///
  /// \param[in] metrics Measurements of the transmitted symbol.
  virtual void on_new_transmit_metrics(const lower_phy_baseband_metrics& metrics) = 0;

  /// \brief Notifies a new receive symbol measurement.
  ///
  /// \param[in] metrics Measurements of the received symbol.
  virtual void on_new_receive_metrics(const lower_phy_baseband_metrics& metrics) = 0;
};

} // namespace srsran
