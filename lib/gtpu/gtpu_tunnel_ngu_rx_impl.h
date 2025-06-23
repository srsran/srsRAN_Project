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

#include "gtpu_tunnel_base_rx.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_ngu_rx.h"
#include "srsran/psup/psup_packing.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/sdu_window.h"
#include "srsran/support/timers.h"

namespace srsran {

constexpr unsigned gtpu_sn_mod         = 65536;
constexpr unsigned gtpu_rx_window_size = 32768;

/// GTP-U RX state variables
struct gtpu_rx_state {
  /// RX_NEXT indicates the SN value of the next GTP-U SDU expected to be received.
  uint16_t rx_next;
  /// RX_DELIV indicates the SN value of the first GTP-U SDU not delivered to the lower layers, but still
  /// waited for.
  uint16_t rx_deliv;
  /// RX_REORD indicates the SN value following the SN value associated with the GTP-U PDU which
  /// triggered t-Reordering.
  uint16_t rx_reord;
};

struct gtpu_rx_sdu_info {
  byte_buffer             sdu         = {};
  qos_flow_id_t           qos_flow_id = qos_flow_id_t::invalid;
  std::optional<uint16_t> sn          = {};
};

/// Class used for receiving GTP-U NGU bearers, e.g. on N3 interface.
class gtpu_tunnel_ngu_rx_impl : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_ngu_rx_impl(srs_cu_up::ue_index_t                             ue_index,
                          gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_rx_config cfg,
                          gtpu_tunnel_ngu_rx_lower_layer_notifier&          rx_lower_,
                          timer_factory                                     ue_ctrl_timer_factory_) :
    gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix{ue_index, cfg.local_teid, "DL"}, cfg.test_mode),
    psup_packer(logger.get_basic_logger()),
    lower_dn(rx_lower_),
    config(cfg),
    rx_window(logger, gtpu_rx_window_size),
    ue_ctrl_timer_factory(ue_ctrl_timer_factory_)
  {
    srsran_assert(cfg.ue_ambr_limiter != nullptr, "No UE-AMBR limiter provided");
    if (config.t_reordering.count() != 0) {
      reordering_timer = ue_ctrl_timer_factory.create_timer();
      reordering_timer.set(config.t_reordering, reordering_callback{this});
    }
    logger.log_info("GTPU NGU Rx configured. {}", config);
  }
  ~gtpu_tunnel_ngu_rx_impl() override = default;

  void stop()
  {
    if (not stopped) {
      reordering_timer.stop();
      stopped = true;
    }
  }

  /*
   * Testing Helpers
   */
  void          set_state(gtpu_rx_state st_) { st = st_; }
  gtpu_rx_state get_state() { return st; }
  bool          is_reordering_timer_running() { return reordering_timer.is_running(); }

