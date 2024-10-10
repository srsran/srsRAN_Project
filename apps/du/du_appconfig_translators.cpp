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
#include "apps/services/worker_manager_config.h"
#include "du_appconfig.h"

using namespace srsran;
using namespace std::chrono_literals;

// TODO: refactor.
srsran::sctp_network_connector_config srsran::generate_e2ap_nw_config(const e2_appconfig& config, int ppid)
{
  srsran::sctp_network_connector_config out_cfg;
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

void srsran::fill_du_worker_manager_config(worker_manager_config& config, const du_appconfig& unit_cfg)
{
  srsran_assert(config.du_hi_cfg, "DU high worker config does not exist");

  config.du_hi_cfg->pdu_queue_size = unit_cfg.nru_cfg.pdu_queue_size;
  config.nof_low_prio_threads      = unit_cfg.expert_execution_cfg.threads.non_rt_threads.nof_non_rt_threads;
  config.low_prio_sched_config     = unit_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg;
}
