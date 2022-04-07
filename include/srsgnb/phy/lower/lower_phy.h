#ifndef SRSGNB_PHY_LOWER_PHY_H
#define SRSGNB_PHY_LOWER_PHY_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/resource_grid_context.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the lower physical layer input gateway.
class lower_phy_input_gateway
{
public:
  /// Default destructor.
  virtual ~lower_phy_input_gateway() = default;

  /// \brief Sends resource grid through the gateway.
  ///
  /// \param[in] context Indicates the resource grid context.
  /// \param[in] grid Provides the resource grid to transmit.
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

/// Describes context of the new received symbol.
struct lower_phy_rx_symbol_context_t {
  /// Indicates the slot.
  slot_point slot;
  /// Indicates the sector.
  unsigned sector;
  /// Indicates the number of symbols that are available for processing.
  unsigned nof_symbols;
};

/// Lower physical layer's notifier to provide received symbol.
class lower_phy_rx_symbol_handler
{
public:
  /// Default destructor.
  virtual ~lower_phy_rx_symbol_handler() = default;

  /// \brief Notifies the completion of an OFDM symbol for a given context.
  ///
  /// \param [in] context Provides the notification context.
  /// \param [in] grid Provides the resource grid that belongs to the context.
  virtual void notify_rx_symbol(const lower_phy_rx_symbol_context_t& context, const resource_grid_reader& grid) = 0;

  /// \brief Notifies the completion of PRACH symbols.
  ///
  /// \param [in] context Provides the notification context.
  virtual void notify_rx_prach_symbol(const lower_phy_rx_symbol_context_t& context) = 0;

  /// \brief Notifies the completion of SRS symbols.
  ///
  /// \param [in] context Provides the notification context.
  virtual void notify_rx_srs_symbol(const lower_phy_rx_symbol_context_t& context) = 0;
};

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
