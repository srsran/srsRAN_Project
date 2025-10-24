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

#include "pdcp_entity_tx.h"
#include "../security/security_engine_impl.h"
#include "srsran/instrumentation/traces/up_traces.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/executors/execution_context_description.h"
#include "srsran/support/srsran_assert.h"
#include <algorithm>

using namespace srsran;

pdcp_entity_tx::pdcp_entity_tx(uint32_t                        ue_index,
                               rb_id_t                         rb_id_,
                               pdcp_tx_config                  cfg_,
                               pdcp_tx_lower_notifier&         lower_dn_,
                               pdcp_tx_upper_control_notifier& upper_cn_,
                               timer_factory                   ue_ctrl_timer_factory_,
                               task_executor&                  ue_dl_executor_,
                               task_executor&                  crypto_executor_,
                               uint32_t                        max_nof_crypto_workers_,
                               pdcp_metrics_aggregator&        metrics_agg_) :
  pdcp_entity_tx_rx_base(rb_id_, cfg_.rb_type, cfg_.rlc_mode, cfg_.sn_size),
  logger("PDCP", {ue_index, rb_id_, "DL"}),
  cfg(cfg_),
  lower_dn(lower_dn_),
  upper_cn(upper_cn_),
  ue_ctrl_timer_factory(ue_ctrl_timer_factory_),
  ue_dl_executor(ue_dl_executor_),
  crypto_executor(crypto_executor_),
  max_nof_crypto_workers(max_nof_crypto_workers_),
  tx_window(cfg.rb_type, cfg.rlc_mode, cfg.sn_size, logger),
  metrics(metrics_agg_.get_metrics_period().count()),
  metrics_agg(metrics_agg_)
{
  if (metrics_agg.get_metrics_period().count()) {
    metrics_timer = ue_ctrl_timer_factory.create_timer();
    metrics_timer.set(std::chrono::milliseconds(metrics_agg.get_metrics_period().count()), [this](timer_id_t tid) {
      if (stopped) {
        return;
      }
      metrics_agg.push_tx_metrics(metrics.get_metrics_and_reset());
      metrics_timer.run();
    });
    metrics_timer.run();
  }

  // Validate configuration
  if (is_srb() && (cfg.sn_size != pdcp_sn_size::size12bits)) {
    report_error("PDCP SRB with invalid sn_size. {}", cfg);
  }
  if (is_srb() && is_um()) {
    report_error("PDCP SRB cannot be used with RLC UM. {}", cfg);
  }
  if (is_srb() && cfg.discard_timer != pdcp_discard_timer::infinity) {
    logger.log_error("Invalid SRB config with discard_timer={}", cfg.discard_timer);
  }
  if (is_drb() && !cfg.discard_timer.has_value()) {
    logger.log_error("Invalid DRB config, discard_timer is not configured");
  }

  logger.log_info("PDCP configured. {}", cfg);

  discard_timer           = ue_ctrl_timer_factory.create_timer();
  crypto_reordering_timer = ue_ctrl_timer_factory.create_timer();

  crypto_reordering_timer.set(std::chrono::milliseconds{pdcp_tx_crypto_reordering_timeout_ms},
                              [this](timer_id_t /*timer_id*/) { crypto_reordering_timeout(); });

  // Populate null security engines
  sec_engine_pool.reserve(max_nof_crypto_workers);
  for (uint32_t i = 0; i < max_nof_crypto_workers; i++) {
    std::unique_ptr<security::security_engine_impl> null_engine;
    sec_engine_pool.push_back(std::move(null_engine));
  }
}

pdcp_entity_tx::~pdcp_entity_tx()
{
  stop();
}

void pdcp_entity_tx::stop()
{
  if (not stopped) {
    stopped = true;
    tx_window.clear(); // discard all SDUs
    if (cfg.discard_timer.has_value()) {
      discard_timer.stop();
    }
    crypto_reordering_timer.stop();
    metrics_timer.stop();
    token_mngr.stop();
    logger.log_debug("Stopped PDCP entity");
  }
}

void pdcp_entity_tx::notify_pdu_processing_stopped()
{
  if (not stopped) {
    token_mngr.stop();
    logger.log_debug("Stopped PDCP entity PDU processing");
  }
}

void pdcp_entity_tx::restart_pdu_processing()
{
  if (not stopped) {
    token_mngr.start();
    logger.log_debug("Started PDCP entity PDU processing");
  }
}

manual_event_flag& pdcp_entity_tx::crypto_awaitable()
{
  return token_mngr.get_awaitable();
}

