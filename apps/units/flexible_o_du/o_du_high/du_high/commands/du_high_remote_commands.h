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

#include "apps/services/remote_control/remote_command.h"
#include "srsran/du/du_high/du_manager/du_configurator.h"

namespace srsran {

/// Remote command that modifies the SSB parameters.
class ssb_modify_remote_command : public app_services::remote_command
{
  srs_du::du_configurator& configurator;

public:
  explicit ssb_modify_remote_command(srs_du::du_configurator& configurator_) : configurator(configurator_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "ssb_set"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Modifies the SSB parameters"; }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

/// Remote command that modifies the RRM policy ratio.
class rrm_policy_ratio_remote_command : public app_services::remote_command
{
  srs_du::du_configurator& configurator;

public:
  explicit rrm_policy_ratio_remote_command(srs_du::du_configurator& configurator_) : configurator(configurator_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "rrm_policy_ratio_set"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Modifies the RRM policy ratio"; }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

} // namespace srsran
