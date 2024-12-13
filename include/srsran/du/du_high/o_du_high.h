/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

class du_power_controller;

namespace fapi_adaptor {
class mac_fapi_adaptor;
} // namespace fapi_adaptor

namespace srs_du {

class du_high;

/// O-RAN Distributed Unit high interface. O-DU high groups the DU high with the MAC-FAPI adaptor.
class o_du_high
{
public:
  virtual ~o_du_high() = default;

  /// Returns the power controller of this O-RAN DU high.
  virtual du_power_controller& get_power_controller() = 0;

  /// Returns the DU high from this O-RAN DU high.
  virtual du_high& get_du_high() = 0;

  /// Returns the MAC-FAPI adaptor of this O-RAN DU high.
  virtual fapi_adaptor::mac_fapi_adaptor& get_mac_fapi_adaptor() = 0;
};

} // namespace srs_du
} // namespace srsran