/// \brief Receive an SDU from the upper layers, apply encryption
/// and integrity protection and pass the resulting PDU
/// to the lower layers.
///
/// \param sdu Buffer that hold the SDU from higher layers.
/// \ref TS 38.323 section 5.2.1: Transmit operation
void pdcp_entity_tx::handle_sdu(byte_buffer buf)
{
  metrics.add_sdus(1, buf.length());
  logger.log_debug(buf.begin(), buf.end(), "TX SDU. sdu_len={}", buf.length());

  if (SRSRAN_UNLIKELY(stopped)) {
    if (not cfg.custom.warn_on_drop) {
      logger.log_info("Dropping SDU. Entity is stopped");
    } else {
      logger.log_warning("Dropping SDU. Entity is stopped");
    }
    metrics.add_lost_sdus(1);
    return;
  }

  trace_point                           tx_tp           = up_tracer.now();
  std::chrono::system_clock::time_point time_of_arrival = std::chrono::high_resolution_clock::now();

  if (is_drb()) {
    auto drop_reason = check_early_drop(buf);
    if (drop_reason != early_drop_reason::no_drop) {
      if (warn_on_drop_count == 0) {
        // Log only at the start of a drop burst.
        if (cfg.custom.warn_on_drop) {
          logger.log_warning("Dropping SDU. Cause: {}", drop_reason);
        } else {
          logger.log_info("Dropping SDU. Cause: {}", drop_reason);
        }
      }
      warn_on_drop_count++;
      metrics.add_lost_sdus(1);
      return;
    }
    if (warn_on_drop_count != 0) {
      if (cfg.custom.warn_on_drop) {
        logger.log_warning("Drop burst finished. nof_sdus={}", warn_on_drop_count);
      } else {
        logger.log_info("Drop burst finished. nof_sdus={}", warn_on_drop_count);
      }
      warn_on_drop_count = 0;
    }
  }

  // The PDCP is not allowed to use the same COUNT value more than once for a given security key,
  // see TS 38.331, section 5.3.1.2. To avoid this, we notify the RRC once we exceed a "maximum"
  // COUNT. It is then the RRC's responsibility to refresh the keys. We continue transmitting until
  // we reached a maximum hard COUNT, after which we simply refuse to TX any further.
  if (st.tx_next >= cfg.custom.max_count.hard) {
    if (!max_count_overflow) {
      logger.log_error("Reached maximum count, refusing to transmit further. count={}", st.tx_next);
      metrics.add_lost_sdus(1);
      upper_cn.on_protocol_failure();
      max_count_overflow = true;
    }
    return;
  }
  if (st.tx_next >= cfg.custom.max_count.notify) {
    if (!max_count_notified) {
      logger.log_warning("Approaching count wrap-around, notifying RRC. count={}", st.tx_next);
      upper_cn.on_max_count_reached();
      max_count_notified = true;
    }
  }

  // Wrap SDU with meta information.
  pdcp_tx_window_sdu_info sdu_info = {
      .sdu                   = {}, // we will store a copy of the SDU here later if required.
      .count                 = st.tx_next,
      .time_of_arrival       = time_of_arrival,
      .tick_point_of_arrival = {} // set later only for finite discard timer
  };

  // Start discard timer, if required.
  if (cfg.discard_timer.has_value()) {
    // Only start for finite durations and if not running already.
    if (cfg.discard_timer.value() != pdcp_discard_timer::infinity) {
      // Start the discard timer if not running.
      // Keep tick point of arrival for discard timer optimizations.
      sdu_info.tick_point_of_arrival = discard_timer.now();
      if (not discard_timer.is_running()) {
        discard_timer.set(std::chrono::milliseconds(static_cast<unsigned>(cfg.discard_timer.value())),
                          [this](timer_id_t timer_id) { discard_callback(); });
        discard_timer.run();
      }
    }

    // For AM, we need also to copy the SDU buffer into the tx_window
    // for a possible data recovery procedure.
    if (is_am()) {
      expected<byte_buffer> sdu_copy = buf.deep_copy();
      if (not sdu_copy.has_value()) {
        logger.log_error("Unable to deep copy SDU");
        metrics.add_lost_sdus(1);
        upper_cn.on_protocol_failure();
        return;
      }
      sdu_info.sdu = std::move(sdu_copy.value());
    }
  }

  // Add SDU info into TX window; determine the SDU length from original buffer.
  tx_window.add_sdu(std::move(sdu_info), buf.length());
  logger.log_debug("Added to tx window. count={} discard_timer={}", st.tx_next, cfg.discard_timer);

  // Perform header compression
  // TODO

  // Prepare header
  pdcp_data_pdu_header hdr = {};
  hdr.sn                   = SN(st.tx_next);

  // Pack header
  if (not write_data_pdu_header(buf, hdr)) {
    logger.log_error("Could not append PDU header, dropping SDU and notifying RRC. count={}", st.tx_next);
    metrics.add_lost_sdus(1);
    upper_cn.on_protocol_failure();
    return;
  }

  /// Prepare buffer info struct to pass to crypto executor.
  pdcp_tx_buffer_info buf_info{.is_retx = false,
                               .retx_id = retransmit_id,
                               .count   = st.tx_next,
                               .buf     = std::move(buf),
                               .token   = pdcp_crypto_token(token_mngr)};

  // Increment TX_NEXT. We do this before passing the SDU+Header
  // to the crypto executor, so that the reordering function has the updated state.
  st.tx_next++;

  // Apply security in crypto executor
  auto fn = [this, buf_info = std::move(buf_info)]() mutable { apply_security(std::move(buf_info)); };
  if (not crypto_executor.execute(std::move(fn))) {
    logger.log_warning("Dropped PDU, crypto executor queue is full. st={}", st);
    metrics.add_lost_sdus(1);
  }

  up_tracer << trace_event{"pdcp_tx_pdu", tx_tp};
}

void pdcp_entity_tx::apply_reordering(pdcp_tx_buffer_info buf_info)
{
  if (SRSRAN_UNLIKELY(stopped)) {
    logger.log_debug("Dropping security protected PDU. Entity is stopped");
    return;
  }

  // Drop PDU if its out of date due to retransmissions.
  if (buf_info.retx_id != retransmit_id) {
    logger.log_debug(
        "Dropping PDU, PDU out of date due to retransmissions. count={} st={} old_retx_id={} new_retx_id={}",
        buf_info.count,
        st,
        buf_info.retx_id,
        retransmit_id);
    return;
  }

  // Drop PDU if TX window has advanced and COUNT is no longer inside the TX window.
  if (buf_info.count < st.tx_next_ack) {
    logger.log_warning("Dropping PDU, COUNT no longer inside TX window. count={} st={}", buf_info.count, st);
    return;
  }

  // Drop PDU if PDU is inside window, but we already got a transmission notification for it.
  if (buf_info.count < st.tx_trans) {
    logger.log_error(
        "Dropping PDU, already got transmission notification for this COUNT. count={} st={}", buf_info.count, st);
    return;
  }

  // Sanity check if we have SDU in window.
  if (not tx_window.has_sn(buf_info.count)) {
    logger.log_error("Dropping PDU, SDU does not exist in TX window. count={} st={}", buf_info.count, st);
    return;
  }

  // Store protected PDU in TX window.
  tx_window[buf_info.count].pdu = std::move(buf_info.buf);
  if (buf_info.count != st.tx_trans_crypto) {
    logger.log_debug("Buffered PDU and awaiting reordering. count={} st={}", buf_info.count, st);
  }

  // Deliver in order PDUs. Break and update TX_TRANS_CRYPTO when we find a missing PDU.
  for (uint32_t count = st.tx_trans_crypto; count < st.tx_next && not tx_window[count].pdu.empty(); count++) {
    pdcp_tx_pdu_info pdu_info{
        .pdu = std::move(tx_window[count].pdu), .count = count, .sdu_toa = tx_window[count].sdu_info.time_of_arrival};
    write_data_pdu_to_lower_layers(std::move(pdu_info), buf_info.is_retx);
    st.tx_trans_crypto = count + 1;
    // Automatically trigger delivery notifications when using test mode
    if (cfg.custom.test_mode) {
      handle_transmit_notification(SN(count));
    }
  }

  // Handle reordering timers
  if (crypto_reordering_timer.is_running() and st.tx_trans_crypto >= st.tx_reord_crypto) {
    crypto_reordering_timer.stop();
    logger.log_debug("Stopped t-Reordering.", st);
  }
  if (not crypto_reordering_timer.is_running() and st.tx_trans_crypto < st.tx_next) {
    st.tx_reord_crypto = st.tx_next;
    crypto_reordering_timer.run();
    logger.log_debug("Started t-Reordering.");
  }
}

