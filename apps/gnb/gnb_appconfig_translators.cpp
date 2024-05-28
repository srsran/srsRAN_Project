/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_appconfig_translators.h"
#include "apps/units/cu_cp/cu_cp_unit_config.h"
#include "apps/units/cu_up/cu_up_unit_config.h"
#include "gnb_appconfig.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_up/cu_up_configuration_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/prach/prach_helper.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/rlc/rlc_srb_config_factory.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include <algorithm>
#include <map>

using namespace srsran;
using namespace std::chrono_literals;

srsran::sctp_network_connector_config srsran::generate_ngap_nw_config(const cu_cp_unit_amf_config& config)
{
  srsran::sctp_network_connector_config out_cfg;
  out_cfg.connection_name = "AMF";
  out_cfg.connect_address = config.ip_addr;
  out_cfg.connect_port    = config.port;
  if (config.n2_bind_addr == "auto") {
    out_cfg.bind_address = config.bind_addr;
  } else {
    out_cfg.bind_address = config.n2_bind_addr;
  }
  out_cfg.bind_interface = config.n2_bind_interface;
  out_cfg.ppid           = NGAP_PPID;

  if (config.sctp_rto_initial >= 0) {
    out_cfg.rto_initial = config.sctp_rto_initial;
  }
  if (config.sctp_rto_min >= 0) {
    out_cfg.rto_min = config.sctp_rto_min;
  }
  if (config.sctp_rto_max >= 0) {
    out_cfg.rto_max = config.sctp_rto_max;
  }
  if (config.sctp_init_max_attempts >= 0) {
    out_cfg.init_max_attempts = config.sctp_init_max_attempts;
  }
  if (config.sctp_max_init_timeo >= 0) {
    out_cfg.max_init_timeo = config.sctp_max_init_timeo;
  }
  out_cfg.nodelay = config.sctp_nodelay;

  return out_cfg;
}


srsran::sctp_network_connector_config srsran::generate_e2ap_nw_config(const gnb_appconfig& config, int ppid)
{
  srsran::sctp_network_connector_config out_cfg;
  out_cfg.connection_name = "NearRT-RIC";
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
