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

#include "split6_plugin.h"

namespace srsran {

struct worker_manager_config;

/// Dummy split 6 plugin implementation.
class split6_plugin_dummy : public split6_plugin
{
public:
  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override {}

  // See interface for documentation.
  bool on_configuration_validation() const override { return false; }

  // See interface for documentation.
  void on_loggers_registration() override {}

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override {}

  // See interface for documentation.
  std::vector<std::unique_ptr<fapi::fapi_adaptor>>
  create_fapi_adaptor(const srs_du::du_high_configuration& du_high_cfg,
                      const o_du_unit_dependencies&        dependencies) override;
};

} // namespace srsran
