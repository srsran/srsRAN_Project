/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_qos_config.h"
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
inline std::map<five_qi_t, du_qos_config> make_default_du_qos_config_list(bool warn_on_drop, int rlc_metrics_report)
{
  std::map<five_qi_t, du_qos_config> qos_list = {};
  {
    // 5QI=1
    du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 50;
    cfg.rlc.um.tx.queue_size       = 16384;
    cfg.rlc.um.tx.queue_size_bytes = 6172672;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = 6172672;
    // MAC
    cfg.mac          = make_default_drb_mac_lc_config();
    cfg.mac.priority = 4;
    cfg.mac.lcg_id   = uint_to_lcg_id(1);

    qos_list[uint_to_five_qi(1)] = cfg;
  }
  {
    // 5QI=2
    du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 50;
    cfg.rlc.um.tx.queue_size       = 16384;
    cfg.rlc.um.tx.queue_size_bytes = 6172672;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = 6172672;
    // MAC
    cfg.mac          = make_default_drb_mac_lc_config();
    cfg.mac.priority = 4;
    cfg.mac.lcg_id   = uint_to_lcg_id(1);

    qos_list[uint_to_five_qi(2)] = cfg;
  }
  {
    // 5QI=5
    du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size12bits;
    cfg.rlc.am.tx.t_poll_retx       = 80;
    cfg.rlc.am.tx.poll_pdu          = 64;
    cfg.rlc.am.tx.poll_byte         = 125;
    cfg.rlc.am.tx.max_retx_thresh   = 4;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = 16384;
    cfg.rlc.am.tx.queue_size_bytes  = 6172672;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size12bits;
    cfg.rlc.am.rx.t_reassembly      = 80;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = 6172672;
    // MAC
    cfg.mac = make_default_drb_mac_lc_config();

    qos_list[uint_to_five_qi(5)] = cfg;
  }
  {
    // 5QI=7
    du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                   = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length  = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly     = 100;
    cfg.rlc.um.tx.queue_size       = 16384;
    cfg.rlc.um.tx.queue_size_bytes = 6172672;
    cfg.rlc.metrics_period         = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = 6172672;
    cfg.f1u.warn_on_drop          = warn_on_drop;
    // MAC
    cfg.mac          = make_default_drb_mac_lc_config();
    cfg.mac.priority = 4;
    cfg.mac.lcg_id   = uint_to_lcg_id(1);

    qos_list[uint_to_five_qi(7)] = cfg;
  }
  {
    // 5QI=9
    du_qos_config cfg{};
    // RLC
    cfg.rlc.mode                    = rlc_mode::am;
    cfg.rlc.am.tx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.tx.t_poll_retx       = 20;
    cfg.rlc.am.tx.poll_pdu          = 16;
    cfg.rlc.am.tx.poll_byte         = -1;
    cfg.rlc.am.tx.max_retx_thresh   = 32;
    cfg.rlc.am.tx.max_window        = 0;
    cfg.rlc.am.tx.queue_size        = 16384;
    cfg.rlc.am.tx.queue_size_bytes  = 6172672;
    cfg.rlc.am.rx.sn_field_length   = rlc_am_sn_size::size18bits;
    cfg.rlc.am.rx.t_reassembly      = 20;
    cfg.rlc.am.rx.t_status_prohibit = 10;
    cfg.rlc.am.rx.max_sn_per_status = {};
    cfg.rlc.metrics_period          = std::chrono::milliseconds(rlc_metrics_report);
    // F1-U
    cfg.f1u.t_notify              = 10;
    cfg.f1u.rlc_queue_bytes_limit = 6172672;
    cfg.f1u.warn_on_drop          = warn_on_drop;
    // MAC
    cfg.mac = make_default_drb_mac_lc_config();

    qos_list[uint_to_five_qi(9)] = cfg;
  }
  return qos_list;
}

} // namespace config_helpers
} // namespace srsran