void pdcp_entity_tx::reestablish(security::sec_128_as_config sec_cfg)
{
  logger.log_debug("Reestablishing PDCP. st={}", st);
  // - for UM DRBs and AM DRBs, reset the ROHC protocol for uplink and start with an IR state in U-mode (as
  //   defined in RFC 3095 [8] and RFC 4815 [9]) if drb-ContinueROHC is not configured in TS 38.331 [3];
  // - for UM DRBs and AM DRBs, reset the EHC protocol for uplink if drb-ContinueEHC-UL is not configured in
  //   TS 38.331 [3];
  //   Header compression not supported yet (TODO).

  // - for UM DRBs and SRBs, set TX_NEXT to the initial value;
  // - for SRBs, discard all stored PDCP SDUs and PDCP PDUs;
  if (is_srb() || is_um()) {
    reset(); // While not explicitly stated in the spec,
             // there is no point in storing PDCP UM PDUs.
             // They cannot be RETXed and RLC already discarded them.
             // Also, this avoids having multiple discard timers
             // associated with the with the same COUNT.
  }

  // - apply the ciphering algorithm and key provided by upper layers during the PDCP entity re-establishment
  //   procedure;
  // - apply the integrity protection algorithm and key provided by upper layers during the PDCP entity re-
  //   establishment procedure;
  configure_security(sec_cfg, integrity_enabled, ciphering_enabled);

  // - for UM DRBs, for each PDCP SDU already associated with a PDCP SN but for which a corresponding PDU has
  //   not previously been submitted to lower layers, and;
  // - for AM DRBs for Uu interface whose PDCP entities were suspended, from the first PDCP SDU for which the
  //   successful delivery of the corresponding PDCP Data PDU has not been confirmed by lower layers, for each
  //   PDCP SDU already associated with a PDCP SN:
  //   - consider the PDCP SDUs as received from upper layer;
  //   - perform transmission of the PDCP SDUs in ascending order of the COUNT value associated to the PDCP
  //     SDU prior to the PDCP re-establishment without restarting the discardTimer, as specified in clause 5.2.1;
  //
  //  For UM DRBs, when SDUs are associated with a PDCP SN they are immediately pushed to the lower-layer.
  //  As such, there is nothing to do here.
  //  For AM DRBs, PDCP entity suspension is not supported yet (TODO).

  // - for AM DRBs whose PDCP entities were not suspended, from the first PDCP SDU for which the successful
  //   delivery of the corresponding PDCP Data PDU has not been confirmed by lower layers, perform retransmission
  //   or transmission of all the PDCP SDUs already associated with PDCP SNs in ascending order of the COUNT
  //   values associated to the PDCP SDU prior to the PDCP entity re-establishment as specified below:
  //   - perform header compression of the PDCP SDU using ROHC as specified in the clause 5.7.4 and/or using
  //     EHC as specified in the clause 5.12.4;
  //   - perform integrity protection and ciphering of the PDCP SDU using the COUNT value associated with this
  //     PDCP SDU as specified in the clause 5.9 and 5.8;
  //   - submit the resulting PDCP Data PDU to lower layer, as specified in clause 5.2.1.
  if (is_am()) {
    retransmit_all_pdus();
  }
  logger.log_info("Reestablished PDCP. st={}", st);
}

void pdcp_entity_tx::write_data_pdu_to_lower_layers(pdcp_tx_pdu_info&& pdu_info, bool is_retx)
{
  logger.log_info(pdu_info.pdu.begin(),
                  pdu_info.pdu.end(),
                  "TX PDU. type=data pdu_len={} sn={} count={} is_retx={}",
                  pdu_info.pdu.length(),
                  SN(pdu_info.count),
                  pdu_info.count,
                  is_retx);
  metrics.add_pdus(1, pdu_info.pdu.length());
  auto sdu_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() -
                                                                             pdu_info.sdu_toa);
  metrics.add_pdu_latency_ns(sdu_latency_ns.count());
  lower_dn.on_new_pdu(std::move(pdu_info.pdu), is_retx);
}

void pdcp_entity_tx::write_control_pdu_to_lower_layers(byte_buffer buf)
{
  logger.log_info(buf.begin(), buf.end(), "TX PDU. type=ctrl pdu_len={}", buf.length());
  metrics.add_pdus(1, buf.length());
  lower_dn.on_new_pdu(std::move(buf), /* is_retx = */ false);
}

void pdcp_entity_tx::handle_status_report(byte_buffer_chain status)
{
  auto status_buffer = byte_buffer::create(status.begin(), status.end());
  if (not status_buffer.has_value()) {
    logger.log_warning("Unable to allocate byte_buffer");
    return;
  }

  byte_buffer buf = std::move(status_buffer.value());
  bit_decoder dec(buf);

  // Unpack and check PDU header
  uint32_t dc = 0;
  dec.unpack(dc, 1);
  if (dc != to_number(pdcp_dc_field::control)) {
    logger.log_warning(
        buf.begin(), buf.end(), "Invalid D/C field in status report. dc={}", to_number(pdcp_dc_field::control), dc);
    return;
  }
  uint32_t cpt = 0;
  dec.unpack(cpt, 3);
  if (cpt != to_number(pdcp_control_pdu_type::status_report)) {
    logger.log_warning(buf.begin(),
                       buf.end(),
                       "Invalid CPT field in status report. cpt={}",
                       to_number(pdcp_control_pdu_type::status_report),
                       cpt);
    return;
  }
  uint32_t reserved = 0;
  dec.unpack(reserved, 4);
  if (reserved != 0) {
    logger.log_warning(
        buf.begin(), buf.end(), "Ignoring status report because reserved bits are set. reserved={:#x}", reserved);
    return;
  }

  // Unpack FMC field
  uint32_t fmc = 0;
  dec.unpack(fmc, 32);
  logger.log_info("Status report. fmc={}", fmc);

  // Discard any SDU with COUNT < FMC
  for (uint32_t count = st.tx_next_ack; count < fmc; count++) {
    discard_pdu(count);
  }

  // Evaluate bitmap: discard any SDU with the bit in the bitmap set to 1
  unsigned bit = 0;
  while (dec.unpack(bit, 1)) {
    fmc++;
    // Bit == 0: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is missing.
    // Bit == 1: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is correctly received.
    if (bit == 1) {
      discard_pdu(fmc);
    }
  }
}

