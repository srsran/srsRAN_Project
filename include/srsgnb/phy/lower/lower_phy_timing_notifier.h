#pragma once

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the context of the current timing boundary.
struct lower_phy_timing_context {
  /// Indicates the slot.
  slot_point slot;
};

/// Lower physical layer interface to notify timing related events.
class lower_phy_timing_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// Notifies that it started processing a new slot. It indicates in \c context the next slot available for
  /// transmission.
  ///
  /// \param [in] context Provides the notification context.
  virtual void on_tti_boundary(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies that an uplink half slot has been received and processed by the lower PHY.
  /// \param[in] context Provides the notification context
  virtual void on_ul_half_slot_boundary(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies that an uplink full slot has been received and processed by the lower PHY.
  ///
  /// After this notification, the lower PHY does not report more uplink receive symbols events for the given context.
  ///
  /// \param[in] context Provides the notification context
  virtual void on_ul_full_slot_boundary(const lower_phy_timing_context& context) = 0;
};
} // namespace srsgnb
