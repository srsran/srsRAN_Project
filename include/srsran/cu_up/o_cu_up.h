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
namespace srs_cu_up {

class cu_up_interface;
class cu_up_power_controller;

/// ORAN CU-UP interface.
class o_cu_up
{
public:
  virtual ~o_cu_up() = default;

  /// Returns the CU-UP of this ORAN CU-UP.
  virtual cu_up_interface& get_cu_up() = 0;

  /// Returns the power controller of this ORAN CU-UP.
  virtual cu_up_power_controller& get_power_controller() = 0;
};

} // namespace srs_cu_up
} // namespace srsran