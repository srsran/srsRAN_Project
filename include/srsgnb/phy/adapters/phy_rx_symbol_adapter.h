
#ifndef SRSGNB_PHY_ADAPTERS_PHY_RX_SYMBOL_ADAPTER_H
#define SRSGNB_PHY_ADAPTERS_PHY_RX_SYMBOL_ADAPTER_H

#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/upper/upper_phy_rx_symbol_handler.h"

namespace srsgnb {

/// Implements a generic physical layer receive symbol adapter.
class phy_rx_symbol_adapter : public lower_phy_rx_symbol_notifier
{
private:
  upper_phy_rx_symbol_handler* rx_symbol_handler = nullptr;

public:
  /// Connects the adaptor to the upper physical layer handler.
  void connect(upper_phy_rx_symbol_handler* upper_handler) { rx_symbol_handler = upper_handler; }

  // See interface for documentation.
  void on_rx_symbol(const lower_phy_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    srsran_always_assert(rx_symbol_handler, "Adapter is not connected.");
    upper_phy_rx_symbol_context upper_context;
    upper_context.slot   = context.slot;
    upper_context.sector = context.sector;
    upper_context.symbol = context.nof_symbols;
    rx_symbol_handler->handle_rx_symbol(upper_context, grid);
  }

  // See interface for documentation.
  void on_rx_prach_window(const prach_buffer_context& context, const prach_buffer* buffer) override
  {
    srsran_always_assert(rx_symbol_handler, "Adapter is not connected.");
    rx_symbol_handler->handle_rx_prach_window(context, buffer);
  }

  // See interface for documentation.
  void on_rx_srs_symbol(const lower_phy_rx_symbol_context& context) override
  {
    srsran_always_assert(rx_symbol_handler, "Adapter is not connected.");
    upper_phy_rx_symbol_context upper_context;
    upper_context.slot   = context.slot;
    upper_context.sector = context.sector;
    upper_context.symbol = context.nof_symbols;
    rx_symbol_handler->handle_rx_srs_symbol(upper_context);
  }
};

} // namespace srsgnb

#endif // SRSGNB_PHY_ADAPTERS_PHY_RX_SYMBOL_ADAPTER_H
