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

#include "srsran/fapi_adaptor/fapi_adaptor.h"
#include <memory>

namespace CLI {
class App;
} // namespace CLI

namespace srsran {

/// \brief Split 6 plugin interface.
///
/// The plugin interface allows different implementations of the split 6.
class split6_plugin
{
public:
  virtual ~split6_plugin() = default;

  /// Registers the parsing configuration properties that will be used by this application unit.
  virtual void on_parsing_configuration_registration(CLI::App& app) = 0;

  /// Validates the configuration of this application unit. Returns true on success, otherwise false.
  virtual bool on_configuration_validation() const = 0;

  /// Registers the loggers of this application unit.
  virtual void on_loggers_registration() = 0;

  /// Creates and returns a vector of fapi adaptors, each of them representing a cell.
  virtual std::vector<std::unique_ptr<fapi::fapi_adaptor>>
  create_fapi_adaptor(const du_unit_dependencies& dependencies) = 0;
};

} // namespace srsran