/*
 * Ciphering and Integrity Protection Helpers
 */
void pdcp_entity_tx::apply_security(pdcp_tx_buffer_info buf_info)

{
  auto     pre      = std::chrono::high_resolution_clock::now();
  uint32_t tx_count = buf_info.count;

  // Apply deciphering and integrity check
  security::security_result result = apply_ciphering_and_integrity_protection(std::move(buf_info.buf), tx_count);

  if (!result.buf.has_value()) {
    auto handle_failure = [this, sec_err = result.buf.error(), count = result.count]() {
      switch (sec_err) {
        case srsran::security::security_error::integrity_failure:
          logger.log_warning("Applying integrity failed, dropping PDU. count={}", count);
          upper_cn.on_protocol_failure();
          break;
        case srsran::security::security_error::ciphering_failure:
          logger.log_warning("Applying ciphering failed, dropping PDU. count={}", count);
          upper_cn.on_protocol_failure();
          break;
        case srsran::security::security_error::buffer_failure:
          logger.log_error("Buffer error when ciphering and applying integrity, dropping PDU. count={}", count);
          upper_cn.on_protocol_failure();
          break;
        case srsran::security::security_error::engine_failure:
          logger.log_error("Engine error when ciphering and applying integrity, dropping PDU. count={}", count);
          upper_cn.on_protocol_failure();
          break;
      }
    };
    if (not ue_dl_executor.execute(std::move(handle_failure))) {
      logger.log_warning("Dropped PDU with security error, UE executor queue is full. count={} sec_err={}",
                         tx_count,
                         result.buf.error());
    }
    return;
  }
  logger.log_debug(result.buf.value().begin(), result.buf.value().end(), "Security applied. count={}", tx_count);

  pdcp_tx_buffer_info pdu_info{.is_retx = buf_info.is_retx,
                               .retx_id = buf_info.retx_id,
                               .count   = tx_count,
                               .buf     = std::move(result.buf.value()),
                               .token   = std::move(buf_info.token)};

  auto post           = std::chrono::high_resolution_clock::now();
  auto sdu_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(post - pre);
  metrics.add_crypto_processing_latency(sdu_latency_ns.count());

  // apply reordering in UE executor
  auto fn = [this, pdu_info = std::move(pdu_info)]() mutable { apply_reordering(std::move(pdu_info)); };
  if (not ue_dl_executor.execute(std::move(fn))) {
    if (cfg.custom.warn_on_drop) {
      logger.log_warning("Dropped PDU, UE executor queue is full. count={}", tx_count);
    } else {
      logger.log_debug("Dropped PDU, UE executor queue is full. count={}", tx_count);
    }
  }
}

security::security_result pdcp_entity_tx::apply_ciphering_and_integrity_protection(byte_buffer buf, uint32_t count)
{
  // obtain the thread-specific ID of the worker
  uint32_t worker_idx = execution_context::get_current_worker_index();

  if (worker_idx >= max_nof_crypto_workers) {
    srsran_assertion_failure("Worker index exceeds number of crypto workers. worker_idx={} max_nof_crypto_workers={}",
                             worker_idx,
                             max_nof_crypto_workers);
    logger.log_error("Worker index exceeds number of crypto workers. worker_idx={} max_nof_crypto_workers={}",
                     worker_idx,
                     max_nof_crypto_workers);
    return {.buf = make_unexpected(srsran::security::security_error::engine_failure), .count = count};
  }

  logger.log_debug("Using sec_engine with worker_idx={}. count={} pdu_len={}", worker_idx, count, buf.length());

  security::security_engine_tx* sec_engine = sec_engine_pool[worker_idx].get();
  if (sec_engine == nullptr) {
    // Security is not configured. Append zero MAC-I for SRBs, error for DRBs.
    if (is_srb()) {
      security::sec_mac mac = {};
      if (not buf.append(mac)) {
        logger.log_warning("Failed to append MAC-I to PDU. count={}", count);
        return {.buf = make_unexpected(srsran::security::security_error::buffer_failure), .count = count};
      }
      return {.buf = std::move(buf), .count = count};
    }
    logger.log_error("Empty engine for DRB bearer. count={}", count);
    return {.buf = make_unexpected(srsran::security::security_error::engine_failure), .count = count};
  }

  // TS 38.323, section 5.8: Ciphering
  // The data unit that is ciphered is the MAC-I and the
  // data part of the PDCP Data PDU except the
  // SDAP header and the SDAP Control PDU if included in the PDCP SDU.

  // TS 38.323, section 5.9: Integrity protection
  // The data unit that is integrity protected is the PDU header
  // and the data part of the PDU before ciphering.

  unsigned                  hdr_size = cfg.sn_size == pdcp_sn_size::size12bits ? 2 : 3;
  security::security_result result   = sec_engine->encrypt_and_protect_integrity(std::move(buf), hdr_size, count);
  if (!result.buf.has_value()) {
    logger.log_warning("Failed to apply security on PDU. count={}", result.count);
  }
  return {std::move(result.buf.value())};
}

/*
 * Security configuration
 */
