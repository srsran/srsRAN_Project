/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_configuration.h"
#include "srsran/ngap/ngap_configuration_helpers.h"

namespace srsran {
namespace config_helpers {

/// Generates a map of 5QI to default QoS configurations.
std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> make_default_cu_cp_qos_config_list();

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_cu_cp::cu_cp_configuration make_default_cu_cp_config()
{
  srs_cu_cp::cu_cp_configuration cfg{};
  cfg.ngap_config = make_default_ngap_config();
  return cfg;
}

/// Generates default QoS configuration used by gNB DU. The default configuration should be valid.
inline std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> make_default_cu_cp_qos_config_list()
{
  std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> qos_list = {};
  {
    // 5QI=7
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config_t               pdcp_cfg{};
    pdcp_cfg.t_reordering = pdcp_t_reordering::ms0;

    drb_t drb_cfg                          = {};
    drb_cfg.pdcp_sn_size_ul                = pdcp_sn_size::size18bits;
    drb_cfg.pdcp_sn_size_dl                = pdcp_sn_size::size18bits;
    drb_cfg.integrity_protection_present   = false;
    drb_cfg.discard_timer                  = pdcp_discard_timer::infinity;
    drb_cfg.status_report_required_present = false;
    drb_cfg.out_of_order_delivery_present  = false;
    drb_cfg.rlc_mode                       = pdcp_rlc_mode::um;
    pdcp_cfg.drb                           = drb_cfg;

    cfg.pdcp    = pdcp_cfg;
    qos_list[7] = cfg;
  }
  {
    // 5QI=9 (temporary default to UM)
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config_t               pdcp_cfg{};
    pdcp_cfg.t_reordering = pdcp_t_reordering::ms0;

    drb_t drb_cfg                          = {};
    drb_cfg.pdcp_sn_size_ul                = pdcp_sn_size::size18bits;
    drb_cfg.pdcp_sn_size_dl                = pdcp_sn_size::size18bits;
    drb_cfg.integrity_protection_present   = false;
    drb_cfg.discard_timer                  = pdcp_discard_timer::infinity;
    drb_cfg.status_report_required_present = false;
    drb_cfg.out_of_order_delivery_present  = false;
    drb_cfg.rlc_mode                       = pdcp_rlc_mode::um;
    pdcp_cfg.drb                           = drb_cfg;

    cfg.pdcp    = pdcp_cfg;
    qos_list[9] = cfg;
  }
  return qos_list;
}

/// Returns true if the given CU-CP configuration is valid, otherwise false.
inline bool is_valid_configuration(const srs_cu_cp::cu_cp_configuration& config)
{
  // Notifiers aren't checked here.
  if (!is_valid_configuration(config.ngap_config)) {
    fmt::print("Invalid NGAP configuration.\n");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsran
