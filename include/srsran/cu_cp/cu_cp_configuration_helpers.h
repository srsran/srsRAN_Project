/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_configuration.h"

namespace srsran::config_helpers {

/// Generates default QoS configuration used by gNB CU-CP. The default configuration should be valid.
/// The default values are picked to try to acheive the QoS requirements defined in
/// TS 23.501 -- System architecture for the 5G System, table 5.7.4-1.
///
/// Dependencies between timers should be considered:
///   * discardTimer: How long the PDCP will wait for a transmist/acknoledgment notification,
///                   before requesting a discard to the RLC. Should not exceed the packet
///                   delay budget, if the 5QI is delay sensitive.
///   * t-Reordering: How long the PDCP will wait for an out-of-order PDU. When using RLC UM,
///                   this value should be larger than the RLC's t-Reassembly. When using AM,
///                   this value should be larger than a few RLC retransmissions, see the RLC
///                   timers for details.
inline std::map<five_qi_t, srs_cu_cp::cu_cp_qos_config> make_default_cu_cp_qos_config_list()
{
  std::map<five_qi_t, srs_cu_cp::cu_cp_qos_config> qos_list = {};
  //
  // Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=1 e.g. Conversational Voice
    // PDB=100ms PER=10^-2
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms100;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(1)] = cfg;
  }
  {
    // 5QI=2 e.g. conversational video
    // PDB=150ms PER=10^-3
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms150;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(2)] = cfg;
  }
  {
    // 5QI=3 e.g. real time gaming
    // PDB=50ms PER=10^-3
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms50;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(3)] = cfg;
  }
  {
    // 5QI = 4 e.g. non-conversational video
    // PDB = 300ms PER = 10^-6
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms300;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms140;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(4)] = cfg;
  }
  {
    // 5QI = 65 e.g. Mission Critical user plane Push To Talk
    // PDB = 75ms PER = 10^-2
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms75;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(65)] = cfg;
  }
  {
    // 5QI = 66 e.g. Mission Critical user plane Push To Talk
    // PDB = 100ms PER = 10^-2
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms100;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(66)] = cfg;
  }
  {
    // 5QI = 67 e.g. Mission Critical user plane Push To Talk
    // PDB = 100ms PER = 10^-3
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms100;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(67)] = cfg;
  }
  //
  // Non-Guaranteed Bitrate 5QIs
  //
  {
    // 5QI = 5 e.g IMS signaling
    // PDB = 100ms PER = 10^-6
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms100;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms500;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(5)] = cfg;
  }
  {
    // 5QI = 6 e.g Video (Buffered Streaming)
    // PDB = 300ms PER = 10^-6
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms300;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms100;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(5)] = cfg;
  }
  {
    // 5QI = 7 e.g Voice,Video (Live Streaming)
    // PDB = 100ms PER = 10^-3
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
    // 5QI = 8 e.g Video (Buffered Streaming)
    // PDB = 300ms PER = 10^-6
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms300;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms100;

    cfg.pdcp                     = pdcp_cfg;
    qos_list[uint_to_five_qi(9)] = cfg;
  }
  {
    // 5QI = 9 e.g Video (Buffered Streaming)
    // PDB = 300ms PER = 10^-6
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
  {
    // 5QI = 10 e.g Video (Buffered Streaming)
    // PDB = 1100ms PER = 10^-6
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms300; // fix
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms500;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(10)] = cfg;
  }
  {
    // 5QI = 69 e.g Mission Critical delay sensitive signalling
    // PDB = 60ms PER = 10^-6
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms60;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(69)] = cfg;
  }
  {
    // 5QI = 70 e.g Mission Critical Data
    // PDB = 200ms PER = 10^-6
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::am;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size18bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms200;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size18bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms100;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(70)] = cfg;
  }
  return qos_list;
}

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_cu_cp::cu_cp_configuration make_default_cu_cp_config()
{
  srs_cu_cp::cu_cp_configuration cfg{};
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

inline std::vector<plmn_identity>
get_supported_plmns(const std::vector<srs_cu_cp::cu_cp_configuration::ngap_params>& ngaps)
{
  std::vector<plmn_identity> plmns;

  for (const auto& ngap : ngaps) {
    for (const auto& ta : ngap.supported_tas) {
      for (const auto& plmn_item : ta.plmn_list) {
        plmns.push_back(plmn_item.plmn_id);
      }
    }
  }

  return plmns;
}

} // namespace srsran::config_helpers
