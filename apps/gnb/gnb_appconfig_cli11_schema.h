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

#include "CLI/CLI11.hpp"

namespace srsran {

struct gnb_appconfig;
struct cu_cp_unit_config;
struct du_high_unit_config;

/// Configures the given CLI11 application with the gNB application configuration schema.
void configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_appconfig& gnb_parsed_cfg);

/// Auto derive gNB parameters after the parsing.
void autoderive_gnb_parameters_after_parsing(CLI::App& app, gnb_appconfig& parsed_cfg);

/// Auto derive the supported TAs for the CU-CP AMF config from the DU cells config.
void autoderive_supported_tas_for_amf_from_du_cells(const du_high_unit_config& du_hi_cfg, cu_cp_unit_config& cu_cp_cfg);

} // namespace srsran
