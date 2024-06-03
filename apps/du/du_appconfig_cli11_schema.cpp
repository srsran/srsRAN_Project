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

static void configure_cli11_f1c_args(CLI::App& app, srs_du::f1c_appconfig& f1c_params)
{
  app.add_option("--cu_cp_addr", f1c_params.cu_cp_address, "CU-CP F1-C address to connect to")->capture_default_str();
  app.add_option(
         "--bind_addr", f1c_params.bind_address, "DU F1-C bind address. If left empty, implicit bind is performed")
      ->capture_default_str();
}

static void configure_cli11_f1u_args(CLI::App& app, srs_du::f1u_appconfig& f1u_params)
{
  app.add_option("--queue_size", f1u_params.pdu_queue_size, "F1-U PDU queue size")->capture_default_str();
  app.add_option(
         "--bind_addr", f1u_params.bind_address, "DU F1-U bind address. If left empty, implicit bind is performed")
      ->capture_default_str();
}

void srsran::configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& parsed_cfg)
{
  // F1-C section.
  CLI::App* f1c_subcmd = app.add_subcommand("f1c", "F1-C interface configuration")->configurable();
  configure_cli11_f1c_args(*f1c_subcmd, parsed_cfg.f1c_cfg);

  // F1-U section.
  CLI::App* f1u_subcmd = app.add_subcommand("f1u", "F1-U interface configuration")->configurable();
  configure_cli11_f1u_args(*f1u_subcmd, parsed_cfg.f1u_cfg);

  // Logging section.
  // TODO

  // PCAP section.
  // TODO

  // Metrics section.
  // TODO

  // E2 section.
  // TODO

  // Buffer pool section.
  // TODO

  // Expert section.
  // TODO

  // HAL section.
  // TODO
}

void srsran::autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& parsed_cfg) {}
