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
                                   timer_manager&                    timers,
                                   task_executor&                    ue_executor) :
  rlc_rx_entity(du_index, lcid, upper_dn),
  cfg(config),
  mod(cardinality(to_number(cfg.sn_field_length))),
  um_window_size(window_size(to_number(cfg.sn_field_length))),
  reassembly_timer(timers.create_unique_timer())
{
  // check reassembly_timer
  srsgnb_assert(reassembly_timer.is_valid(), "Cannot create RLC RX UM: reassembly_timer not configured");

  // configure reassembly_timer
  if (cfg.t_reassembly > 0) {
    reassembly_timer.set(static_cast<uint32_t>(cfg.t_reassembly), [this, &ue_executor](uint32_t tid) {
      ue_executor.execute([this, tid]() { on_expired_status_prohibit_timer(tid); });
    });
  }
}

// TS 38.322 v16.2.0 Sec. 5.2.3.2.2
void rlc_rx_um_entity::handle_pdu(byte_buffer_slice buf)
{
  metrics_add_pdus(1, buf.length());

  logger.log_debug(buf.begin(), buf.end(), "Rx data PDU ({} B)", buf.length());
  rlc_um_pdu_header header = {};
  if (not rlc_um_read_data_pdu_header(buf.view(), cfg.sn_field_length, &header)) {
    logger.log_warning("Failed to unpack header of RLC PDU");
    metrics_add_malformed_pdus(1);
    return;
  }
  logger.log_debug("PDU header: {}", header);

  // strip header, extract payload
  size_t            header_len = rlc_um_nr_packed_length(header);
  byte_buffer_slice payload    = buf.make_slice(header_len, buf.length() - header_len);

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
    rlc_rx_um_sdu_segment segment = {};
    segment.header                = header;
    segment.payload               = std::move(payload);

    // check if this SN is already present in rx buffer
    if (rx_window.find(header.sn) == rx_window.end()) {
      // first received segment of this SN, add to rx buffer
      logger.log_debug(segment.payload.begin(),
                       segment.payload.end(),
                       "placing {} segment of SN={} ({} B) in Rx buffer",
                       header.si,
                       header.sn,
                       segment.payload.length());
      rlc_rx_um_sdu_info sdu_info = {};
      update_total_sdu_length(sdu_info, segment);
      sdu_info.segments.emplace(header.so, std::move(segment));
      rx_window[header.sn] = std::move(sdu_info);
    } else {
      // other segment for this SN already present, update received data
      logger.log_debug(segment.payload.begin(),
                       segment.payload.end(),
                       "updating SN={} at SO={} with {} B",
                       segment.header.sn,
                       segment.header.so,
                       segment.payload.length());
      rlc_rx_um_sdu_info& sdu_info = rx_window.at(header.sn);

      // calculate total SDU length
      update_total_sdu_length(sdu_info, segment);

      // append to list of segments
      sdu_info.segments.emplace(header.so, std::move(segment));
    }

    // handle received segments
    handle_rx_buffer_update(header.sn);
  }

  log_state(srslog::basic_levels::debug);
}

// TS 38.322 v16.2.0 Sec 5.2.2.2.3
void rlc_rx_um_entity::handle_rx_buffer_update(const uint32_t sn)
{
  if (rx_window.find(sn) != rx_window.end()) {
    bool sdu_complete = false;

    // iterate over received segments and try to assemble full SDU
    rlc_rx_um_sdu_info& sdu_info = rx_window.at(sn);
    for (auto it = sdu_info.segments.begin(); it != sdu_info.segments.end();) {
      logger.log_debug(
          "Have {} segment with SO={} for SN={}", it->second.header.si, it->second.header.so, it->second.header.sn);
      if (it->second.header.so == sdu_info.next_expected_so) {
        if (sdu_info.next_expected_so == 0) {
          if (sdu_info.sdu.empty()) {
            // reuse buffer of first segment for final SDU
            sdu_info.sdu              = std::move(it->second.payload);
            sdu_info.next_expected_so = sdu_info.sdu.length();
            logger.log_debug("Reusing first segment of SN={} for final SDU", it->second.header.sn);
            it = sdu_info.segments.erase(it);
          } else {
            logger.log_debug("SDU buffer already allocated. Possible retransmission of first segment.");
            if (it->second.header.so != sdu_info.next_expected_so) {
              logger.log_error("Invalid PDU. SO doesn't match. Discarding all segments of SN={}.", sn);
              rx_window.erase(sn);
              return;
            }
          }
        } else {
          // add this segment to the end of the SDU buffer
          sdu_info.sdu.push_back(std::move(it->second.payload));
          sdu_info.next_expected_so += it->second.payload.length();
          logger.log_debug("Appended SO={} of SN={}", it->second.header.so, it->second.header.sn);
          it = sdu_info.segments.erase(it);

          if (sdu_info.next_expected_so == sdu_info.total_sdu_length) {
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
      logger.log_info("Rx SDU ({} B)", sdu_info.sdu.length());
      metrics_add_sdus(1, sdu_info.sdu.length());
      upper_dn.on_new_sdu(std::move(sdu_info.sdu));

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
        for (const auto& sdu_info_pair : rx_window) {
          if (sdu_info_pair.first >= rx_mod_base(st.rx_next_highest - um_window_size)) {
            st.rx_next_reassembly = sdu_info_pair.first;
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

void rlc_rx_um_entity::update_total_sdu_length(rlc_rx_um_sdu_info& sdu_info, const rlc_rx_um_sdu_segment& segment)
{
  if (segment.header.si == rlc_si_field::last_segment) {
    sdu_info.total_sdu_length = segment.header.so + segment.payload.length();
    logger.log_debug("updating total SDU length for SN={} to {} B", segment.header.sn, sdu_info.total_sdu_length);
  }
};

// TS 38.322 v16.2.0 Sec. 5.2.2.2.4
void rlc_rx_um_entity::on_expired_status_prohibit_timer(uint32_t timeout_id)
{
  if (reassembly_timer.id() == timeout_id) {
    logger.log_debug("reassembly timeout expiry for SN={} - updating rx_next_reassembly and reassembling",
                     st.rx_next_reassembly);
    metrics_add_lost_pdus(1);

    // update RX_Next_Reassembly to the next SN that has not been reassembled yet
    st.rx_next_reassembly = st.rx_timer_trigger;
    while (rx_mod_base(st.rx_next_reassembly) < rx_mod_base(st.rx_next_highest)) {
      st.rx_next_reassembly = (st.rx_next_reassembly + 1) % mod;
      log_state(srslog::basic_levels::debug);
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

    log_state(srslog::basic_levels::debug);
  }
}

// TS 38.322 v16.2.0 Sec 5.2.2.2.1
bool rlc_rx_um_entity::sn_in_reassembly_window(const uint32_t sn)
{
  return (rx_mod_base(st.rx_next_highest - um_window_size) <= rx_mod_base(sn) &&
          rx_mod_base(sn) < rx_mod_base(st.rx_next_highest));
}

// TS 38.322 v16.2.0 Sec 5.2.2.2.2
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
