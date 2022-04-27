#ifndef SRSGNB_PHY_UPPER_UPPER_PHY_TIMING_NOTIFIER_H
#define SRSGNB_PHY_UPPER_UPPER_PHY_TIMING_NOTIFIER_H

namespace srsgnb {

class slot_point;

/// Upper physical layer's timing notifier.
class upper_phy_timing_notifier
{
public:
  /// Default destructor
  virtual ~upper_phy_timing_notifier() = default;

  /// Notifies a new TTI boundary event
  ///
  /// \param [in] slot Provides the notification context
  virtual void on_tti_boundary(const slot_point& slot) = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_UPPER_PHY_TIMING_NOTIFIER_H
