/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_rx_um_entity.h"

using namespace srsgnb;

rlc_rx_um_entity::rlc_rx_um_entity(du_ue_index_t                     du_index,
                                   lcid_t                            lcid,
                                   const rlc_rx_um_config&           config,
                                   rlc_rx_upper_layer_data_notifier& upper_dn,
                                   timer_manager&                    timers) :
  rlc_rx_entity(du_index, lcid, upper_dn),
  cfg(config),
  mod((cfg.sn_field_length == rlc_um_sn_size::size6bits) ? 64 : 4096),
  um_window_size((cfg.sn_field_length == rlc_um_sn_size::size6bits) ? 32 : 2048),
  reassembly_timer(timers.create_unique_timer())
{
  // check timer
  srsran_assert(reassembly_timer.is_valid(), "Cannot create RLC RX UM: timers not configured");

  // configure timer
  if (cfg.t_reassembly_ms > 0) {
    reassembly_timer.set(static_cast<uint32_t>(cfg.t_reassembly_ms), [this](uint32_t tid) { timer_expired(tid); });
  }
}

// TS 38.322 v15.003 Section 5.2.2.2.4
void rlc_rx_um_entity::timer_expired(uint32_t timeout_id)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (reassembly_timer.id() == timeout_id) {
    logger.log_debug("reassembly timeout expiry for SN={} - updating rx_next_reassembly and reassembling",
                     st.rx_next_reassembly);
    metrics_add_lost_pdus(1);

    // update RX_Next_Reassembly to the next SN that has not been reassembled yet
    st.rx_next_reassembly = st.rx_timer_trigger;
    while (rx_mod_base(st.rx_next_reassembly) < rx_mod_base(st.rx_next_highest)) {
      st.rx_next_reassembly = (st.rx_next_reassembly + 1) % mod;
      debug_state();
    }

    // discard all segments with SN < updated RX_Next_Reassembly
    for (auto it = rx_window.begin(); it != rx_window.end();) {
      if (rx_mod_base(it->first) < rx_mod_base(st.rx_next_reassembly)) {
        it = rx_window.erase(it);
      } else {
        ++it;
      }
    }

    // check start of t_reassembly
    if (rx_mod_base(st.rx_next_highest) > rx_mod_base(st.rx_next_reassembly + 1) ||
        ((rx_mod_base(st.rx_next_highest) == rx_mod_base(st.rx_next_reassembly + 1) &&
          has_missing_byte_segment(st.rx_next_reassembly)))) {
      logger.log_debug("starting reassembly timer for SN={}", st.rx_next_reassembly);
      reassembly_timer.run();
      st.rx_timer_trigger = st.rx_next_highest;
    }

    debug_state();
  }
}

void rlc_rx_um_entity::handle_pdu(shared_byte_buffer_view buf)
{
  std::lock_guard<std::mutex> lock(mutex);

  metrics_add_pdus(1, buf.length());

  logger.log_debug(buf.begin(), buf.end(), "Rx data PDU ({} B)", buf.length());
  rlc_um_pdu_header header = {};
  if (not rlc_um_read_data_pdu_header(buf.view(), cfg.sn_field_length, &header)) {
    logger.log_warning("Failed to unpack header of RLC PDU");
    metrics_add_malformed_pdus(1);
    return;
  }

  // strip header, extract payload
  size_t                  header_len = rlc_um_nr_packed_length(header);
  shared_byte_buffer_view payload    = buf.shared_view(header_len, buf.length() - header_len);

  // check if PDU contains a SN
  if (header.si == rlc_si_field::full_sdu) {
    // deliver to upper layer
    logger.log_info("Rx SDU ({} B)", payload.length());
    metrics_add_sdus(1, payload.length());
    upper_dn.on_new_sdu(std::move(payload));
  } else if (sn_invalid_for_rx_buffer(header.sn)) {
    logger.log_info("Discarding SN={}", header.sn);
    // Nothing else to do here ..
  } else {
    // place PDU in receive buffer
    rlc_um_pdu rx_pdu = {};
    rx_pdu.header     = header;
    rx_pdu.payload    = std::move(payload);

    // check if this SN is already present in rx buffer
    if (rx_window.find(header.sn) == rx_window.end()) {
      // first received segment of this SN, add to rx buffer
      logger.log_debug(rx_pdu.payload.begin(),
                       rx_pdu.payload.end(),
                       "placing {} segment of SN={} ({} B) in Rx buffer",
                       to_string(header.si).c_str(),
                       header.sn,
                       rx_pdu.payload.length());
      rlc_umd_pdu_segments pdu_segments = {};
      update_total_sdu_length(pdu_segments, rx_pdu);
      pdu_segments.segments.emplace(header.so, std::move(rx_pdu));
      rx_window[header.sn] = std::move(pdu_segments);
    } else {
      // other segment for this SN already present, update received data
      logger.log_debug(rx_pdu.payload.begin(),
                       rx_pdu.payload.end(),
                       "updating SN={} at SO={} with {} B",
                       rx_pdu.header.sn,
                       rx_pdu.header.so,
                       rx_pdu.payload.length());
      auto& pdu_segments = rx_window.at(header.sn);

      // calculate total SDU length
      update_total_sdu_length(pdu_segments, rx_pdu);

      // append to list of segments
      pdu_segments.segments.emplace(header.so, std::move(rx_pdu));
    }

    // handle received segments
    handle_rx_buffer_update(header.sn);
  }

  debug_state();
}

