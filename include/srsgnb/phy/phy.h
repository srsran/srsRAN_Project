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

/// Describes context of the new received symbol
struct phy_rx_symbol_context_t {
  slot_context_t slot;
  unsigned       sector;
  unsigned       symbol;
};

/// Physical layer receive symbol notification interface
class phy_rx_symbol_notifier
{
public:
  /// Default destructor
  virtual ~phy_rx_symbol_notifier() = default;

  /// Notifies the arrival of all packets for given CC
  ///
  /// \param [in] context Provides the notification context
  /// \param [in] grid Provides the resource grids for each port of the given sector
  virtual void on_rx_symbol(const phy_rx_symbol_context_t& context, const span<resource_grid_reader>& grid) = 0;

  /// Notifies the arrival of PRACH packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void on_rx_prach_symbol(const phy_rx_symbol_context_t& context) = 0;

  /// Notifies the arrival of SRS packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void on_rx_srs_symbol(const phy_rx_symbol_context_t& context) = 0;
};

/// Describes the context of the current timing boundary
struct phy_boundary_context_t {
  slot_context_t slot;
};

/// Physical layer timing boundary notification interface
class phy_timing_notifier
{
public:
  /// Default destructor
  virtual ~phy_timing_notifier() = default;

  /// Notifies a new TTI boundary event
  ///
  /// \param [in] context Provides the notification context
  virtual void on_tti_boundary(const phy_boundary_context_t& context) = 0;

  /// Notifies a half slot (sym 7) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void on_ul_half_slot_boundary(const phy_boundary_context_t& context) = 0;

  /// Notifies a full slot (sym 14) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void on_ul_full_slot_boundary(const phy_boundary_context_t& context) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_PHY_H
