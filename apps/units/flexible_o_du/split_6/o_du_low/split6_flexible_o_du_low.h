/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "fapi_adaptor/phy_fapi_p5_sector_operation_request_adaptor.h"
#include "split6_flexible_o_du_low_session_manager.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi_adaptor/mac/p5/mac_fapi_p5_sector_adaptor.h"
#include <memory>

namespace srsran {

/// Split 6 flexible O-DU low dependencies.
struct split6_flexible_o_du_low_dependencies {
  std::unique_ptr<split6_flexible_o_du_low_session_factory>                   odu_low_session_factory;
  std::unique_ptr<fapi_adaptor::mac_fapi_p5_sector_adaptor>                   mac_p5_adaptor;
  std::unique_ptr<fapi_adaptor::phy_fapi_p5_sector_operation_request_adaptor> phy_p5_adaptor;
};

/// \brief Split 6 flexible O-DU low.
///
/// This is the class returned by the application unit when it creates the O-DU low.
/// This object:
///  - Manages/owns the configuration adaptor and the FAPI configuration related objects.
///  - Controls the cell creation/destruction using the cell operation request handler implementation.
class split6_flexible_o_du_low : public du_operation_controller
{
public:
  explicit split6_flexible_o_du_low(split6_flexible_o_du_low_dependencies dependencies);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  split6_flexible_o_du_low_session_manager                                    odu_low_session_manager;
  std::unique_ptr<fapi_adaptor::mac_fapi_p5_sector_adaptor>                   mac_p5_adaptor;
  std::unique_ptr<fapi_adaptor::phy_fapi_p5_sector_operation_request_adaptor> phy_p5_adaptor;
};

} // namespace srsran
