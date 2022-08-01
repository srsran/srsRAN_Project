/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/lower/lower_phy_controller.h"
#include "srsgnb/phy/lower/lower_phy_request_handler.h"
#include "srsgnb/phy/lower/lower_phy_rg_handler.h"

namespace srsgnb {

/// \brief Lower PHY main interface.
///
/// Provides access to all the lower PHY components.
class lower_phy
{
public:
  /// Default destructor.
  virtual ~lower_phy() = default;

  /// \brief Returns a reference to the lower PHY request handler.
  virtual lower_phy_request_handler& get_request_handler() = 0;

  /// \brief Returns a reference to the lower PHY resource grid handler.
  virtual lower_phy_rg_handler& get_rg_handler() = 0;

  /// \brief Returns a reference to the lower PHY controller.
  virtual lower_phy_controller& get_controller() = 0;
};

} // namespace srsgnb
