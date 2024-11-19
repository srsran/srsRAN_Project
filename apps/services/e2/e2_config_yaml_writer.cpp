/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_config_yaml_writer.h"
#include "srsran/e2/e2ap_configuration.h"

using namespace srsran;

void srsran::fill_e2_config_in_yaml_schema(YAML::Node node, const e2_config& config)
{
  node["addr"]                   = config.ip_addr;
  node["port"]                   = config.port;
  node["bind_addr"]              = config.bind_addr;
  node["sctp_rto_initial"]       = config.sctp_rto_initial;
  node["sctp_rto_min"]           = config.sctp_rto_min;
  node["sctp_rto_max"]           = config.sctp_rto_max;
  node["sctp_init_max_attempts"] = config.sctp_init_max_attempts;
  node["sctp_max_init_timeo"]    = config.sctp_max_init_timeo;
  node["e2sm_kpm_enabled"]       = config.e2sm_kpm_enabled;
  node["e2sm_rc_enabled"]        = config.e2sm_rc_enabled;
}