void pdcp_entity_tx::configure_security(security::sec_128_as_config sec_cfg,
                                        security::integrity_enabled integrity_enabled_,
                                        security::ciphering_enabled ciphering_enabled_)
{
  srsran_assert((is_srb() && sec_cfg.domain == security::sec_domain::rrc) ||
                    (is_drb() && sec_cfg.domain == security::sec_domain::up),
                "Invalid sec_domain={} for {} in {}",
                sec_cfg.domain,
                rb_type,
                rb_id);
  // The 'NULL' integrity protection algorithm (nia0) is used only for SRBs and for the UE in limited service mode,
  // see TS 33.501 [11] and when used for SRBs, integrity protection is disabled for DRBs. In case the ′NULL'
  // integrity protection algorithm is used, 'NULL' ciphering algorithm is also used.
  // Ref: TS 38.331 Sec. 5.3.1.2
  //
  // From TS 38.501 Sec. 6.7.3.6: UEs that are in limited service mode (LSM) and that cannot be authenticated (...)
  // may still be allowed to establish emergency session by sending the emergency registration request message. (...)
  if ((sec_cfg.integ_algo == security::integrity_algorithm::nia0) &&
      (is_drb() || (is_srb() && sec_cfg.cipher_algo != security::ciphering_algorithm::nea0))) {
    logger.log_error("Integrity algorithm NIA0 is only permitted for SRBs configured with NEA0. is_srb={} NIA{} NEA{}",
                     is_srb(),
                     sec_cfg.integ_algo,
                     sec_cfg.cipher_algo);
  }

  // Evaluate and store integrity indication
  if (integrity_enabled_ == security::integrity_enabled::on) {
    if (!sec_cfg.k_128_int.has_value()) {
      logger.log_error("Cannot enable integrity protection: Integrity key is not configured.");
      return;
    }
    if (!sec_cfg.integ_algo.has_value()) {
      logger.log_error("Cannot enable integrity protection: Integrity algorithm is not configured.");
      return;
    }
  } else {
    srsran_assert(!is_srb(), "Integrity protection cannot be disabled for SRBs.");
  }
  integrity_enabled = integrity_enabled_;

  // Evaluate and store ciphering indication
  ciphering_enabled = ciphering_enabled_;

  auto direction = cfg.direction == pdcp_security_direction::uplink ? security::security_direction::uplink
                                                                    : security::security_direction::downlink;
  // Remove previous security engines
  sec_engine_pool.clear();
  sec_engine_pool.reserve(max_nof_crypto_workers);

  // Populate new security engines
  for (uint32_t i = 0; i < max_nof_crypto_workers; i++) {
    auto sec_engine = std::make_unique<security::security_engine_impl>(
        sec_cfg, bearer_id, direction, integrity_enabled, ciphering_enabled);
    sec_engine_pool.push_back(std::move(sec_engine));
  }

  logger.log_info("Security configured: NIA{} ({}) NEA{} ({}) domain={}",
                  sec_cfg.integ_algo,
                  integrity_enabled,
                  sec_cfg.cipher_algo,
                  ciphering_enabled,
                  sec_cfg.domain);
  if (sec_cfg.k_128_int.has_value()) {
    logger.log_info("128 K_int: {}", sec_cfg.k_128_int.value());
  }
  logger.log_info("128 K_enc: {}", sec_cfg.k_128_enc);
}

/*
 * Status report and data recovery
 */
void pdcp_entity_tx::send_status_report()
{
  if (cfg.status_report_required) {
    logger.log_info("Status report triggered.");
    byte_buffer status_report = status_provider->compile_status_report();
    write_control_pdu_to_lower_layers(std::move(status_report));
  } else {
    logger.log_warning("Status report triggered but not configured.");
  }
}

void pdcp_entity_tx::data_recovery()
{
  srsran_assert(is_drb() && cfg.rlc_mode == pdcp_rlc_mode::am, "Invalid bearer type for data recovery.");
  logger.log_info("Data recovery requested.");

  /*
   * TS 38.323 Sec. 5.4.1:
   * [...] the receiving PDCP entity shall trigger a PDCP status report when:
   * [...] -upper layer requests a PDCP data recovery; [...]
   */
  if (cfg.status_report_required) {
    send_status_report();
  }
  retransmit_all_pdus();
}

void pdcp_entity_tx::reset()
{
  st = {0, 0, 0, 0, 0};
  tx_window.clear();
  logger.log_debug("Entity was reset. {}", st);
}

void pdcp_entity_tx::retransmit_all_pdus()
{
  if (!cfg.discard_timer.has_value()) {
    logger.log_debug("Cannot retransmit. No discard timer configured.");
    return;
  }
  if (!is_am()) {
    logger.log_error("Cannot retransmit. Not an AM bearer.");
    return;
  }

  // Since we are retransmitting, rewind tx_trans to tx_next_ack
  st.tx_trans = st.tx_next_ack;

  // Also rewind tx_trans_pending to tx_next_ack. Any PDUs pending
  // transmission will be dropped, as the retransmit ID will no longer match.
  st.tx_trans_crypto = st.tx_next_ack;
  retransmit_id++;

  for (uint32_t count = st.tx_next_ack; count < st.tx_next; count++) {
    if (tx_window.has_sn(count)) {
      pdcp_tx_window_element& window_elem = tx_window[count];

      // Create a copy of the SDU buffer that is stored in the TX window
      auto exp_buf_copy = window_elem.sdu_info.sdu.deep_copy();
      if (not exp_buf_copy.has_value()) {
        logger.log_error(
            "Could not deep copy SDU for retransmission, dropping SDU and notifying RRC. count={} {}", count, st);
        upper_cn.on_protocol_failure();
        return;
      }

      byte_buffer buf = std::move(exp_buf_copy.value());

      // Perform header compression if required
      // (TODO)

      // Prepare header
      pdcp_data_pdu_header hdr = {};
      hdr.sn                   = SN(count);

      // Pack header
      if (not write_data_pdu_header(buf, hdr)) {
        logger.log_error("Could not append PDU header, dropping SDU and notifying RRC. count={} {}", count, st);
        upper_cn.on_protocol_failure();
        return;
      }

      /// Prepare buffer info struct to pass to crypto executor.
      pdcp_tx_buffer_info buf_info{.is_retx = true,
                                   .retx_id = retransmit_id,
                                   .count   = count,
                                   .buf     = std::move(buf),
                                   .token   = pdcp_crypto_token(token_mngr)};

      auto fn = [this, buf_info = std::move(buf_info)]() mutable { apply_security(std::move(buf_info)); };
      if (not crypto_executor.execute(std::move(fn))) {
        logger.log_warning("Dropped PDU, crypto executor queue is full. st={}", st);
      }
    }
  }
}

