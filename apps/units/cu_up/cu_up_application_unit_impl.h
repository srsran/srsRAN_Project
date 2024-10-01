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

#include "apps/units/cu_up/cu_up_application_unit.h"
#include "cu_up_unit_config.h"

namespace srsran {

/// CU-UP application unit implementation.
class cu_up_application_unit_impl : public cu_up_application_unit
{
public:
  explicit cu_up_application_unit_impl(std::string_view app_name);

  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  void on_configuration_parameters_autoderivation(CLI::App& app) override {}

  // See interface for documentation.
  bool on_configuration_validation(const os_sched_affinity_bitmask& available_cpus) const override;

  // See interface for documentation.
  void on_loggers_registration() override;

  // See interface for documentation.
  std::unique_ptr<srs_cu_up::cu_up_interface> create_cu_up_unit(const cu_up_unit_dependencies& dependencies) override;

  // See interface for documentation.
  cu_up_unit_config&       get_cu_up_unit_config() override { return unit_cfg; }
  const cu_up_unit_config& get_cu_up_unit_config() const override { return unit_cfg; }

  // See interface for documentation.
  void dump_config(YAML::Node& node) const override;

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override;

private:
  cu_up_unit_config unit_cfg;
};

} // namespace srsran