protected:
  // domain-specific PDU handler
  void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) final
  {
    if (stopped) {
      return;
    }

    // Limit UE to AMBR.
    if (not config.ignore_ue_ambr && not config.ue_ambr_limiter->consume(pdu.buf.length())) {
      if (not config.warn_on_drop) {
        logger.log_info("Dropped GTPU PDU. UE went over UE-AMBR");
      } else {
        logger.log_warning("Dropped GTPU PDU. UE went over UE-AMBR");
      }
      return;
    }

    if (pdu.test_mode) {
      gtpu_rx_sdu_info rx_sdu_info;
      rx_sdu_info.sdu         = std::move(pdu.buf);
      rx_sdu_info.qos_flow_id = qos_flow_id_t{0x01}; // QoS Flow ID for test DRB.
      deliver_sdu(rx_sdu_info);
      return;
    }

    size_t                          pdu_len               = pdu.buf.length();
    gtpu_teid_t                     teid                  = pdu.hdr.teid;
    psup_dl_pdu_session_information pdu_session_info      = {};
    bool                            have_pdu_session_info = false;
    for (auto ext_hdr : pdu.hdr.ext_list) {
      switch (ext_hdr.extension_header_type) {
        case gtpu_extension_header_type::pdu_session_container:
          if (!have_pdu_session_info) {
            have_pdu_session_info = psup_packer.unpack(pdu_session_info, ext_hdr.container);
            if (!have_pdu_session_info) {
              logger.log_error("Failed to unpack PDU session container. pdu_len={}", pdu_len);
            }
          } else {
            logger.log_warning("Ignoring multiple PDU session container. pdu_len={}", pdu_len);
          }
          break;
        default:
          logger.log_warning("Ignoring unexpected extension header at NG-U interface. type={} pdu_len={}",
                             ext_hdr.extension_header_type,
                             pdu_len);
      }
    }
    if (!have_pdu_session_info) {
      logger.log_warning(
          "Incomplete PDU at NG-U interface: missing or invalid PDU session container. pdu_len={} teid={}",
          pdu_len,
          teid);
      // As per TS 29.281 Sec. 5.2.2.7 the (...) PDU Session Container (...) shall be transmitted in a G-PDU over the
      // N3 and N9 user plane interfaces (...).
      return;
    }

    logger.log_debug(pdu.buf.begin(), pdu.buf.end(), "RX PDU. pdu_len={} {}", pdu_len, st);

    if (!pdu.hdr.flags.seq_number || config.t_reordering.count() == 0) {
      // Forward this SDU straight away.
      byte_buffer      rx_sdu      = gtpu_extract_msg(std::move(pdu)); // header is invalidated after extraction
      gtpu_rx_sdu_info rx_sdu_info = {std::move(rx_sdu), pdu_session_info.qos_flow_id};
      deliver_sdu(rx_sdu_info);
      return;
    }

    uint16_t    sn     = pdu.hdr.seq_number;
    byte_buffer rx_sdu = gtpu_extract_msg(std::move(pdu)); // header is invalidated after extraction

    // Check out-of-window
    if (!inside_rx_window(sn)) {
      if (nof_log_sn_out_of_window++ < max_nof_log_sn_out_of_window) {
        logger.log_warning("SN falls out of Rx window. sn={} pdu_len={} {} reordering_timer_running={}",
                           sn,
                           pdu_len,
                           st,
                           reordering_timer.is_running());
        if (nof_log_sn_out_of_window == max_nof_log_sn_out_of_window) {
          logger.log_warning("Throttling previous log message after {} contiguous repetitions",
                             nof_log_sn_out_of_window);
        }
      }
      gtpu_rx_sdu_info rx_sdu_info = {std::move(rx_sdu), pdu_session_info.qos_flow_id, sn};
      deliver_sdu(rx_sdu_info);
      return;
    }

    // Check late SN
    if (rx_mod_base(sn) < rx_mod_base(st.rx_deliv)) {
      logger.log_debug("Out-of-order after timeout or duplicate. sn={} pdu_len={} {}", sn, pdu_len, st);
      gtpu_rx_sdu_info rx_sdu_info = {std::move(rx_sdu), pdu_session_info.qos_flow_id, sn};
      deliver_sdu(rx_sdu_info);
      return;
    }

    // Check if PDU has been received
    if (rx_window.has_sn(sn)) {
      logger.log_warning("Duplicate PDU dropped. sn={} pdu_len={}", sn, pdu_len);
      return;
    }

    gtpu_rx_sdu_info& rx_sdu_info = rx_window.add_sn(sn);
    rx_sdu_info.sdu               = std::move(rx_sdu);
    rx_sdu_info.qos_flow_id       = pdu_session_info.qos_flow_id;
    rx_sdu_info.sn                = sn;

    // Update RX_NEXT
    if (rx_mod_base(sn) >= rx_mod_base(st.rx_next)) {
      st.rx_next = sn + 1;
    }

    if (rx_mod_base(sn) == rx_mod_base(st.rx_deliv)) {
      // Deliver all consecutive SDUs in ascending order of associated SN
      deliver_all_consecutive_sdus();
    }

    // Stop re-ordering timer if we advanced the window past RX_REORD
    if (reordering_timer.is_running() and (not inside_rx_window(st.rx_reord))) {
      reordering_timer.stop();
      logger.log_debug("Stopped t-Reordering. {}", st);
    }

    if (config.t_reordering.count() == 0) {
      st.rx_reord = st.rx_next;
      handle_t_reordering_expire();
    } else if (!reordering_timer.is_running() and rx_mod_base(st.rx_deliv) < rx_mod_base(st.rx_next)) {
      st.rx_reord = st.rx_next;
      reordering_timer.run();
      logger.log_debug("Started t-Reordering. {}", st);
    }

    // Reset throttled logs
    nof_log_sn_out_of_window = 0;
  }

  void deliver_sdu(gtpu_rx_sdu_info& sdu_info)
  {
    logger.log_info(sdu_info.sdu.begin(),
                    sdu_info.sdu.end(),
                    "RX SDU. sdu_len={} qos_flow={} sn={}",
                    sdu_info.sdu.length(),
                    sdu_info.qos_flow_id,
                    sdu_info.sn);
    lower_dn.on_new_sdu(std::move(sdu_info.sdu), sdu_info.qos_flow_id);
  }

  void deliver_all_consecutive_sdus()
  {
    while (st.rx_deliv != st.rx_next && rx_window.has_sn(st.rx_deliv)) {
      gtpu_rx_sdu_info& sdu_info = rx_window[st.rx_deliv];
      deliver_sdu(sdu_info);
      rx_window.remove_sn(st.rx_deliv);

      // Update RX_DELIV
      st.rx_deliv = st.rx_deliv + 1;
    }
  }

  void handle_t_reordering_expire()
  {
    // Check if timer has been restarted by the PDU handling routine between expiration and execution of this handler.
    if (reordering_timer.is_running()) {
      logger.log_info("reordering timer has been already restarted. Skipping outdated event. {}", st);
      return;
    }
    if (not inside_rx_window(st.rx_reord)) {
      logger.log_info("rx_reord is outside RX window. Skipping outdated event. {}", st);
      return;
    }

    while (st.rx_deliv != st.rx_reord) {
      if (rx_window.has_sn(st.rx_deliv)) {
        gtpu_rx_sdu_info& sdu_info = rx_window[st.rx_deliv];
        deliver_sdu(sdu_info);
        rx_window.remove_sn(st.rx_deliv);
      }

      // Update RX_DELIV
      st.rx_deliv = st.rx_deliv + 1;
    }

    deliver_all_consecutive_sdus();

    if (rx_mod_base(st.rx_deliv) < rx_mod_base(st.rx_next)) {
      if (config.t_reordering.count() == 0) {
        logger.log_error("reordering timer expired after 0ms and rx_deliv < rx_next. {}", st);
        return;
      }
      logger.log_debug("updating rx_reord to rx_next. {}", st);
      st.rx_reord = st.rx_next;
      reordering_timer.run();
    }
  }

