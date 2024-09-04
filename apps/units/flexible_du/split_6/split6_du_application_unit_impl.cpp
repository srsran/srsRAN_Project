/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_du_application_unit_impl.h"
#include "split6_du_factory.h"
#include "split6_du_unit_cli11_schema.h"
#include "split6_du_unit_logger_registrator.h"

using namespace srsran;

void split6_du_application_unit_impl::on_loggers_registration()
{
  register_split6_du_loggers(unit_cfg);
  plugin->on_loggers_registration();
}

bool split6_du_application_unit_impl::on_configuration_validation() const
{
  if (!plugin->on_configuration_validation()) {
    return false;
  }

  return true;
}

void split6_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split6_du_unit_config_schema(app, unit_cfg);
  plugin->on_parsing_configuration_registration(app);
}

du_unit split6_du_application_unit_impl::create_unit(const du_unit_dependencies& dependencies)
{
  auto fapi_ctrl = plugin->create_fapi_adaptor(dependencies);
  report_error_if_not(!fapi_ctrl.empty(), "Could not create FAPI adaptor");
  auto du_impl = create_du_split6(unit_cfg, dependencies, std::move(fapi_ctrl));
  report_error_if_not(du_impl.unit, "Could not create split 6 DU");

  return du_impl;
}
