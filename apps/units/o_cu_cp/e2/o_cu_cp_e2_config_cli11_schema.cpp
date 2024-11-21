/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_cu_cp_e2_config_cli11_schema.h"
#include "apps/services/e2/e2_cli11_schema.h"
#include "o_cu_cp_e2_config.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_pcap_args(CLI::App& app, o_cu_cp_e2_pcap_config& pcap_params)
{
  add_option(app, "--e2ap_cu_cp_filename", pcap_params.filename, "E2AP PCAP file output path")->capture_default_str();
  add_option(app, "--e2ap_enable", pcap_params.enabled, "Enable E2AP packet capture")->always_capture_default();
}

void srsran::configure_cli11_with_o_cu_cp_e2_config_schema(CLI::App& app, o_cu_cp_e2_config& unit_cfg)
{
  // PCAP section.
  CLI::App* pcap_subcmd = add_subcommand(app, "pcap", "PCAP configuration")->configurable();
  configure_cli11_pcap_args(*pcap_subcmd, unit_cfg.pcaps);

  // E2 section.
  configure_cli11_with_e2_config_schema(app, unit_cfg.base_config, "--enable_cu_cp_e2", "Enable CU E2 agent");
}

void srsran::autoderive_o_cu_cp_e2_parameters_after_parsing(o_cu_cp_e2_config& unit_cfg)
{
  // If CU CP E2 agent is disabled do not enable e2ap pcap for it.
  unit_cfg.pcaps.enabled = unit_cfg.base_config.enable_unit_e2 && unit_cfg.pcaps.enabled;
}
