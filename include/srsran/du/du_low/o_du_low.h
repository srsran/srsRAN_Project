/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

namespace srsran {

namespace fapi_adaptor {
class phy_fapi_adaptor;
}

class du_operation_controller;
class upper_phy_metrics_notifiers;

namespace srs_du {

class du_low;
class o_du_low_metrics_collector;

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

  /// Returns the metrics collector of this O-DU low.
  virtual o_du_low_metrics_collector* get_metrics_collector() = 0;
};

} // namespace srs_du
} // namespace srsran