// Sec 5.2.2.2.1
bool rlc_rx_um_entity::sn_in_reassembly_window(const uint32_t sn)
{
  return (rx_mod_base(st.rx_next_highest - um_window_size) <= rx_mod_base(sn) &&
          rx_mod_base(sn) < rx_mod_base(st.rx_next_highest));
}

// Sec 5.2.2.2.2
bool rlc_rx_um_entity::sn_invalid_for_rx_buffer(const uint32_t sn)
{
  return (rx_mod_base(st.rx_next_highest - um_window_size) <= rx_mod_base(sn) &&
          rx_mod_base(sn) < rx_mod_base(st.rx_next_reassembly));
}

bool rlc_rx_um_entity::has_missing_byte_segment(const uint32_t sn)
{
  // is at least one missing byte segment of the RLC SDU associated with SN = RX_Next_Reassembly before the last byte of
  // all received segments of this RLC SDU
  return (rx_window.find(sn) != rx_window.end());
}

// Sec 5.2.2.2.3
void rlc_rx_um_entity::handle_rx_buffer_update(const uint32_t sn)
{
  if (rx_window.find(sn) != rx_window.end()) {
    bool sdu_complete = false;

    // iterate over received segments and try to assemble full SDU
    auto& pdu = rx_window.at(sn);
    for (auto it = pdu.segments.begin(); it != pdu.segments.end();) {
      logger.log_debug("Have {} segment with SO={} for SN={}",
                       to_string(it->second.header.si).c_str(),
                       it->second.header.so,
                       it->second.header.sn);
      if (it->second.header.so == pdu.next_expected_so) {
        if (pdu.next_expected_so == 0) {
          if (pdu.sdu.empty()) {
            // reuse buffer of first segment for final SDU
            // TODO: optimize copy - e.g. change upstream SDU type to a list-like aggregate of shared_byte_buffer
            pdu.sdu              = byte_buffer(it->second.payload.begin(), it->second.payload.end());
            pdu.next_expected_so = pdu.sdu.length();
            logger.log_debug("Reusing first segment of SN={} for final SDU", it->second.header.sn);
            it = pdu.segments.erase(it);
          } else {
            logger.log_debug("SDU buffer already allocated. Possible retransmission of first segment.");
            if (it->second.header.so != pdu.next_expected_so) {
              logger.log_error("Invalid PDU. SO doesn't match. Discarding all segments of SN={}.", sn);
              rx_window.erase(sn);
              return;
            }
          }
        } else {
          // add this segment to the end of the SDU buffer
          // TODO: optimize copy - e.g. change upstream SDU type to a list-like aggregate of shared_byte_buffer
          pdu.sdu.append(it->second.payload);
          pdu.next_expected_so += it->second.payload.length();
          logger.log_debug("Appended SO={} of SN={}", it->second.header.so, it->second.header.sn);
          it = pdu.segments.erase(it);

          if (pdu.next_expected_so == pdu.total_sdu_length) {
            // entire SDU has been received, it will be passed up the stack outside the loop
            sdu_complete = true;
            break;
          }
        }
      } else {
        // handle next segment
        ++it;
      }
    }

    if (sdu_complete) {
      // deliver full SDU to upper layers
      logger.log_info("Rx SDU ({} B)", pdu.sdu.length());
      metrics_add_sdus(1, pdu.sdu.length());
      upper_dn.on_new_sdu(std::move(pdu.sdu));

      // delete PDU from rx_window
      rx_window.erase(sn);

      // find next SN in rx buffer
      if (sn == st.rx_next_reassembly) {
        if (rx_window.empty()) {
          // no further segments received
          st.rx_next_reassembly = st.rx_next_highest;
        } else {
          for (auto it = rx_window.begin(); it != rx_window.end(); ++it) {
            logger.log_debug("SN={} has {} segments", it->first, it->second.segments.size());
            if (rx_mod_base(it->first) > rx_mod_base(st.rx_next_reassembly)) {
              st.rx_next_reassembly = it->first;
              break;
            }
          }
        }
        logger.log_debug("Updating rx_next_reassembly={}", st.rx_next_reassembly);
      }
    } else if (not sn_in_reassembly_window(sn)) {
      // SN outside of rx window
      st.rx_next_highest = (sn + 1) % mod; // update RX_Next_highest
      logger.log_debug("Updating RX_Next_Highest={}", st.rx_next_highest);

      // drop all SNs outside of new rx window
      for (auto it = rx_window.begin(); it != rx_window.end();) {
        if (not sn_in_reassembly_window(it->first)) {
          logger.log_info("SN={} outside rx window [{}:{}] - discarding",
                          it->first,
                          st.rx_next_highest - um_window_size,
                          st.rx_next_highest);
          it = rx_window.erase(it);
          metrics_add_lost_pdus(1);
        } else {
          ++it;
        }
      }

      if (not sn_in_reassembly_window(st.rx_next_reassembly)) {
        // update RX_Next_Reassembly to first SN that has not been reassembled and delivered
        for (const auto& rx_pdu : rx_window) {
          if (rx_pdu.first >= rx_mod_base(st.rx_next_highest - um_window_size)) {
            st.rx_next_reassembly = rx_pdu.first;
            logger.log_debug("Updating rx_next_reassembly={}", st.rx_next_reassembly);
            break;
          }
        }
      }
    }

    if (reassembly_timer.is_running()) {
      if (st.rx_timer_trigger <= st.rx_next_reassembly ||
          (not sn_in_reassembly_window(st.rx_timer_trigger) and st.rx_timer_trigger != st.rx_next_highest) ||
          ((st.rx_next_highest == st.rx_next_reassembly + 1) && not has_missing_byte_segment(st.rx_next_reassembly))) {
        logger.log_debug("stopping reassembly timer");
        reassembly_timer.stop();
      }
    }

    if (not reassembly_timer.is_running()) {
      if ((rx_mod_base(st.rx_next_highest) > rx_mod_base(st.rx_next_reassembly + 1)) ||
          ((rx_mod_base(st.rx_next_highest) == rx_mod_base(st.rx_next_reassembly + 1)) &&
           has_missing_byte_segment(st.rx_next_reassembly))) {
        logger.log_debug("Starting reassembly timer for SN={}", sn);
        reassembly_timer.run();
        st.rx_timer_trigger = st.rx_next_highest;
      }
    }
  } else {
    logger.log_error("SN={} does not exist in Rx buffer", sn);
  }
}

void rlc_rx_um_entity::update_total_sdu_length(rlc_umd_pdu_segments& pdu_segments, const rlc_um_pdu& rx_pdu)
{
  if (rx_pdu.header.si == rlc_si_field::last_segment) {
    pdu_segments.total_sdu_length = rx_pdu.header.so + rx_pdu.payload.length();
    logger.log_debug("updating total SDU length for SN={} to {} B", rx_pdu.header.sn, pdu_segments.total_sdu_length);
  }
};
