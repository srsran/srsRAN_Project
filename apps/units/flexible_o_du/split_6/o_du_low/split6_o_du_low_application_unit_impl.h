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

#include "apps/services/metrics/metrics_config.h"
#include "apps/units/application_unit.h"
#include "split6_o_du_low_plugin.h"
#include "split6_o_du_low_unit_config.h"
#include "srsran/du/du_operation_controller.h"
#include <memory>

namespace srsran {

class timer_manager;
struct worker_manager;

namespace app_services {
class metrics_notifier;
}

struct split6_o_du_low_unit {
  std::unique_ptr<du_operation_controller>  odu_low;
  std::vector<app_services::metrics_config> metrics;
};

/// O-RAN DU low Split6 application unit implementation.
class split6_o_du_low_application_unit_impl : public application_unit
{
public:
  split6_o_du_low_application_unit_impl(std::string_view app_name, std::unique_ptr<split6_o_du_low_plugin> plugin_);

  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  void on_configuration_parameters_autoderivation(CLI::App& app) override;

  // See interface for documentation.
  bool on_configuration_validation(const os_sched_affinity_bitmask& available_cpus) const;

  // See interface for documentation.
  void on_loggers_registration() override;

  // See interface for documentation.
  bool are_metrics_enabled() const override;

  // See interface for documentation.
  void dump_config(YAML::Node& node) const override;

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override;

  /// Creates and returns the low FAPI control adaptor of this application unit.
  split6_o_du_low_unit create_flexible_o_du_low(worker_manager&                 workers,
                                                app_services::metrics_notifier& metrics_notifier,
                                                timer_manager&                  timers,
                                                srslog::basic_logger&           logger);

private:
  split6_o_du_low_unit_config             unit_cfg;
  std::unique_ptr<split6_o_du_low_plugin> plugin;
};

std::unique_ptr<split6_o_du_low_application_unit_impl>
create_flexible_o_du_low_application_unit(std::string_view app_name);

} // namespace srsran