/*
 * Notification Helpers
 */
void pdcp_entity_tx::handle_transmit_notification(uint32_t notif_sn)
{
  if (SRSRAN_UNLIKELY(stopped)) {
    logger.log_debug("Dropping transmit notification. Entity is stopped");
    return;
  }

  handle_transmit_notification_impl(notif_sn, false);
}

void pdcp_entity_tx::handle_transmit_notification_impl(uint32_t notif_sn, bool is_retx)
{
  logger.log_debug("Handling transmit notification for notif_sn={} is_retx={}", notif_sn, is_retx);
  if (notif_sn >= pdcp_sn_cardinality(cfg.sn_size)) {
    logger.log_error(
        "Invalid transmit notification for notif_sn={} exceeds sn_size={}. is_retx={}", notif_sn, cfg.sn_size, is_retx);
    return;
  }
  uint32_t notif_count = notification_count_estimation(notif_sn);
  if (notif_count >= st.tx_trans_crypto) {
    logger.log_error("Invalid notification SN, notif_count is larger then pending TX'es. notif_sn={} notif_count={} {}",
                     notif_sn,
                     notif_count,
                     st);
    return;
  }
  if (notif_count < st.tx_trans) {
    logger.log_info("Invalid notification SN, notif_count is too low. notif_sn={} notif_count={} is_retx={} {}",
                    notif_sn,
                    notif_count,
                    is_retx,
                    st);
    return;
  }
  if (notif_count >= st.tx_next) {
    logger.log_error("Invalid notification SN, notif_count is too high. notif_sn={} notif_count={} is_retx={} {}",
                     notif_sn,
                     notif_count,
                     is_retx,
                     st);
    return;
  }
  st.tx_trans = notif_count + 1;
  logger.log_debug("Updated tx_trans. {}", st);

  // Stop discard timers if required
  if (!cfg.discard_timer.has_value()) {
    return;
  }

  if (is_um()) {
    stop_discard_timer(notif_count);
  }
}

void pdcp_entity_tx::handle_delivery_notification(uint32_t notif_sn)
{
  if (SRSRAN_UNLIKELY(stopped)) {
    logger.log_debug("Dropping delivery notification. Entity is stopped");
    return;
  }

  handle_delivery_notification_impl(notif_sn, false);
}

void pdcp_entity_tx::handle_delivery_notification_impl(uint32_t notif_sn, bool is_retx)
{
  logger.log_debug("Handling delivery notification for notif_sn={} is_retx={}", notif_sn, is_retx);
  if (notif_sn >= pdcp_sn_cardinality(cfg.sn_size)) {
    logger.log_error(
        "Invalid delivery notification for notif_sn={} exceeds sn_size={}. is_retx={}", notif_sn, cfg.sn_size, is_retx);
    return;
  }
  uint32_t notif_count = notification_count_estimation(notif_sn);
  if (notif_count >= st.tx_next) {
    logger.log_error("Got notification for invalid COUNT. notif_count={} is_retx={} {}", notif_count, is_retx, st);
    return;
  }

  // Stop discard timers if required
  if (!cfg.discard_timer.has_value()) {
    return;
  }

  if (is_am()) {
    stop_discard_timer(notif_count);
  } else {
    logger.log_error(
        "Ignored unexpected PDU delivery notification in UM bearer. notif_sn={} is_retx={}", notif_sn, is_retx);
  }
}

void pdcp_entity_tx::handle_retransmit_notification(uint32_t notif_sn)
{
  if (SRSRAN_UNLIKELY(stopped)) {
    logger.log_debug("Dropping retransmit notification. Entity is stopped");
    return;
  }

  if (SRSRAN_UNLIKELY(is_srb())) {
    logger.log_error("Ignored unexpected PDU retransmit notification in SRB. notif_sn={}", notif_sn);
    return;
  }
  if (SRSRAN_UNLIKELY(is_um())) {
    logger.log_error("Ignored unexpected PDU retransmit notification in UM bearer. notif_sn={}", notif_sn);
    return;
  }

  handle_transmit_notification_impl(notif_sn, true);
}

void pdcp_entity_tx::handle_delivery_retransmitted_notification(uint32_t notif_sn)
{
  if (SRSRAN_UNLIKELY(stopped)) {
    logger.log_debug("Dropping delivery retransmitted notification. Entity is stopped");
    return;
  }

  if (SRSRAN_UNLIKELY(is_srb())) {
    logger.log_error("Ignored unexpected PDU delivery retransmitted notification in SRB. notif_sn={}", notif_sn);
    return;
  }
  if (SRSRAN_UNLIKELY(is_um())) {
    logger.log_error("Ignored unexpected PDU delivery retransmitted notification in UM bearer. notif_sn={}", notif_sn);
    return;
  }

  handle_delivery_notification_impl(notif_sn, true);
}

void pdcp_entity_tx::handle_desired_buffer_size_notification(uint32_t desired_bs)
{
  if (SRSRAN_UNLIKELY(stopped)) {
    logger.log_debug("Dropping desired buffer size notification. Entity is stopped");
    return;
  }
  desired_buffer_size = desired_bs;
}

uint32_t pdcp_entity_tx::notification_count_estimation(uint32_t notification_sn) const
{
  // Get lower edge of the window. If discard timer is enabled, use the lower edge of the tx_window, i.e. TX_NEXT_ACK.
  // If discard timer is not configured, use TX_TRANS as lower edge of window.
  uint32_t tx_lower;
  if (cfg.discard_timer.has_value()) {
    tx_lower = st.tx_next_ack;
  } else {
    tx_lower = st.tx_trans;
  }

  /*
   * Calculate NOTIFICATION_COUNT. This is adapted from TS 38.331 Sec. 5.2.2 "Receive operation" of the Rx side.
   *
   * - if NOTIFICATION_SN < SN(TX_LOWER) – Window_Size:
   *   - NOTIFICATION_HFN = HFN(TX_LOWER) + 1.
   * - else if NOTIFICATION_SN >= SN(TX_LOWER) + Window_Size:
   *   - NOTIFICATION_HFN = HFN(TX_LOWER) – 1.
   * - else:
   *   - NOTIFICATION_HFN = HFN(TX_LOWER);
   * - NOTIFICATION_COUNT = [NOTIFICATION_HFN, NOTIFICATION_SN].
   */
  uint32_t notification_hfn;
  if ((int64_t)notification_sn < (int64_t)SN(tx_lower) - (int64_t)window_size) {
    notification_hfn = HFN(tx_lower) + 1;
  } else if (notification_sn >= SN(tx_lower) + window_size) {
    notification_hfn = HFN(tx_lower) - 1;
  } else {
    notification_hfn = HFN(tx_lower);
  }
  return COUNT(notification_hfn, notification_sn);
}

