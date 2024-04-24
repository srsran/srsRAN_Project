/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dynamic_du_application_unit_impl.h"
#include "dynamic_du_unit_cli11_schema.h"
#include "dynamic_du_unit_logger_registrator.h"

using namespace srsran;

void dynamic_du_application_unit_impl::on_loggers_registration()
{
  register_dynamic_du_loggers(unit_cfg);
}

bool dynamic_du_application_unit_impl::on_configuration_validation() const
{
  return false;
}

void dynamic_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_dynamic_du_unit_config_schema(app, unit_cfg);
}
