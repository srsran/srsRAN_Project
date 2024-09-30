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

#include "srsran/du/du_high/du_qos_config.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/ran/qos/five_qi.h"
#include <map>

namespace srsran {
namespace config_helpers {

/// Generates default QoS configuration used by gNB DU. The default configuration should be valid.
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
const uint32_t                                    default_rlc_queue_size_sdus  = 16384;
const uint32_t                                    default_rlc_queue_size_bytes = 4096 * (1500 + 7);
inline std::map<five_qi_t, srs_du::du_qos_config> make_default_du_qos_config_list(bool warn_on_drop,
                                                                                  int  rlc_metrics_report)
{
  std::map<five_qi_t, srs_du::du_qos_config> qos_list = {};
  {
    // 5QI=1
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
    // MAC
    cfg.mac          = make_default_drb_mac_lc_config();
    cfg.mac.priority = 4;
    cfg.mac.lcg_id   = uint_to_lcg_id(1);

    qos_list[uint_to_five_qi(1)] = cfg;
  }
  {
    // 5QI=2
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
    // MAC
    cfg.mac          = make_default_drb_mac_lc_config();
    cfg.mac.priority = 4;
    cfg.mac.lcg_id   = uint_to_lcg_id(1);

    qos_list[uint_to_five_qi(2)] = cfg;
  }
  {
    // 5QI=5
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size12bits;
    cfg.rlc.am.tx.t_poll_retx       = 80;
    cfg.rlc.am.tx.poll_pdu          = 64;
    cfg.rlc.am.tx.poll_byte         = 125;
    cfg.rlc.am.tx.max_retx_thresh   = 4;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = default_rlc_queue_size_sdus;
    cfg.rlc.am.tx.queue_size_bytes  = default_rlc_queue_size_bytes;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size12bits;
    cfg.rlc.am.rx.t_reassembly      = 80;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    // MAC
    cfg.mac = make_default_drb_mac_lc_config();

    qos_list[uint_to_five_qi(5)] = cfg;
  }
  {
    // 5QI=7
    srs_du::du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 100;
    cfg.rlc.um.tx.queue_size       = default_rlc_queue_size_sdus;
    cfg.rlc.um.tx.queue_size_bytes = default_rlc_queue_size_bytes;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = default_rlc_queue_size_bytes;
    cfg.f1u.warn_on_drop          = warn_on_drop;
    // MAC
    cfg.mac          = make_default_drb_mac_lc_config();
    cfg.mac.priority = 4;
    cfg.mac.lcg_id   = uint_to_lcg_id(1);

    qos_list[uint_to_five_qi(7)] = cfg;
  }
  {
    // 5QI=9
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
    // MAC
    cfg.mac = make_default_drb_mac_lc_config();

    qos_list[uint_to_five_qi(9)] = cfg;
  }
  return qos_list;
}

} // namespace config_helpers
} // namespace srsran
