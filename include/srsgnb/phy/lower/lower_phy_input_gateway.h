#ifndef SRSGNB_PHY_LOWER_LOWER_PHY_INPUT_GATEWAY_H
#define SRSGNB_PHY_LOWER_LOWER_PHY_INPUT_GATEWAY_H

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/resource_grid_context.h"

namespace srsgnb {

/// Describes the lower physical layer input gateway.
class lower_phy_input_gateway
{
public:
  /// Default destructor.
  virtual ~lower_phy_input_gateway() = default;

  /// \brief Sends resource grid through the gateway.
  /// \param[in] context Indicates the resource grid context.
  /// \param[in] grid Provides the resource grid to transmit.
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_LOWER_PHY_INPUT_GATEWAY_H
