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

} // namespace srsran
