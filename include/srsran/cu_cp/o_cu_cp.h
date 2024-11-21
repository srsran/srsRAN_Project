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
namespace srs_cu_cp {

class cu_cp;
class cu_cp_power_controller;

/// O-RAN CU-CP interface.
class o_cu_cp
{
public:
  virtual ~o_cu_cp() = default;

  /// Returns the CU-CP of this O-RAN CU-CP.
  virtual cu_cp& get_cu_cp() = 0;

  /// Returns the power controller of this O-RAN CU-CP.
  virtual cu_cp_power_controller& get_power_controller() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
