#ifndef SRSGNB_PHY_UPPER_PHY_H
#define SRSGNB_PHY_UPPER_PHY_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/resource_grid_context.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the upper physical layer output gateway
class upper_phy_output_gateway
{
public:
  /// Default destructor to ensure generic ownership destroys the instance
  virtual ~upper_phy_output_gateway() = default;

  /// Sends resource grid through the gateway
  ///
  /// \param context Provides the context the resource grid belongs
  /// \param grid Provides the resource grid reader instance
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

/// Describes context of the new received symbol
struct upper_phy_rx_symbol_context_t {
  slot_point slot;
  unsigned   sector;
  unsigned   symbol;
};

/// Upper physical layer's handler to process receive symbol
class upper_phy_rx_symbol_handler
{
public:
  /// Default destructor
  virtual ~upper_phy_rx_symbol_handler() = default;

  /// Handles the arrival of all packets for given CC
  ///
  /// \param [in] context Provides the notification context
  /// \param [in] grid Provides the resource grids for each port of the given sector
  virtual void handle_rx_symbol(const upper_phy_rx_symbol_context_t& context,
                                const span<resource_grid_reader>&    grid) = 0;

  /// Handles the arrival of PRACH packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void handle_rx_prach_symbol(const upper_phy_rx_symbol_context_t& context) = 0;

  /// Handles the arrival of SRS packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void handle_rx_srs_symbol(const upper_phy_rx_symbol_context_t& context) = 0;
};

/// Describes the context of the current timing boundary
struct upper_phy_timing_context_t {
  slot_point slot;
};

/// Upper physical layer's handle to align timing boundaries
class upper_phy_timing_handler
{
public:
  /// Default destructor
  virtual ~upper_phy_timing_handler() = default;

  /// Handles a new TTI boundary event
  ///
  /// \param [in] context Provides the notification context
  virtual void handle_tti_boundary(const upper_phy_timing_context_t& context) = 0;

  /// Handles a half slot (sym 7) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void handle_ul_half_slot_boundary(const upper_phy_timing_context_t& context) = 0;

  /// Handles a full slot (sym 14) packet arrival
  ///
  /// \param [in] context Provides the notification context
  virtual void handle_ul_full_slot_boundary(const upper_phy_timing_context_t& context) = 0;
};

} // namespace srsgnb

#endif //  SRSGNB_PHY_UPPER_PHY_H
