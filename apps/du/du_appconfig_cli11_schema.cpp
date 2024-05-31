/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_appconfig_cli11_schema.h"
#include "du_appconfig.h"

using namespace srsran;

void srsran::configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& parsed_cfg) {}

void srsran::autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& parsed_cfg) {}
