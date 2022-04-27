#ifndef SRSGNB_PHY_ADAPTERS_PHY_RG_GATEWAY_ADAPTER_H
#define SRSGNB_PHY_ADAPTERS_PHY_RG_GATEWAY_ADAPTER_H

#include "srsgnb/phy/lower/lower_phy_input_gateway.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"

namespace srsgnb {

/// Implements a generic physical layer resource grid gateway adapter.
class phy_rg_gateway_adapter : public upper_phy_rg_gateway
{
private:
  lower_phy_input_gateway* gateway = nullptr;

public:
  /// Connects the adaptor to the lower physical layer gateway.
  void connect(lower_phy_input_gateway* lower_gateway) { gateway = lower_gateway; }

  // See interface for documentation.
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    srsran_always_assert(gateway, "Adapter is not connected.");
    gateway->send(context, grid);
  }
};

} // namespace srsgnb

#endif // SRSGNB_PHY_ADAPTERS_PHY_RG_GATEWAY_ADAPTER_H
