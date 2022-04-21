#ifndef SRSGNB_PHY_LOWER_LOWER_PHY_TIMING_HANDLER_H
#define SRSGNB_PHY_LOWER_LOWER_PHY_TIMING_HANDLER_H

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the context of the current timing boundary.
struct lower_phy_timing_context_t {
  /// Indicates the slot.
  slot_point slot;
};

/// Lower physical layer's notifier to align timing boundaries
class lower_phy_timing_handler
{
public:
  /// Default destructor
  virtual ~lower_phy_timing_handler() = default;

  /// Notifies a new TTI boundary event
  ///
  /// \param [in] context Provides the notification context
  virtual void on_tti_boundary(const lower_phy_timing_context_t& context) = 0;

  /// Notifies a half slot (sym 7) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void on_ul_half_slot_boundary(const lower_phy_timing_context_t& context) = 0;

  /// Notifies a full slot (sym 14) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void on_ul_full_slot_boundary(const lower_phy_timing_context_t& context) = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_LOWER_PHY_TIMING_HANDLER_H
