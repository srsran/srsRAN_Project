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

#include "e2_appconfig_translators.h"
#include "e2_appconfig.h"

using namespace srsran;

sctp_network_connector_config srsran::generate_e2ap_nw_config(const e2_appconfig& config, int ppid)
{
  sctp_network_connector_config out_cfg;
  out_cfg.dest_name       = "NearRT-RIC";
  out_cfg.if_name         = "E2";
  out_cfg.connect_address = config.ip_addr;
  out_cfg.connect_port    = config.port;
  out_cfg.bind_address    = config.bind_addr;
  out_cfg.ppid            = ppid;

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

  return out_cfg;
}
