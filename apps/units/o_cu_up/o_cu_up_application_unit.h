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

#include "apps/units/application_unit.h"
#include "apps/units/o_cu_up/o_cu_up_builder.h"
#include <yaml-cpp/node/node.h>

namespace srsran {

struct o_cu_up_unit_config;
struct worker_manager_config;

/// O-RAN CU-UP application unit interface.
class o_cu_up_application_unit : public application_unit
{
public:
  virtual ~o_cu_up_application_unit() = default;

  /// Creates an O-RAN CU-UP using the given dependencies.
  virtual o_cu_up_unit create_o_cu_up_unit(const o_cu_up_unit_dependencies& dependencies) = 0;

  /// Validates the configuration of this application unit. Returns true on success, otherwise false.
  virtual bool on_configuration_validation(bool tracing_enabled) const = 0;

  /// Returns the O-RAN CU-UP unit configuration of this O-RAN CU-UP application unit.
  virtual o_cu_up_unit_config&       get_o_cu_up_unit_config()       = 0;
  virtual const o_cu_up_unit_config& get_o_cu_up_unit_config() const = 0;
};

/// Creates an O-RAN CU-UP application unit.
std::unique_ptr<o_cu_up_application_unit> create_o_cu_up_application_unit(std::string_view app_name);

} // namespace srsran