/*
 * PDU Helpers
 */
bool pdcp_entity_tx::write_data_pdu_header(byte_buffer& buf, const pdcp_data_pdu_header& hdr) const
{
  // Sanity check: 18-bit SN not allowed for SRBs
  srsran_assert(
      !(is_srb() && cfg.sn_size == pdcp_sn_size::size18bits), "Invalid SN size for SRB. sn_size={}", cfg.sn_size);

  unsigned hdr_len = cfg.sn_size == pdcp_sn_size::size12bits ? 2 : 3;
  auto     view    = buf.reserve_prepend(cfg.sn_size == pdcp_sn_size::size12bits ? 2 : 3);
  if (view.length() != hdr_len) {
    logger.log_error("Not enough space to write header. sn_size={}", cfg.sn_size);
    return false;
  }

  byte_buffer::iterator hdr_writer = buf.begin();
  if (hdr_writer == buf.end()) {
    logger.log_error("Not enough space to write header. sn_size={}", cfg.sn_size);
  }

  // Set D/C if required
  if (is_drb()) {
    // D/C bit field (1).
    *hdr_writer = 0x80;
  } else {
    // No D/C bit field.
    *hdr_writer = 0x00;
  }

  // Add SN
  switch (cfg.sn_size) {
    case pdcp_sn_size::size12bits:
      *hdr_writer |= (hdr.sn & 0x00000f00U) >> 8U;
      hdr_writer++;
      if (hdr_writer == buf.end()) {
        logger.log_error("Not enough space to write header. sn_size={}", cfg.sn_size);
      }
      *hdr_writer = hdr.sn & 0x000000ffU;
      break;
    case pdcp_sn_size::size18bits:
      *hdr_writer |= (hdr.sn & 0x00030000U) >> 16U;
      hdr_writer++;
      if (hdr_writer == buf.end()) {
        logger.log_error("Not enough space to write header. sn_size={}", cfg.sn_size);
      }
      *hdr_writer = (hdr.sn & 0x0000ff00U) >> 8U;
      hdr_writer++;
      if (hdr_writer == buf.end()) {
        logger.log_error("Not enough space to write header. sn_size={}", cfg.sn_size);
      }
      *hdr_writer = hdr.sn & 0x000000ffU;
      break;
    default:
      logger.log_error("Invalid sn_size={}", cfg.sn_size);
      return false;
  }
  return true;
}

uint32_t pdcp_entity_tx::get_pdu_size(const byte_buffer& sdu) const
{
  return pdcp_data_header_size(sn_size) + sdu.length() + (integrity_enabled == security::integrity_enabled::on ? 4 : 0);
}

/*
 * Timers
 */
void pdcp_entity_tx::stop_discard_timer(uint32_t highest_count)
{
  if (!cfg.discard_timer.has_value()) {
    logger.log_debug("Cannot stop discard timers. No discard timer configured. highest_count={}", highest_count);
    return;
  }

  // Transmission or delivery notification arrived for a COUNT that is outside of the TX_WINDOW.
  // This can happen if the notification arrived after the discard timer has expired.
  if (highest_count < st.tx_next_ack || highest_count >= st.tx_next) {
    logger.log_debug("Cannot stop discard timers. highest_count={} is outside tx_window. {}", highest_count, st);
    return;
  }
  logger.log_debug("Stopping discard timers. highest_count={}", highest_count);

  // Get oldest PDU time of arrival.
  if (not tx_window.has_sn(st.tx_next_ack)) {
    logger.log_error(
        "Trying to stop discard timers, but TX_NEXT_ACK not in TX window. highest_count={} st={} tx_window_size={}",
        highest_count,
        st,
        tx_window.get_nof_sdus());
    return;
  }

  // Stop discard timers and update TX_NEXT_ACK to oldest element in tx_window
  discard_timer.stop();
  while (st.tx_next_ack <= highest_count) {
    if (tx_window.has_sn(st.tx_next_ack)) {
      tx_window.remove_sdu(st.tx_next_ack);
      logger.log_debug("Stopped discard timer. count={}", st.tx_next_ack);
    }
    st.tx_next_ack++;
  }

  // Update TX_TRANS if it falls out of the tx_window
  st.tx_trans = std::max(st.tx_trans, st.tx_next_ack);

  // Restart discard timer if requrired.
  if (cfg.discard_timer.value() == pdcp_discard_timer::infinity) {
    return;
  }

  // There are still old PDUs, restart discard timer.
  if (st.tx_next_ack != st.tx_next) {
    auto& window_elem = tx_window[st.tx_next_ack];
    srsran_assert(window_elem.sdu_info.tick_point_of_arrival.has_value(),
                  "Cannot update discard timer for SDU without arrival time. count={}",
                  window_elem.sdu_info.count);
    tick_point_t now = discard_timer.now();
    unsigned     new_timeout =
        window_elem.sdu_info.tick_point_of_arrival.value() + (unsigned)cfg.discard_timer.value() - now;
    discard_timer.set(std::chrono::milliseconds(new_timeout), [this](timer_id_t timer_id) { discard_callback(); });
    discard_timer.run();
  }
}

