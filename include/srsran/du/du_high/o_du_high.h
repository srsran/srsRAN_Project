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

class du_operation_controller;

namespace fapi_adaptor {
class mac_fapi_adaptor;
} // namespace fapi_adaptor

namespace srs_du {

class du_high;
class o_du_high_metrics_notifier;

/// O-RAN Distributed Unit high interface. O-DU high groups the DU high with the MAC-FAPI adaptor.
class o_du_high
{
public:
  virtual ~o_du_high() = default;

  /// Returns the operation controller of this O-RAN DU high.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the DU high from this O-RAN DU high.
  virtual du_high& get_du_high() = 0;

  /// Returns the MAC-FAPI adaptor of this O-RAN DU high.
  virtual fapi_adaptor::mac_fapi_adaptor& get_mac_fapi_adaptor() = 0;

  /// Sets the O-DU high metrics notifier to the given one.
  virtual void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier) = 0;
};

} // namespace srs_du
} // namespace srsran
