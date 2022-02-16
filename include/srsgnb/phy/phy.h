#ifndef SRSGNB_PHY_PHY_H
#define SRSGNB_PHY_PHY_H

#include "srsgnb/adt/slot_context.h"
#include "srsgnb/support/resource_grid.h"
#include "srsgnb/support/resource_grid_context.h"
#include <complex>

namespace srsgnb {

/// Describes the physical layer output gateway
class phy_output_gateway
{
public:
  /// Default destructor to ensure generic ownership destroys the instance
  virtual ~phy_output_gateway() = default;

  /// Sends resource grid through the gateway
  ///
  /// \param context Provides the context the resource grid belongs
  /// \param grid Provides the resource grid reader instance
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

class phy_notifier
{
public:
  /// Describes an RX packet notifier context
  struct rx_context_t {
    slot_context_t slot;
    unsigned       sector;
    unsigned       symbol;
  };

  /// Describes an event notifier context
  struct event_context_t {
    slot_context_t slot;
  };

  /// Default destructor
  virtual ~phy_notifier() = default;

  /// Notifies the arrival of all packets for given CC
  ///
  /// \param [in] context Provides the notification context
  /// \param [in] grid Provides the resource grids for each port of the given sector
  virtual void rx(const rx_context_t& context, const span<resource_grid_reader>& grid) = 0;

  /// Notifies the arrival of PRACH packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void rx_prach(const rx_context_t& context) = 0;

  /// Notifies the arrival of SRS packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void rx_srs(const rx_context_t& context) = 0;

  /// Notifies a new TTI boundary event
  ///
  /// \param [in] context Provides the notification context
  virtual void tti(const event_context_t& context) = 0;

  /// Notifies a half slot (sym 7) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void ul_half_slot(const event_context_t& context) = 0;

  /// Notifies a full slot (sym 14) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void ul_full_slot(const event_context_t& context) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_PHY_H
