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

#include "ngap_configuration.h"

namespace srsran {
namespace config_helpers {

std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> make_default_ngap_qos_config_list();

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_cu_cp::ngap_configuration make_default_ngap_config()
{
  srs_cu_cp::ngap_configuration cfg{};
  cfg.gnb_id        = 411;
  cfg.ran_node_name = "srsgnb01";
  cfg.plmn          = "00101";
  cfg.tac           = 7;

  cfg.qos_config = make_default_ngap_qos_config_list();
  return cfg;
}

/// Generates default QoS configuration used by gNB DU. The default configuration should be valid.
inline std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> make_default_ngap_qos_config_list()
{
  std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> qos_list = {};
  {
    // 5QI=7
    srs_cu_cp::cu_cp_qos_config cfg{};
    // TX
    // > Common Config
    cfg.pdcp.tx.rb_type                       = pdcp_rb_type::drb;
    cfg.pdcp.tx.rlc_mode                      = pdcp_rlc_mode::um;
    cfg.pdcp.tx.sn_size                       = pdcp_sn_size::size18bits;
    cfg.pdcp.tx.direction                     = pdcp_security_direction::downlink;
    cfg.pdcp.tx.integrity_protection_required = false;
    cfg.pdcp.tx.ciphering_required            = false;
    // > TX specific
    cfg.pdcp.tx.discard_timer          = pdcp_discard_timer::not_configured;
    cfg.pdcp.tx.status_report_required = false;
    // RX
    // > Common Config
    cfg.pdcp.rx.rb_type                       = pdcp_rb_type::drb;
    cfg.pdcp.rx.rlc_mode                      = pdcp_rlc_mode::um;
    cfg.pdcp.rx.sn_size                       = pdcp_sn_size::size18bits;
    cfg.pdcp.rx.direction                     = pdcp_security_direction::downlink;
    cfg.pdcp.rx.integrity_protection_required = false;
    cfg.pdcp.rx.ciphering_required            = false;
    // > RX specific
    cfg.pdcp.rx.out_of_order_delivery = false;
    cfg.pdcp.rx.t_reordering          = pdcp_t_reordering::ms0;
    qos_list[7]                       = cfg;
  }
  {
    // 5QI=9 (temporary default to UM)
    srs_cu_cp::cu_cp_qos_config cfg{};
    // TX
    // > Common Config
    cfg.pdcp.tx.rb_type                       = pdcp_rb_type::drb;
    cfg.pdcp.tx.rlc_mode                      = pdcp_rlc_mode::um;
    cfg.pdcp.tx.sn_size                       = pdcp_sn_size::size18bits;
    cfg.pdcp.tx.direction                     = pdcp_security_direction::downlink;
    cfg.pdcp.tx.integrity_protection_required = false;
    cfg.pdcp.tx.ciphering_required            = false;
    // > TX specific
    cfg.pdcp.tx.discard_timer          = pdcp_discard_timer::not_configured;
    cfg.pdcp.tx.status_report_required = false;
    // RX
    // > Common Config
    cfg.pdcp.rx.rb_type                       = pdcp_rb_type::drb;
    cfg.pdcp.rx.rlc_mode                      = pdcp_rlc_mode::um;
    cfg.pdcp.rx.sn_size                       = pdcp_sn_size::size18bits;
    cfg.pdcp.rx.direction                     = pdcp_security_direction::downlink;
    cfg.pdcp.rx.integrity_protection_required = false;
    cfg.pdcp.rx.ciphering_required            = false;
    // > RX specific
    cfg.pdcp.rx.out_of_order_delivery = false;
    cfg.pdcp.rx.t_reordering          = pdcp_t_reordering::ms0;
    qos_list[9]                       = cfg;
  }
  return qos_list;
}

/// Returns true if the given CU-CP configuration is valid, otherwise false.
inline bool is_valid_configuration(const srs_cu_cp::ngap_configuration& config)
{
  if (config.ran_node_name.empty()) {
    fmt::print("RAN node name is empty");
    return false;
  }

  if (config.plmn.empty()) {
    fmt::print("PLMN id is empty");
    return false;
  }

  if (config.plmn.find("0x") != std::string::npos) {
    fmt::print("PLMN must not contain 0x");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsran
