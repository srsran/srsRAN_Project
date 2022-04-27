
#ifndef SRSGNB_PHY_LOWER_LOWER_PHY_RX_SYMBOL_NOTIFIER_H
#define SRSGNB_PHY_LOWER_LOWER_PHY_RX_SYMBOL_NOTIFIER_H

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

  /// \brief Notifies the completion of PRACH symbols.
  ///
  /// \param [in] context Provides the notification context.
  virtual void on_rx_prach_symbol(const lower_phy_rx_symbol_context& context) = 0;

  /// \brief Notifies the completion of SRS symbols.
  ///
  /// \param [in] context Provides the notification context.
  virtual void on_rx_srs_symbol(const lower_phy_rx_symbol_context& context) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_LOWER_PHY_RX_SYMBOL_NOTIFIER_H
