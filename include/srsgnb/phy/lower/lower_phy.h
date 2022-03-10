#ifndef SRSGNB_PHY_LOWER_PHY_H
#define SRSGNB_PHY_LOWER_PHY_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/resource_grid_context.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the lower physical layer input gateway
class lower_phy_input_gateway
{
public:
  /// Default destructor to ensure generic ownership destroys the instance
  virtual ~lower_phy_input_gateway() = default;

  /// Sends resource grid through the gateway
  ///
  /// \param context Provides the context the resource grid belongs
  /// \param grid Provides the resource grid reader instance
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

/// Describes context of the new received symbol
struct lower_phy_rx_symbol_context_t {
  slot_point slot;
  unsigned   sector;
  unsigned   symbol;
};

/// Lower physical layer's notifier to provide received symbol
class lower_phy_rx_symbol_notifier
{
public:
  /// Default destructor
  virtual ~lower_phy_rx_symbol_notifier() = default;

  /// Notifies the arrival of all packets for given CC
  ///
  /// \param [in] context Provides the notification context
  /// \param [in] grid Provides the resource grids for each port of the given sector
  virtual void notify_rx_symbol(const lower_phy_rx_symbol_context_t& context,
                                const span<resource_grid_reader>&    grid) = 0;

  /// Notifies the arrival of PRACH packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void notify_rx_prach_symbol(const lower_phy_rx_symbol_context_t& context) = 0;

  /// Notifies the arrival of SRS packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void notify_rx_srs_symbol(const lower_phy_rx_symbol_context_t& context) = 0;
};

/// Describes the context of the current timing boundary
struct lower_phy_timing_context_t {
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
  virtual void notify_tti_boundary(const lower_phy_timing_context_t& context) = 0;

  /// Notifies a half slot (sym 7) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void notify_ul_half_slot_boundary(const lower_phy_timing_context_t& context) = 0;

  /// Notifies a full slot (sym 14) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void notify_ul_full_slot_boundary(const lower_phy_timing_context_t& context) = 0;
};

} // namespace srsgnb

#endif //  SRSGNB_PHY_LOWER_PHY_H
