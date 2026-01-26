/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

class mac_fapi_p7_sector_fastpath_adaptor;
class operation_controller;

/// \brief MAC-FAPI bidirectional sector fastpath adaptor interface.
///
/// This adaptor represents a sector in FAPI, containing the operation controller of the sector, P5
/// (control/configuration) and P7 (user/slot) FAPI data planes.
class mac_fapi_sector_fastpath_adaptor
{
public:
  virtual ~mac_fapi_sector_fastpath_adaptor() = default;

  /// Returns the operation controller of this sector adaptor.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the P7 adaptor of this sector fastpath adaptor.
  virtual mac_fapi_p7_sector_fastpath_adaptor& get_p7_sector_adaptor() = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
