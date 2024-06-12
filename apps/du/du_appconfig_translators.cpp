/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_appconfig_translators.h"
#include "du_appconfig.h"

using namespace srsran;
using namespace std::chrono_literals;

// TODO: refactor.
srsran::sctp_network_connector_config srsran::generate_e2ap_nw_config(const du_appconfig& config, int ppid)
{
  srsran::sctp_network_connector_config out_cfg;
  out_cfg.dest_name       = "NearRT-RIC";
  out_cfg.if_name         = "E2";
  out_cfg.connect_address = config.e2_cfg.ip_addr;
  out_cfg.connect_port    = config.e2_cfg.port;
  out_cfg.bind_address    = config.e2_cfg.bind_addr;
  out_cfg.ppid            = ppid;

  if (config.e2_cfg.sctp_rto_initial >= 0) {
    out_cfg.rto_initial = config.e2_cfg.sctp_rto_initial;
  }
  if (config.e2_cfg.sctp_rto_min >= 0) {
    out_cfg.rto_min = config.e2_cfg.sctp_rto_min;
  }
  if (config.e2_cfg.sctp_rto_max >= 0) {
    out_cfg.rto_max = config.e2_cfg.sctp_rto_max;
  }
  if (config.e2_cfg.sctp_init_max_attempts >= 0) {
    out_cfg.init_max_attempts = config.e2_cfg.sctp_init_max_attempts;
  }
  if (config.e2_cfg.sctp_max_init_timeo >= 0) {
    out_cfg.max_init_timeo = config.e2_cfg.sctp_max_init_timeo;
  }

  return out_cfg;
}
