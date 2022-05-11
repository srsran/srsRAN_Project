/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_UPPER_PHY_TIMING_NOTIFIER_H
#define SRSGNB_PHY_UPPER_UPPER_PHY_TIMING_NOTIFIER_H

namespace srsgnb {

class slot_point;

/// Upper physical layer's timing notifier.
class upper_phy_timing_notifier
{
public:
  virtual ~upper_phy_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// \param [in] slot Provides the notification context.
  virtual void on_tti_boundary(const slot_point& slot) = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_UPPER_PHY_TIMING_NOTIFIER_H