void pdcp_entity_tx::discard_pdu(uint32_t count)
{
  if (!cfg.discard_timer.has_value()) {
    logger.log_debug("Cannot discard PDU. No discard timer configured. count={}", count);
    return;
  }
  if (count < st.tx_next_ack || count >= st.tx_next) {
    logger.log_warning("Cannot discard PDU. The PDU is outside tx_window. count={} {}", count, st);
    return;
  }
  if (!tx_window.has_sn(count)) {
    logger.log_warning("Cannot discard PDU. The PDU is missing in tx_window. count={} {}", count, st);
    return;
  }
  logger.log_debug("Discarding PDU. count={}", count);

  // Notify lower layers of the discard. It's the RLC to actually discard, if no segment was transmitted yet.
  lower_dn.on_discard_pdu(SN(count));

  tx_window.remove_sdu(count);

  // Update TX_NEXT_ACK to oldest element in tx_window
  while (st.tx_next_ack < st.tx_next && !tx_window.has_sn(st.tx_next_ack)) {
    st.tx_next_ack++;
  }

  // Update TX_TRANS if it falls out of the tx_window
  st.tx_trans = std::max(st.tx_trans, st.tx_next_ack);
}

// Discard Timer Callback (discardTimer)
void pdcp_entity_tx::discard_callback()
{
  if (stopped) {
    logger.log_debug("Discard timer expired after bearer was stopped. st={}", st);
    return;
  }
  logger.log_debug("Discard timer expired. st={}", st);

  // Add discard to metrics.
  metrics.add_discard_timouts(1);

  // Sanity check oldest PDU.
  if (not tx_window.has_sn(st.tx_next_ack)) {
    logger.log_error("Discard timer expired, but oldest PDU not in TX window. st={}", st);
    return;
  }

  // Discard all PDUs that match the discard timer tick.
  pdcp_tx_window_element& oldest_window_elem = tx_window[st.tx_next_ack];
  srsran_assert(oldest_window_elem.sdu_info.tick_point_of_arrival.has_value(),
                "Cannot determine oldest time point in discard callback: SDU without arrival time. count={}",
                oldest_window_elem.sdu_info.count);
  tick_point_t oldest_timepoint = oldest_window_elem.sdu_info.tick_point_of_arrival.value();
  do {
    discard_pdu(st.tx_next_ack); // this updates st.tx_next_ack to the oldest PDU still in the window.
    if (not tx_window.has_sn(st.tx_next_ack)) {
      logger.log_debug("Finished discard callback. There are no new PDUs. st={}", st);
      break;
    }
    pdcp_tx_window_element& window_elem = tx_window[st.tx_next_ack];
    srsran_assert(window_elem.sdu_info.tick_point_of_arrival.has_value(),
                  "Cannot update discard timer for SDU without arrival time. count={}",
                  window_elem.sdu_info.count);
    if (window_elem.sdu_info.tick_point_of_arrival != oldest_timepoint) {
      // Restart timeout for any pending SDUs.
      unsigned new_timeout = (window_elem.sdu_info.tick_point_of_arrival.value() - oldest_timepoint);
      logger.log_debug("Finished discard callback. There are new PDUs with a new discard timer. new_timeout={}, st={}",
                       new_timeout,
                       st);
      discard_timer.set(std::chrono::milliseconds(new_timeout), [this](timer_id_t timer_id) { discard_callback(); });
      discard_timer.run();
      break;
    }
  } while (st.tx_next_ack != st.tx_next);
}

// Crypto reordering timeout
void pdcp_entity_tx::crypto_reordering_timeout()
{
  if (stopped) {
    logger.log_debug("Crypto reordering timer expired after bearer was stopped. timeout={} st={}",
                     pdcp_tx_crypto_reordering_timeout_ms,
                     st);
    return;
  }

  if (cfg.custom.warn_on_drop) {
    logger.log_warning("Crypto reordering timer expired. timeout={} st={}", pdcp_tx_crypto_reordering_timeout_ms, st);
  } else {
    logger.log_debug("Crypto reordering timer expired. timeout={} st={}", pdcp_tx_crypto_reordering_timeout_ms, st);
  }

  // Advance the TX_TRANS_CRYPTO to TX_REORD_CRYPTO.
  // Deliver all processed PDUs up until TX_REORD_CRYPTO.
  while (st.tx_trans_crypto != st.tx_reord_crypto) {
    if (tx_window.has_sn(st.tx_trans_crypto) && not tx_window[st.tx_trans_crypto].pdu.empty()) {
      pdcp_tx_pdu_info pdu_info{.pdu     = std::move(tx_window[st.tx_trans_crypto].pdu),
                                .count   = st.tx_trans_crypto,
                                .sdu_toa = tx_window[st.tx_trans_crypto].sdu_info.time_of_arrival};
      write_data_pdu_to_lower_layers(std::move(pdu_info), false);
    } else {
      logger.log_debug("Dropping SDU. count={}", st.tx_trans_crypto);
    }
    // Update RX_DELIV
    st.tx_trans_crypto = st.tx_trans_crypto + 1;
  }

  // Deliver in order PDUs. Break and update TX_TRANS_CRYPTO when we find a missing PDU.
  for (uint32_t count = st.tx_trans_crypto; count < st.tx_next && not tx_window[count].pdu.empty(); count++) {
    pdcp_tx_pdu_info pdu_info{
        .pdu = std::move(tx_window[count].pdu), .count = count, .sdu_toa = tx_window[count].sdu_info.time_of_arrival};
    write_data_pdu_to_lower_layers(std::move(pdu_info), false);
    st.tx_trans_crypto = count + 1;
    // Automatically trigger delivery notifications when using test mode
    if (cfg.custom.test_mode) {
      handle_transmit_notification(SN(count));
    }
  }

  // Update TX_CRYPTO_REORD to TX_NEXT if there are still PDUs that need to be processed.
  if (st.tx_trans_crypto < st.tx_next) {
    logger.log_debug("Updating tx_trans_crypto to tx_next. {}", st);
    st.tx_reord_crypto = st.tx_next;
    crypto_reordering_timer.run();
  }
}

pdcp_entity_tx::early_drop_reason pdcp_entity_tx::check_early_drop(const byte_buffer& buf)
{
  if (desired_buffer_size == 0) {
    return early_drop_reason::zero_dbs;
  }
  uint32_t pdu_size            = get_pdu_size(buf);
  uint32_t updated_buffer_size = tx_window.get_pdu_bytes(integrity_enabled) + pdu_size;
  if (updated_buffer_size > desired_buffer_size) {
    return early_drop_reason::full_rlc_queue;
  }
  if ((st.tx_next - st.tx_next_ack) >= (window_size - 1)) {
    return early_drop_reason::full_window;
  }
  return early_drop_reason::no_drop;
}
