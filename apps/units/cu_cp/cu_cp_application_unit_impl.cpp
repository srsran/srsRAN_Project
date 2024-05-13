/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cu_cp_application_unit_impl.h"
#include "cu_cp_logger_registrator.h"
#include "cu_cp_unit_config_cli11_schema.h"
#include "cu_cp_unit_config_validator.h"

using namespace srsran;

void cu_cp_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_cu_cp_unit_config_schema(app, unit_cfg);
}

bool cu_cp_application_unit_impl::on_configuration_validation() const
{
  return validate_cu_cp_unit_config(unit_cfg);
}

void cu_cp_application_unit_impl::on_loggers_registration()
{
  register_cu_cp_loggers(unit_cfg.loggers);
}
