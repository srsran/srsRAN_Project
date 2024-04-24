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

#include "apps/units/application_unit.h"
#include "dynamic_du_unit_config.h"

namespace srsran {

/// \brief Dynamic DU application unit implementation.
///
/// A DU of type Dynamic adds support to all the types of Radio Units.
class dynamic_du_application_unit_impl : public application_unit
{
public:
  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  bool on_configuration_validation() const override;

  // See interface for documentation.
  void on_loggers_registration() override;

private:
  dynamic_du_unit_config unit_cfg;
};

} // namespace srsran
