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

#include "srsran/du/du_high/du_qos_config.h"
#include "srsran/ran/qos/five_qi.h"
#include <map>

namespace srsran::config_helpers {

/// Generates default QoS configuration used by gNB DU. The default configuration should be valid.
/// The default values are picked to try to acheive the QoS requirements defined in
/// TS 23.501 -- System architecture for the 5G System, table 5.7.4-1.
///
/// Dependencies between RLC timers should be considered:
///   * t-Reassembly: How long it takes for the RLC to detect a lost PDU. If larger than the MAC SR, we may drop a
///                   PDU prematurely in the case UM, or we may send NACKs prematurely for the case of AM.
///
///   * t-StatusProhibit: This value dictates how often the RLC is allowed to send status reports. If this value is
///                       shorter than the MAC's SR, it may take longer than t-StatusProhibit to send a control PDU.
///
///   * t-PollRetransmission: This value should be slightly larger than t-StatusProhibit and also account for RTT.
///                           Moreover this value should be slightly larger than the SR of the MAC
///                           to avoid spurious RETX'es from late status reports. See t-StatusProhibit for details.
///
/// Note: These three timers will have implications in picking the PDCP's t-Reordering. See the generation of
///       t-Reordering default configuration for details.
///
/// Dependencies between F1-U timers should be considered:
///   * t-Notify: This value determines the maximum backoff time to aggregate notifications (towards CU_UP) of which
///               PDCP SDUs have been transmitted (RLC UM/AM) or delivered (RLC AM). Small values increase the number of
///               F1-U messages. Large values may trigger unnecessary discard notifications due to expiration of the
///               PDCP discard timer.

///  Default value for RLC SDU queue limit in bytes are chosen such that it allows for 4096 PDCP pdus of 1500 of payload
///  and 7 bytes of PDCP overhead. The SDU limit should be much larger then this, so that the limit is the number of
///  bytes in the queue, not the number of SDUs, even in the case of small PDUs
const uint32_t default_rlc_queue_size_sdus  = 16384;
const uint32_t default_rlc_queue_size_bytes = 4096 * (1500 + 7);

inline std::map<five_qi_t, srs_du::du_qos_config> make_default_du_qos_config_list(bool warn_on_drop,
                                                                                  int  rlc_metrics_report)
{
  std::map<five_qi_t, srs_du::du_qos_config> qos_list = {};
  //
  // Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=1 e.g. Conversational Voice
    // PDB=100ms PER=10^-2
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 50;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(1)] = cfg;
  }
  {
    // 5QI=2 e.g. conversational video
    // PDB=150ms PER=10^-3
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 50;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(2)] = cfg;
  }
  {
    // 5QI=3 e.g. real time gaming
    // PDB=50ms PER=10^-3
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 35;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(3)] = cfg;
  }
  {
    // 5QI = 4 e.g. non-conversational video
    // PDB = 300ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 80;
    cfg.rlc.am.tx.poll_pdu          = 64;
    cfg.rlc.am.tx.poll_byte         = 125;
    cfg.rlc.am.tx.max_retx_thresh   = 4;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 80;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(4)] = cfg;
  }
  {
    // 5QI = 65 e.g. Mission Critical user plane Push To Talk
    // PDB = 75ms PER = 10^-2
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 50;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(65)] = cfg;
  }
  {
    // 5QI = 66 e.g. Non-Mission-Critical user plane Push To Talk voice
    // PDB = 100ms PER = 10^-2
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 50;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(66)] = cfg;
  }
  {
    // 5QI = 67 e.g. Mission Critical Video user plane
    // PDB = 100ms PER = 10^-3
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 50;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(67)] = cfg;
  }
  {
    // 5QI = 5 e.g IMS signaling
    // PDB = 100ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 80;
    cfg.rlc.am.tx.poll_pdu          = 64;
    cfg.rlc.am.tx.poll_byte         = 125;
    cfg.rlc.am.tx.max_retx_thresh   = 4;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 80;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(5)] = cfg;
  }
  {
    // 5QI = 6 e.g Video (Buffered Streaming)
    // PDB = 300ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 80;
    cfg.rlc.am.tx.poll_pdu          = 64;
    cfg.rlc.am.tx.poll_byte         = 125;
    cfg.rlc.am.tx.max_retx_thresh   = 4;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 80;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(6)] = cfg;
  }
  {
    // 5QI = 7 e.g Voice,Video (Live Streaming)
    // PDB = 100ms PER = 10^-3
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 100;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    cfg.f1u.warn_on_drop          = warn_on_drop;

    qos_list[uint_to_five_qi(7)] = cfg;
  }
  {
    // 5QI = 8 e.g Video (Buffered Streaming)
    // PDB = 300ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 80;
    cfg.rlc.am.tx.poll_pdu          = 64;
    cfg.rlc.am.tx.poll_byte         = 125;
    cfg.rlc.am.tx.max_retx_thresh   = 4;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 80;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    cfg.f1u.warn_on_drop          = warn_on_drop;

    qos_list[uint_to_five_qi(8)] = cfg;
  }
  {
    // 5QI = 9 e.g Video (Buffered Streaming)
    // PDB = 300ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 100;
    cfg.rlc.am.tx.poll_pdu          = 16;
    cfg.rlc.am.tx.poll_byte         = -1;
    cfg.rlc.am.tx.max_retx_thresh   = 32;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 20;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    cfg.f1u.warn_on_drop          = warn_on_drop;

    qos_list[uint_to_five_qi(9)] = cfg;
  }
  {
    // 5QI = 10 e.g Video (Buffered Streaming)
    // PDB = 1100ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 20;
    cfg.rlc.am.tx.poll_pdu          = 16;
    cfg.rlc.am.tx.poll_byte         = -1;
    cfg.rlc.am.tx.max_retx_thresh   = 32;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 20;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    cfg.f1u.warn_on_drop          = warn_on_drop;

    qos_list[uint_to_five_qi(10)] = cfg;
  }
  {
    // 5QI = 69 e.g Mission Critical delay sensitive signalling
    // PDB = 60ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 20;
    cfg.rlc.am.tx.poll_pdu          = 16;
    cfg.rlc.am.tx.poll_byte         = -1;
    cfg.rlc.am.tx.max_retx_thresh   = 32;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 20;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    cfg.f1u.warn_on_drop          = warn_on_drop;

    qos_list[uint_to_five_qi(69)] = cfg;
  }
  {
    // 5QI = 70 e.g Mission Critical Data
    // PDB = 200ms PER = 10^-6
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 20;
    cfg.rlc.am.tx.poll_pdu          = 16;
    cfg.rlc.am.tx.poll_byte         = -1;
    cfg.rlc.am.tx.max_retx_thresh   = 32;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 20;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    cfg.f1u.warn_on_drop          = warn_on_drop;

    qos_list[uint_to_five_qi(70)] = cfg;
  }
  //
  // Delay-critical Guaranteed Bitrate 5QIs
  //
  {
    // 5QI = 82 e.g Discrete Automation
    // PDB = 10ms PER = 10^-4
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 10;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(82)] = cfg;
  }
  {
    // 5QI = 83 e.g Discrete Automation
    // PDB = 10ms PER = 10^-4
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 10;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(83)] = cfg;
  }
  {
    // 5QI = 84 e.g Intelligent transport systems
    // PDB = 30ms PER = 10^-5
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 30;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(84)] = cfg;
  }
  {
    // 5QI = 85 e.g Electricity Distribution-high voltage
    // PDB = 5ms PER = 10^-5
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 5;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(85)] = cfg;
  }
  {
    // 5QI = 86 e.g V2X messages
    // PDB = 5ms PER = 10^-4
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 5;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(86)] = cfg;
  }
  {
    // 5QI = 87 e.g Interactive Service-Motion tracking data
    // PDB = 5ms PER = 10^-3
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 5;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(87)] = cfg;
  }
  {
    // 5QI = 88 e.g Interactive Service-Motion tracking data
    // PDB = 10ms PER = 10^-3
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 5;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(88)] = cfg;
  }
  {
    // 5QI = 89 e.g Visual content for cloud/edge/split rendering
    // PDB = 15ms PER = 10^-4
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 15;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(89)] = cfg;
  }
  {
    // 5QI = 90 e.g Visual content for cloud/edge/split rendering
    // PDB = 20ms PER = 10^-4
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size6bits;
    cfg.rlc.um.rx.t_reassembly     = 20;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;

    qos_list[uint_to_five_qi(90)] = cfg;
  }
  return qos_list;
}

} // namespace srsran::config_helpers
