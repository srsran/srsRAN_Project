#pragma once

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the context of the current timing boundary.
struct lower_phy_timing_context {
  /// Indicates the slot.
  slot_point slot;
};

/// Lower physical layer's notifier to align timing boundaries
class lower_phy_timing_notifier
{
public:
  /// Default destructor
  virtual ~lower_phy_timing_notifier() = default;

  /// Notifies a new TTI boundary event
  ///
  /// \param [in] context Provides the notification context
  virtual void on_tti_boundary(const lower_phy_timing_context& context) = 0;

  /// Notifies a half slot (sym 7) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void on_ul_half_slot_boundary(const lower_phy_timing_context& context) = 0;

  /// Notifies a full slot (sym 14) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void on_ul_full_slot_boundary(const lower_phy_timing_context& context) = 0;
};
} // namespace srsgnb
