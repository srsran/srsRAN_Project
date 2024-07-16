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

#pragma once

#include "cu_cp_configuration.h"

namespace srsran {
namespace config_helpers {

/// Generates default QoS configuration used by gNB CU-CP. The default configuration should be valid.
/// Dependencies between timers should be considered:
///   * t-Reordering: How long the PDCP will wait for an out-of-order PDU. When using RLC UM,
///                   this value should be larger than the RLC's t-Reassembly. When using AM,
///                   this value should be larger than a few RLC retransmissions, see the RLC
///                   timers for details.
inline std::map<five_qi_t, srs_cu_cp::cu_cp_qos_config> make_default_cu_cp_qos_config_list()
{
  std::map<five_qi_t, srs_cu_cp::cu_cp_qos_config> qos_list = {};
  {
    // 5QI=1
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::infinity;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms80;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(1)] = cfg;
  }
  {
    // 5QI=2
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::infinity;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms80;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(2)] = cfg;
  }
  {
    // 5QI=5
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::infinity;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms80;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(5)] = cfg;
  }
  {
    // 5QI=7
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::infinity;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms220;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(7)] = cfg;
  }
  {
    // 5QI=9
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::infinity;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms220;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(9)] = cfg;
  }
  return qos_list;
}

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_cu_cp::cu_cp_configuration make_default_cu_cp_config()
{
  srs_cu_cp::cu_cp_configuration cfg{};
  // Slices
  s_nssai_t slice_cfg;
  slice_cfg.sst = 1;
  cfg.node.supported_slices.push_back(slice_cfg);
  // DRBs
  cfg.bearers.drb_config = config_helpers::make_default_cu_cp_qos_config_list();
  // Security.
  cfg.security.int_algo_pref_list = {security::integrity_algorithm::nia2,
                                     security::integrity_algorithm::nia1,
                                     security::integrity_algorithm::nia3,
                                     security::integrity_algorithm::nia0};
  cfg.security.enc_algo_pref_list = {security::ciphering_algorithm::nea0,
                                     security::ciphering_algorithm::nea2,
                                     security::ciphering_algorithm::nea1,
                                     security::ciphering_algorithm::nea3};
  return cfg;
}

inline bool is_valid_configuration(const srs_cu_cp::mobility_configuration& config)
{
  if (!is_valid_configuration(config.meas_manager_config)) {
    fmt::print("Invalid meas manager configuration.\n");
    return false;
  }

  return true;
}

/// Returns true if the given CU-CP configuration is valid, otherwise false.
inline bool is_valid_configuration(const srs_cu_cp::cu_cp_configuration& config)
{
  // Notifiers aren't checked here.
  if (config.node.ran_node_name.empty()) {
    fmt::print("RAN node name is empty\n");
    return false;
  }

  if (!is_valid_configuration(config.mobility)) {
    fmt::print("Invalid mobility configuration.\n");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsran
