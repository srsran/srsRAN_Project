/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_cp_configuration.h"

namespace srsran::config_helpers {

/// Generates default QoS configuration used by gNB CU-CP. The default configuration should be valid.
/// The default values are picked to try to acheive the QoS requirements defined in
/// TS 23.501 -- System architecture for the 5G System, table 5.7.4-1.
///
/// Dependencies between timers should be considered:
///   * discardTimer: How long the PDCP will wait for a transmit/acknowledgement notification,
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
    // 5QI = 66 e.g. Non-Mission-Critical user plane Push To Talk voice
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
    // 5QI = 67 e.g. Mission Critical Video user plane
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
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms50;

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
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms1500;
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
  //
  // Delay-critical Guaranteed Bitrate 5QIs
  //
  {
    // 5QI = 82 e.g Discrete Automation
    // PDB = 10ms PER = 10^-4
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms10;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(82)] = cfg;
  }
  {
    // 5QI = 83 e.g Discrete Automation
    // PDB = 10ms PER = 10^-4
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms10;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(83)] = cfg;
  }
  {
    // 5QI = 84 e.g Intelligent transport systems
    // PDB = 30ms PER = 10^-5
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms30;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(84)] = cfg;
  }
  {
    // 5QI = 85 e.g Electricity Distribution-high voltage
    // PDB = 5ms PER = 10^-5
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms10;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(85)] = cfg;
  }
  {
    // 5QI = 86 e.g V2X messages
    // PDB = 5ms PER = 10^-4
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms10;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(86)] = cfg;
  }
  {
    // 5QI = 87 e.g Interactive Service-Motion tracking data
    // PDB = 5ms PER = 10^-3
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms10;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(87)] = cfg;
  }
  {
    // 5QI = 88 e.g Interactive Service-Motion tracking data
    // PDB = 10ms PER = 10^-3
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms10;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(88)] = cfg;
  }
  {
    // 5QI = 89 e.g Visual content for cloud/edge/split rendering
    // PDB = 15ms PER = 10^-4
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms20;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(89)] = cfg;
  }
  {
    // 5QI = 90 e.g Visual content for cloud/edge/split rendering
    // PDB = 20ms PER = 10^-4
    srs_cu_cp::cu_cp_qos_config cfg{};
    pdcp_config                 pdcp_cfg{};

    pdcp_cfg.rb_type                       = pdcp_rb_type::drb;
    pdcp_cfg.rlc_mode                      = pdcp_rlc_mode::um;
    pdcp_cfg.ciphering_required            = true;
    pdcp_cfg.integrity_protection_required = false;

    // > Tx
    pdcp_cfg.tx.sn_size                = pdcp_sn_size::size12bits;
    pdcp_cfg.tx.discard_timer          = pdcp_discard_timer::ms20;
    pdcp_cfg.tx.status_report_required = false;

    // > Rx
    pdcp_cfg.rx.sn_size               = pdcp_sn_size::size12bits;
    pdcp_cfg.rx.out_of_order_delivery = false;
    pdcp_cfg.rx.t_reordering          = pdcp_t_reordering::ms0;

    cfg.pdcp                      = pdcp_cfg;
    qos_list[uint_to_five_qi(90)] = cfg;
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
get_supported_plmns(const std::vector<srs_cu_cp::cu_cp_configuration::ngap_config>& ngaps)
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
