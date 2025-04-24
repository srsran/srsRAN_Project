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

namespace YAML {
class Node;
}

namespace CLI {
class App;
} // namespace CLI

namespace srsran {

struct worker_manager_config;
struct os_sched_affinity_bitmask;

/// \brief Application unit interface.
///
/// An application unit object is the most basic building block used by an application. Possible implementations of this
/// object can be a CU or DU. The application must register the unit capabilities in the given services, so it can use
/// them. An application is composed of one or more units.
class application_unit
{
public:
  virtual ~application_unit() = default;

  /// Registers the parsing configuration properties that will be used by this application unit.
  virtual void on_parsing_configuration_registration(CLI::App& app) = 0;

  /// Auto derive configuration parameters.
  virtual void on_configuration_parameters_autoderivation(CLI::App& app) = 0;

  /// Validates the configuration of this application unit. Returns true on success, otherwise false.
  virtual bool on_configuration_validation(const os_sched_affinity_bitmask& available_cpus) const = 0;

  /// Registers the loggers of this application unit.
  virtual void on_loggers_registration() = 0;

  /// Returns true if the metrics are enabled for this application unit, otherwise false.
  virtual bool are_metrics_enabled() const = 0;

  /// Dumps the application unit configuration into the given YAML node.
  virtual void dump_config(YAML::Node& node) const = 0;

  /// Fills the given worker manager configuration with the application unit parameters.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

} // namespace srsran
