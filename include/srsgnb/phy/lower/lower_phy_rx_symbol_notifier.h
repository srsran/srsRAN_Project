
#pragma once

#include "srsgnb/phy/prach_buffer.h"
#include "srsgnb/phy/prach_buffer_context.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes context of the new received symbol.
struct lower_phy_rx_symbol_context {
  /// Indicates the slot.
  slot_point slot;
  /// Indicates the sector.
  unsigned sector;
  /// Indicates the number of symbols that are available for processing.
  unsigned nof_symbols;
};

/// Lower physical layer's notifier to provide received symbol.
class lower_phy_rx_symbol_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_rx_symbol_notifier() = default;

  /// \brief Notifies the completion of an OFDM symbol for a given context.
  ///
  /// \param [in] context Provides the notification context.
  /// \param [in] grid Provides the resource grid that belongs to the context.
  virtual void on_rx_symbol(const lower_phy_rx_symbol_context& context, const resource_grid_reader& grid) = 0;

  /// \brief Notifies the completion of PRACH window.
  ///
  /// The lower PHY uses this method to notify that a PRACH window identified by \c context has been written in \c
  /// buffer.
  ///
  /// \param [in] context Provides the PRACH context.
  /// \param [in] buffer  Read-only PRACH buffer.
  virtual void on_rx_prach_window(const prach_buffer_context& context, const prach_buffer* buffer) = 0;

  /// \brief Notifies the completion of SRS symbols.
  ///
  /// \param [in] context Provides the notification context.
  virtual void on_rx_srs_symbol(const lower_phy_rx_symbol_context& context) = 0;
};

} // namespace srsgnb
