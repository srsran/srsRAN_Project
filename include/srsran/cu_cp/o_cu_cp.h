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
namespace srs_cu_cp {

class cu_cp;
class cu_cp_operation_controller;

/// O-RAN CU-CP interface.
class o_cu_cp
{
public:
  virtual ~o_cu_cp() = default;

  /// Returns the CU-CP of this O-RAN CU-CP.
  virtual cu_cp& get_cu_cp() = 0;

  /// Returns the operation controller of this O-RAN CU-CP.
  virtual cu_cp_operation_controller& get_operation_controller() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
