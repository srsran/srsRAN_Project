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

#include "gtpu_sdu_window.h"
#include "gtpu_tunnel_base_rx.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/psup/psup_packing.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/timers.h"

namespace srsran {

constexpr unsigned gtpu_rx_window_size = 32768;

/// GTP-U RX state variables
struct gtpu_rx_state {
  /// RX_NEXT indicates the SN value of the next GTP-U SDU expected to be received.
  uint32_t rx_next;
  /// RX_DELIV indicates the SN value of the first GTP-U SDU not delivered to the lower layers, but still
  /// waited for.
  uint32_t rx_deliv;
  /// RX_REORD indicates the SN value following the SN value associated with the GTP-U PDU which
  /// triggered t-Reordering.
  uint32_t rx_reord;
};

/// Class used for receiving GTP-U bearers.
class gtpu_tunnel_ngu_rx : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_ngu_rx(srs_cu_up::ue_index_t                    ue_index,
                     gtpu_config::gtpu_rx_config              cfg,
                     gtpu_tunnel_ngu_rx_lower_layer_notifier& rx_lower_) :
    gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix{ue_index, cfg.local_teid, "DL"}),
    psup_packer(logger.get_basic_logger()),
    lower_dn(rx_lower_),
    rx_window(std::make_unique<gtpu_sdu_window<byte_buffer, gtpu_rx_window_size>>(logger))
  {
  }
  ~gtpu_tunnel_ngu_rx() = default;

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
    gtpu_teid_t                     teid                  = pdu.hdr.teid;
    psup_dl_pdu_session_information pdu_session_info      = {};
    bool                            have_pdu_session_info = false;
    for (auto ext_hdr : pdu.hdr.ext_list) {
      switch (ext_hdr.extension_header_type) {
        case gtpu_extension_header_type::pdu_session_container:
          if (!have_pdu_session_info) {
            have_pdu_session_info = psup_packer.unpack(pdu_session_info, ext_hdr.container);
            if (!have_pdu_session_info) {
              logger.log_error("Failed to unpack PDU session container.");
            }
          } else {
            logger.log_warning("Ignoring multiple PDU session container.");
          }
          break;
        default:
          logger.log_warning("Ignoring unexpected extension header at NG-U interface. type={}",
                             ext_hdr.extension_header_type);
      }
    }
    if (!have_pdu_session_info) {
      logger.log_warning(
          "Incomplete PDU at NG-U interface: missing or invalid PDU session container. pdu_len={} teid={}",
          pdu.buf.length(),
          teid);
      // As per TS 29.281 Sec. 5.2.2.7 the (...) PDU Session Container (...) shall be transmitted in a G-PDU over the N3
      // and N9 user plane interfaces (...).
      return;
    }

    if (!pdu.hdr.flags.seq_number) {
      // Sanity check
      if (reordering_timer.is_set()) {
        logger.log_warning("Forwarding T-PDU without SN while reordering timer is set.");
      }

      // Forward this SDU straight away.
      byte_buffer sdu = gtpu_extract_t_pdu(std::move(pdu)); // header is invalidated after extraction
      logger.log_info(sdu.begin(),
                      sdu.end(),
                      "RX SDU. sdu_len={} teid={} qos_flow={}",
                      sdu.length(),
                      teid,
                      pdu_session_info.qos_flow_id);
      lower_dn.on_new_sdu(std::move(sdu), pdu_session_info.qos_flow_id);
      return;
    }

    uint16_t    sn  = pdu.hdr.seq_number;
    byte_buffer sdu = gtpu_extract_t_pdu(std::move(pdu)); // header is invalidated after extraction

    // Check valid SN
    if (sn < st.rx_deliv) { // TODO: check wraparound
      logger.log_debug("Out-of-order after timeout, duplicate or count wrap-around. sn={} {}", sn, st);
      return;
    }

    // Check if PDU has been received
    if (rx_window->has_sn(sn)) { // TODO: check wraparound
      logger.log_debug("Duplicate PDU dropped. sn={}", sn);
      return;
    }

    (*rx_window)[sn] = std::move(sdu);

    // Update RX_NEXT
    if (sn >= st.rx_next) { // TODO: check wraparound
      st.rx_next = sn + 1;
    }

    if (sn == st.rx_deliv) { // TODO: check wraparound
      // Deliver all consecutive SDUs in ascending order of associated SN

      // TODO
    }
  }

private:
  psup_packing                             psup_packer;
  gtpu_tunnel_ngu_rx_lower_layer_notifier& lower_dn;

  /// Rx state
  gtpu_rx_state st = {};

  /// Rx window
  std::unique_ptr<gtpu_sdu_window_base<byte_buffer>> rx_window;

  /// Rx reordering timer
  unique_timer reordering_timer;
};
} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::gtpu_rx_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_rx_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "rx_next={} rx_deliv={} rx_reord={}", st.rx_next, st.rx_deliv, st.rx_reord);
  }
};

} // namespace fmt
