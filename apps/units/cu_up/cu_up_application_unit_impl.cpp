/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_application_unit_impl.h"
#include "cu_up_logger_registrator.h"
#include "cu_up_unit_config_cli11_schema.h"
#include "cu_up_unit_config_validator.h"

using namespace srsran;

void cu_up_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_cu_up_unit_config_schema(app, unit_cfg);
}

bool cu_up_application_unit_impl::on_configuration_validation() const
{
  return validate_cu_up_unit_config(unit_cfg);
}

void cu_up_application_unit_impl::on_loggers_registration()
{
  register_cu_up_loggers(unit_cfg.loggers);
}
