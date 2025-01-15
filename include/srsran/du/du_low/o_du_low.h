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

namespace srsran {

namespace fapi_adaptor {
class phy_fapi_adaptor;
}

class du_operation_controller;

namespace srs_du {

class du_low;

/// O-RAN DU low interface. O-DU low groups the DU low with the PHY-FAPI adaptor.
class o_du_low
{
public:
  /// Default destructor.
  virtual ~o_du_low() = default;

  /// Returns the operation controller of this DU low.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the upper PHY of this O-DU low.
  virtual du_low& get_du_low() = 0;

  /// Returns the PHY&ndash;FAPI adaptor of this O-DU low.
  virtual fapi_adaptor::phy_fapi_adaptor& get_phy_fapi_adaptor() = 0;
};

} // namespace srs_du
} // namespace srsran