private:
  psup_packing                             psup_packer;
  gtpu_tunnel_ngu_rx_lower_layer_notifier& lower_dn;
  bool                                     stopped = false;

  /// Rx config
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_rx_config config;

  /// Rx state
  gtpu_rx_state st = {};

  /// Rx window
  sdu_window<gtpu_rx_sdu_info, gtpu_tunnel_logger> rx_window;

  /// Rx reordering timer
  unique_timer reordering_timer;

  /// Timer factory
  timer_factory ue_ctrl_timer_factory;

  /// Reordering callback (t-Reordering)
  class reordering_callback
  {
  public:
    explicit reordering_callback(gtpu_tunnel_ngu_rx_impl* parent_) : parent(parent_) {}
    void operator()(timer_id_t timer_id)
    {
      if (not parent->config.warn_on_drop) {
        parent->logger.log_info(
            "reordering timer expired after {}ms. {}", parent->config.t_reordering.count(), parent->st);
      } else {
        parent->logger.log_warning(
            "reordering timer expired after {}ms. {}", parent->config.t_reordering.count(), parent->st);
      }
      parent->handle_t_reordering_expire();
    }

  private:
    gtpu_tunnel_ngu_rx_impl* parent;
  };

  /// \brief Helper function for arithmetic comparisons of state variables or SN values
  ///
  /// When performing arithmetic comparisons of state variables or SN values, a modulus base shall be used.
  /// This is adapted from RLC AM, TS 38.322 Sec. 7.1
  ///
  /// \param sn The sequence number to be rebased from RX_Deliv, as this is the lower-edge of the window.
  /// \return The rebased value of sn
  constexpr uint16_t rx_mod_base(uint16_t sn) const { return (sn - st.rx_deliv) % gtpu_sn_mod; }

  /// Checks whether a sequence number is inside the current Rx window
  /// \param sn The sequence number to be checked
  /// \return True if sn is inside the Rx window, false otherwise
  constexpr bool inside_rx_window(uint16_t sn) const
  {
    // RX_Deliv <= SN < RX_Deliv + Window_Size
    return rx_mod_base(sn) < gtpu_rx_window_size;
  }

  // Log helper for throttling
  static constexpr unsigned max_nof_log_sn_out_of_window = 5;
  unsigned                  nof_log_sn_out_of_window     = 0;
};

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::gtpu_rx_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_rx_state& st, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "rx_deliv={} rx_reord={} rx_next={} ", st.rx_deliv, st.rx_reord, st.rx_next);
  }
};

} // namespace fmt
