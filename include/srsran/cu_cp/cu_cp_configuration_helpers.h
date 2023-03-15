/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// Generates default QoS configuration used by gNB CU-CP. The default configuration should be valid.
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
