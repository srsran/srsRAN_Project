/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_high_e2_config_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "o_du_high_e2_config.h"
#include "srsran/e2/e2ap_configuration_helpers.h"

using namespace srsran;

e2ap_configuration srsran::generate_e2_config(const o_du_high_e2_config& config,
                                              gnb_id_t                   gnb_id,
                                              const std::string&         plmn,
                                              std::optional<gnb_du_id_t> gnb_du_id)
{
  e2ap_configuration out_cfg = config_helpers::make_default_e2ap_config();
  out_cfg.gnb_id             = gnb_id;
  out_cfg.plmn               = plmn;
  out_cfg.gnb_du_id          = gnb_du_id;
  out_cfg.e2sm_kpm_enabled   = config.base_cfg.e2sm_kpm_enabled;
  out_cfg.e2sm_rc_enabled    = config.base_cfg.e2sm_rc_enabled;

  return out_cfg;
}

void srsran::fill_o_du_high_e2_worker_manager_config(worker_manager_config& config, const o_du_high_e2_config& unit_cfg)
{
  auto& pcap_cfg = config.pcap_cfg;
  if (unit_cfg.pcaps.enabled) {
    pcap_cfg.is_e2ap_enabled = true;
  }
}
