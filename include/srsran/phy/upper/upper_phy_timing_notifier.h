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

struct upper_phy_timing_context;

/// Upper physical layer timing notifier.
class upper_phy_timing_notifier
{
public:
  virtual ~upper_phy_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// \param[in]  context Notification context.
  virtual void on_tti_boundary(const upper_phy_timing_context& context) = 0;
};

} // namespace srsran
