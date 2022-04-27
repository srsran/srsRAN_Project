#ifndef SRSGNB_PHY_UPPER_UPPER_PHY_RG_GATEWAY_H
#define SRSGNB_PHY_UPPER_UPPER_PHY_RG_GATEWAY_H

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/resource_grid_context.h"

namespace srsgnb {

/// Describes the upper physical layer output gateway.
class upper_phy_rg_gateway
{
public:
  /// Default destructor.
  virtual ~upper_phy_rg_gateway() = default;

  /// \brief Sends resource grid through the gateway.
  /// \param[in] context Indicates the context the resource grid belongs.
  /// \param[in] grid Provides the resource grid reader instance.
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

} // namespace srsgnb

#endif //  SRSGNB_PHY_UPPER_UPPER_PHY_RG_GATEWAY_H
