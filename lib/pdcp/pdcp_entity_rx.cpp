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

#include "pdcp_entity_rx.h"
#include "../security/security_engine_impl.h"
#include "srsran/instrumentation/traces/up_traces.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/executors/execution_context_description.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"

using namespace srsran;

pdcp_entity_rx::pdcp_entity_rx(uint32_t                        ue_index,
                               rb_id_t                         rb_id_,
                               pdcp_rx_config                  cfg_,
                               pdcp_rx_upper_data_notifier&    upper_dn_,
                               pdcp_rx_upper_control_notifier& upper_cn_,
                               timer_factory                   ue_ul_timer_factory_,
                               task_executor&                  ue_ul_executor_,
                               task_executor&                  crypto_executor_,
                               uint32_t                        max_nof_crypto_workers_,
                               pdcp_metrics_aggregator&        metrics_agg_) :
  pdcp_entity_tx_rx_base(rb_id_, cfg_.rb_type, cfg_.rlc_mode, cfg_.sn_size),
  logger("PDCP", {ue_index, rb_id_, "UL"}),
  cfg(cfg_),
  rx_window(logger, pdcp_window_size(pdcp_sn_size_to_uint(cfg.sn_size))),
  upper_dn(upper_dn_),
  upper_cn(upper_cn_),
  ue_ul_timer_factory(ue_ul_timer_factory_),
  ue_ul_executor(ue_ul_executor_),
  crypto_executor(crypto_executor_),
  max_nof_crypto_workers(max_nof_crypto_workers_),
  metrics(metrics_agg_.get_metrics_period().count()),
  metrics_agg(metrics_agg_)
{
  if (metrics_agg.get_metrics_period().count()) {
    metrics_timer = ue_ul_timer_factory.create_timer();
    metrics_timer.set(std::chrono::milliseconds(metrics_agg.get_metrics_period().count()), [this](timer_id_t tid) {
      if (stopped) {
        return;
      }
      metrics_agg.push_rx_metrics(metrics.get_metrics_and_reset());
      metrics_timer.run();
    });
    metrics_timer.run();
  }
  // t-Reordering timer
  if (cfg.t_reordering != pdcp_t_reordering::ms0 && cfg.t_reordering != pdcp_t_reordering::infinity) {
    reordering_timer = ue_ul_timer_factory.create_timer();
    if (static_cast<uint32_t>(cfg.t_reordering) > 0) {
      reordering_timer.set(std::chrono::milliseconds{static_cast<unsigned>(cfg.t_reordering)},
                           reordering_callback{this});
    }
  }
  if (cfg.rb_type == pdcp_rb_type::drb && cfg.t_reordering == pdcp_t_reordering::infinity) {
    logger.log_warning("t-Reordering of infinity on DRBs is not advised. It can cause data stalls.");
  }
  logger.log_info("PDCP configured. {}", cfg);

  // Populate null security engines
  sec_engine_pool.reserve(max_nof_crypto_workers);
  for (uint32_t i = 0; i < max_nof_crypto_workers; i++) {
    std::unique_ptr<security::security_engine_impl> null_engine;
    sec_engine_pool.push_back(std::move(null_engine));
  }
}

pdcp_entity_rx::~pdcp_entity_rx()
{
  stop();
}

void pdcp_entity_rx::stop()
{
  if (not stopped) {
    stopped = true;
    reordering_timer.stop();
    token_mngr.stop();
    metrics_timer.stop();
    logger.log_debug("Stopped PDCP entity");
  }
}

void pdcp_entity_rx::notify_pdu_processing_stopped()
{
  if (not stopped) {
    token_mngr.stop();
    logger.log_debug("Stopped PDCP entity PDU processing");
  }
}

void pdcp_entity_rx::restart_pdu_processing()
{
  if (not stopped) {
    token_mngr.start();
    logger.log_debug("Started PDCP entity PDU processing");
  }
}

manual_event_flag& pdcp_entity_rx::crypto_awaitable()
{
  return token_mngr.get_awaitable();
}

void pdcp_entity_rx::handle_pdu(byte_buffer_chain buf)
{
  if (stopped) {
    logger.log_info("Dropping PDU. Entity is stopped");
    return;
  }

  trace_point rx_tp = up_tracer.now();
  metrics.add_pdus(1, buf.length());
  std::chrono::system_clock::time_point time_of_arrival = std::chrono::high_resolution_clock::now();

  // Log PDU
  logger.log_debug(buf.begin(), buf.end(), "RX PDU. pdu_len={}", buf.length());
  // Sanity check
  if (buf.empty()) {
    metrics.add_dropped_pdus(1);
    logger.log_error("Dropping empty PDU.");
    return;
  }

  auto pdu_copy = buf.deep_copy();
  if (not pdu_copy.has_value()) {
    metrics.add_dropped_pdus(1);
    logger.log_error("Dropping PDU: Copy failed. pdu_len={}", buf.length());
    return;
  }
  byte_buffer pdu = std::move(pdu_copy.value());

  pdcp_dc_field dc = pdcp_pdu_get_dc(*(pdu.begin()));
  if (is_srb() || dc == pdcp_dc_field::data) {
    handle_data_pdu(std::move(pdu), time_of_arrival);
  } else {
    handle_control_pdu(std::move(buf));
  }
  up_tracer << trace_event{"pdcp_rx_pdu", rx_tp};
}

void pdcp_entity_rx::reestablish(security::sec_128_as_config sec_cfg)
{
  // - process the PDCP Data PDUs that are received from lower layers due to the re-establishment of the lower layers,
  //   as specified in the clause 5.2.2.1;

  // - for SRBs, discard all stored PDCP SDUs and PDCP PDUs;
  if (is_srb()) {
    discard_all_sdus();
  }

  // - for SRBs and UM DRBs, if t-Reordering is running:
  //   - stop and reset t-Reordering;
  //   - for UM DRBs, deliver all stored PDCP SDUs to the upper layers in ascending order of associated COUNT
  //     values after performing header decompression;
  if (is_srb() || is_um()) {
    if (reordering_timer.is_running()) {
      reordering_timer.stop();
    }
    if (is_um()) {
      deliver_all_sdus();
    }
  }

  // - for AM DRBs for Uu interface, perform header decompression using ROHC for all stored PDCP SDUs if drb-
  //   ContinueROHC is not configured in TS 38.331 [3];
  // - for AM DRBs for PC5 interface, perform header decompression using ROHC for all stored PDCP IP SDUs;
  // - for AM DRBs for Uu interface, perform header decompression using EHC for all stored PDCP SDUs if drb-
  //   ContinueEHC-DL is not configured in TS 38.331 [3];
  // - for UM DRBs and AM DRBs, reset the ROHC protocol for downlink and start with NC state in U-mode (as
  //   defined in RFC 3095 [8] and RFC 4815 [9]) if drb-ContinueROHC is not configured in TS 38.331 [3];
  // - for UM DRBs and AM DRBs, reset the EHC protocol for downlink if drb-ContinueEHC-DL is not configured in
  //   TS 38.331 [3];
  // TODO header compression not supported yet.

  // - for UM DRBs and SRBs, set RX_NEXT and RX_DELIV to the initial value;
  if (is_srb() || is_um()) {
    st = {};
  }

  // - apply the ciphering algorithm and key provided by upper layers during the PDCP entity re-establishment
  //   procedure;
  // - apply the integrity protection algorithm and key provided by upper layers during the PDCP entity re-
  //   establishment procedure.
  configure_security(sec_cfg, integrity_enabled, ciphering_enabled);
}

void pdcp_entity_rx::handle_data_pdu(byte_buffer pdu, std::chrono::system_clock::time_point time_of_arrival)
{
  // Count all received data PDUs.
  metrics.add_data_pdus(1, pdu.length());

  // Sanity check
  if (pdu.length() <= hdr_len_bytes) {
    metrics.add_dropped_pdus(1);
    logger.log_error(pdu.begin(), pdu.end(), "RX PDU too small. pdu_len={} hdr_len={}", pdu.length(), hdr_len_bytes);
    return;
  }
  // Log state
  log_state(srslog::basic_levels::debug);

  // Unpack header
  pdcp_data_pdu_header hdr = {};
  if (not read_data_pdu_header(hdr, pdu)) {
    metrics.add_dropped_pdus(1);
    logger.log_error(
        pdu.begin(), pdu.end(), "Failed to extract SN. pdu_len={} hdr_len={}", pdu.length(), hdr_len_bytes);
    return;
  }

  /*
   * Calculate RCVD_COUNT:
   *
   * - if RCVD_SN < SN(RX_DELIV) – Window_Size:
   *   - RCVD_HFN = HFN(RX_DELIV) + 1.
   * - else if RCVD_SN >= SN(RX_DELIV) + Window_Size:
   *   - RCVD_HFN = HFN(RX_DELIV) – 1.
   * - else:
   *   - RCVD_HFN = HFN(RX_DELIV);
   * - RCVD_COUNT = [RCVD_HFN, RCVD_SN].
   */
  uint32_t rcvd_hfn;
  uint32_t rcvd_count;
  if ((int64_t)hdr.sn < (int64_t)SN(st.rx_deliv) - (int64_t)window_size) {
    rcvd_hfn = HFN(st.rx_deliv) + 1;
  } else if (hdr.sn >= SN(st.rx_deliv) + window_size) {
    rcvd_hfn = HFN(st.rx_deliv) - 1;
  } else {
    rcvd_hfn = HFN(st.rx_deliv);
  }
  rcvd_count = COUNT(rcvd_hfn, hdr.sn);

  logger.log_info(
      pdu.begin(), pdu.end(), "RX PDU. type=data pdu_len={} sn={} count={}", pdu.length(), hdr.sn, rcvd_count);

  // The PDCP is not allowed to use the same COUNT value more than once for a given security key,
  // see TS 38.331, section 5.3.1.2. To avoid this, we notify the RRC once we exceed a "maximum"
  // notification COUNT. It is then the RRC's responsibility to refresh the keys. We continue receiving until
  // we reach a hard maximum RCVD_COUNT, after which we refuse to receive any further.
  if (rcvd_count > cfg.custom.max_count.notify) {
    if (!max_count_notified) {
      logger.log_warning("Approaching max_count, notifying RRC. count={}", rcvd_count);
      upper_cn.on_max_count_reached();
      max_count_notified = true;
    }
  }
  if (rcvd_count >= cfg.custom.max_count.hard) {
    if (!max_count_overflow) {
      logger.log_error("Reached max_count, refusing further RX. count={}", rcvd_count);
      upper_cn.on_protocol_failure();
      max_count_overflow = true;
    }
    return;
  }

  pdcp_rx_pdu_info pdu_info{.buf             = std::move(pdu),
                            .count           = rcvd_count,
                            .time_of_arrival = time_of_arrival,
                            .token           = pdcp_crypto_token(token_mngr)};

  // apply security in crypto executor
  if (not crypto_executor.execute([this, pdu_info = std::move(pdu_info)]() mutable {
        apply_security(std::move(pdu_info)); // we should not use the PDCP entity past this point, as we possibly no
                                             // longer hold the crypto token causing races upon deletion.
      })) {
    logger.log_warning("Dropped PDU, crypto executor queue is full. count={}", rcvd_count);
  }
}

void pdcp_entity_rx::apply_security(pdcp_rx_pdu_info&& pdu_info)
{
  auto     pre        = std::chrono::high_resolution_clock::now();
  uint32_t rcvd_count = pdu_info.count;

  // Apply deciphering and integrity check
  security::security_result result = apply_deciphering_and_integrity_check(std::move(pdu_info.buf), rcvd_count);

  if (!result.buf.has_value()) {
    auto handle_failure =
        [this, sec_err = result.buf.error(), count = result.count, token = std::move(pdu_info.token)]() {
          switch (sec_err) {
            case srsran::security::security_error::integrity_failure:
              logger.log_warning("Integrity failed, dropping PDU. count={}", count);
              metrics.add_integrity_failed_pdus(1);
              upper_cn.on_integrity_failure();
              break;
            case srsran::security::security_error::ciphering_failure:
              logger.log_warning("Deciphering failed, dropping PDU. count={}", count);
              upper_cn.on_protocol_failure();
              break;
            case srsran::security::security_error::buffer_failure:
              logger.log_error("Buffer error when decrypting and verifying integrity, dropping PDU. count={}", count);
              upper_cn.on_protocol_failure();
              break;
            case srsran::security::security_error::engine_failure:
              logger.log_error("Engine error when decrypting and verifying integrity, dropping PDU. count={}", count);
              upper_cn.on_protocol_failure();
              break;
          }
        };
    if (not ue_ul_executor.execute(std::move(handle_failure))) {
      logger.log_warning("Dropped PDU with security error, UE executor queue is full. count={} sec_err={}",
                         rcvd_count,
                         result.buf.error());
    }
    return;
  }
  logger.log_debug(result.buf.value().begin(), result.buf.value().end(), "Security passed. count={}", rcvd_count);

  // After checking the integrity, we can discard the header.
  unsigned hdr_size = cfg.sn_size == pdcp_sn_size::size12bits ? 2 : 3;
  result.buf.value().trim_head(hdr_size);

  pdu_info.buf = std::move(result.buf.value());

  auto post           = std::chrono::high_resolution_clock::now();
  auto sdu_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(post - pre);
  metrics.add_crypto_processing_latency(sdu_latency_ns.count());

  // apply reordering in UE executor
  if (not ue_ul_executor.execute([this, pdu_info = std::move(pdu_info)]() mutable {
        metrics.add_integrity_verified_pdus(1);
        apply_reordering(std::move(pdu_info));
      })) {
    logger.log_warning("Dropped PDU, UE executor queue is full. count={}", rcvd_count);
  }
}

void pdcp_entity_rx::apply_reordering(pdcp_rx_pdu_info pdu_info)
{
  uint32_t rcvd_count = pdu_info.count;
  /*
   * Check valid rcvd_count:
   *
   * - if RCVD_COUNT < RX_DELIV; or
   * - if the PDCP Data PDU with COUNT = RCVD_COUNT has been received before:
   *   - discard the PDCP Data PDU;
   */
  if (rcvd_count < st.rx_deliv) {
    logger.log_debug("Out-of-order after timeout, duplicate or count wrap-around. count={} {}", rcvd_count, st);
    return; // Invalid count, drop.
  }

  // Check if PDU has been received
  if (rx_window.has_sn(rcvd_count)) {
    const pdcp_rx_sdu_info& sdu_info = rx_window[rcvd_count];
    if (sdu_info.count == rcvd_count) {
      logger.log_debug("Duplicate PDU dropped. count={}", rcvd_count);
      return; // PDU already present, drop.
    }
    logger.log_error("Removing old PDU with count={} for new PDU with count={}", sdu_info.count, rcvd_count);
    rx_window.remove_sn(rcvd_count);
  }

  // Store PDU in Rx window
  pdcp_rx_sdu_info& sdu_info = rx_window.add_sn(rcvd_count);
  sdu_info.buf               = std::move(pdu_info.buf);
  sdu_info.time_of_arrival   = pdu_info.time_of_arrival;

  // Update RX_NEXT
  if (rcvd_count >= st.rx_next) {
    st.rx_next = rcvd_count + 1;
  }

  // TODO if out-of-order configured, submit to upper layer
  // /!\ Caution: reorder_queue is used to build status report:
  //     For out-of-order:
  //     - store empty buffers there
  //     - clean upon each rx'ed PDU
  //     - don't forward empty buffer to upper layers

  if (rcvd_count == st.rx_deliv) {
    // Deliver to upper layers in ascending order of associated COUNT
    deliver_all_consecutive_counts();
  }

  // Handle reordering timers
  if (reordering_timer.is_running() and st.rx_deliv >= st.rx_reord) {
    reordering_timer.stop();
    logger.log_debug("Stopped t-Reordering.", st);
  }

  if (cfg.t_reordering != pdcp_t_reordering::infinity) {
    if (cfg.t_reordering == pdcp_t_reordering::ms0) {
      st.rx_reord = st.rx_next;
      handle_t_reordering_expire();
    } else if (not reordering_timer.is_running() and st.rx_deliv < st.rx_next) {
      st.rx_reord = st.rx_next;
      reordering_timer.run();
      logger.log_debug("Started t-Reordering.");
    }
  }

  // Log state
  log_state(srslog::basic_levels::debug);
}

void pdcp_entity_rx::handle_control_pdu(byte_buffer_chain pdu)
{
  // Read and verify PDU header (first byte)
  uint8_t hdr_byte = *pdu.begin();

  // Assert control PDU
  pdcp_dc_field dc = pdcp_pdu_get_dc(hdr_byte);
  srsran_assert(dc == pdcp_dc_field::control, "Invalid D/C field in control PDU. dc={}", dc);

  // Switch control PDU type (CPT)
  pdcp_control_pdu_header control_hdr = {};
  control_hdr.cpt                     = pdcp_control_pdu_get_cpt(hdr_byte);
  switch (control_hdr.cpt) {
    case pdcp_control_pdu_type::status_report:
      status_handler->on_status_report(std::move(pdu));
      break;
    default:
      logger.log_error(pdu.begin(), pdu.end(), "Unsupported control PDU type. {}", control_hdr);
  }
}

// Deliver all consecutively associated COUNTs.
// Update RX_DELIV after submitting to higher layers
void pdcp_entity_rx::deliver_all_consecutive_counts()
{
  while (st.rx_deliv != st.rx_next && rx_window.has_sn(st.rx_deliv)) {
    pdcp_rx_sdu_info& sdu_info = rx_window[st.rx_deliv];
    logger.log_info("RX SDU. count={}", st.rx_deliv);

    // Pass PDCP SDU to the upper layers
    metrics.add_sdus(1, sdu_info.buf.length());
    record_reordering_dealy(sdu_info.time_of_arrival);
    auto sdu_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now() - sdu_info.time_of_arrival);
    metrics.add_sdu_latency_ns(sdu_latency_ns.count());
    upper_dn.on_new_sdu(std::move(sdu_info.buf));
    rx_window.remove_sn(st.rx_deliv);

    // Update RX_DELIV
    st.rx_deliv = st.rx_deliv + 1;
  }
}

// Deliver all RX'ed SDUs, regardless of order. Used during re-establishment.
// No need to update RX_DELIV, as the re-establishment procedure will be responsible
// for updating the state.
void pdcp_entity_rx::deliver_all_sdus()
{
  for (uint32_t count = st.rx_deliv; count < st.rx_next; count++) {
    if (rx_window.has_sn(count)) {
      pdcp_rx_sdu_info& sdu_info = rx_window[count];
      logger.log_info("RX SDU. count={}", count);

      // Pass PDCP SDU to the upper layers
      metrics.add_sdus(1, sdu_info.buf.length());
      record_reordering_dealy(sdu_info.time_of_arrival);
      auto sdu_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now() - sdu_info.time_of_arrival);
      metrics.add_sdu_latency_ns(sdu_latency_ns.count());
      upper_dn.on_new_sdu(std::move(sdu_info.buf));
      rx_window.remove_sn(count);
    }
  }
}

// Discard all SDUs.
void pdcp_entity_rx::discard_all_sdus()
{
  while (st.rx_deliv != st.rx_next) {
    if (rx_window.has_sn(st.rx_deliv)) {
      rx_window.remove_sn(st.rx_deliv);
      logger.log_debug("Discarded RX SDU. count={}", st.rx_next);
    }

    // Update RX_DELIV
    st.rx_deliv = st.rx_deliv + 1;
  }
}

byte_buffer pdcp_entity_rx::compile_status_report()
{
  byte_buffer buf = {};
  bit_encoder enc(buf);

  // Pack PDU header
  enc.pack(to_number(pdcp_dc_field::control), 1);
  enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
  enc.pack(0b0000, 4);

  // Pack RX_DELIV into FMC field
  enc.pack(st.rx_deliv, 32);

  // Set bitmap boundaries, ensure to not exceed max control PDU size (9000 Bytes)
  constexpr uint32_t max_bits     = (pdcp_control_pdu_max_size - 5) * 8;
  uint32_t           bitmap_begin = st.rx_deliv + 1; // Bitmap starts from FMC+1
  uint32_t           bitmap_end   = st.rx_next;
  if (bitmap_begin < bitmap_end && bitmap_end - bitmap_begin > max_bits) {
    bitmap_end = bitmap_begin + max_bits;
  }

  // Pack bitmap
  for (uint32_t i = bitmap_begin; i < bitmap_end; i++) {
    // Bit == 0: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is missing.
    // Bit == 1: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is correctly received.
    unsigned bit = rx_window.has_sn(i) ? 1 : 0;
    enc.pack(bit, 1);
  }

  return buf;
}

/*
 * Deciphering and Integrity Protection Helpers
 */
security::security_result pdcp_entity_rx::apply_deciphering_and_integrity_check(byte_buffer buf, uint32_t count)
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

  security::security_engine_rx* sec_engine = sec_engine_pool[worker_idx].get();
  if (sec_engine == nullptr) {
    // Security is not configured. Pass through for DRBs; trim zero MAC-I for SRBs.
    if (is_srb()) {
      if (buf.length() <= security::sec_mac_len) {
        logger.log_warning("Failed to trim MAC-I from PDU. count={}", count);
        return {.buf = make_unexpected(srsran::security::security_error::buffer_failure), .count = count};
      }
      buf.trim_tail(security::sec_mac_len);
    }
    return {.buf = std::move(buf), .count = count};
  }

  // TS 38.323, section 5.8: Deciphering
  // The data unit that is ciphered is the MAC-I and the
  // data part of the PDCP Data PDU except the
  // SDAP header and the SDAP Control PDU if included in the PDCP SDU.

  // TS 38.323, section 5.9: Integrity verification
  // The data unit that is integrity protected is the PDU header
  // and the data part of the PDU before ciphering.

  unsigned                  hdr_size = cfg.sn_size == pdcp_sn_size::size12bits ? 2 : 3;
  security::security_result result   = sec_engine->decrypt_and_verify_integrity(std::move(buf), hdr_size, count);
  return result;
}

/*
 * Security configuration
 */
void pdcp_entity_rx::configure_security(security::sec_128_as_config sec_cfg,
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
    logger.log_info("128 K_int: {}", sec_cfg.k_128_int);
  }
  logger.log_info("128 K_enc: {}", sec_cfg.k_128_enc);
}

/*
 * Timers
 */
void pdcp_entity_rx::handle_t_reordering_expire()
{
  metrics.add_t_reordering_timeouts(1);
  // Deliver all PDCP SDU(s) with associated COUNT value(s) < RX_REORD
  while (st.rx_deliv != st.rx_reord) {
    if (rx_window.has_sn(st.rx_deliv)) {
      pdcp_rx_sdu_info& sdu_info = rx_window[st.rx_deliv];
      logger.log_info("RX SDU. count={}", st.rx_deliv);

      // Pass PDCP SDU to the upper layers
      metrics.add_sdus(1, sdu_info.buf.length());
      record_reordering_dealy(sdu_info.time_of_arrival);
      auto sdu_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now() - sdu_info.time_of_arrival);
      metrics.add_sdu_latency_ns(sdu_latency_ns.count());
      upper_dn.on_new_sdu(std::move(sdu_info.buf));
      rx_window.remove_sn(st.rx_deliv);
    }

    // Update RX_DELIV
    st.rx_deliv = st.rx_deliv + 1;
  }

  // Deliver all PDCP SDU(s) consecutively associated COUNT value(s) starting from RX_REORD
  deliver_all_consecutive_counts();

  // Log state
  log_state(srslog::basic_levels::debug);

  if (st.rx_deliv < st.rx_next) {
    if (cfg.t_reordering == pdcp_t_reordering::ms0) {
      logger.log_error("Reordering timer expired after 0ms and rx_deliv < rx_next. {}", st);
      return;
    }
    logger.log_debug("Updating rx_reord to rx_next. {}", st);
    st.rx_reord = st.rx_next;
    reordering_timer.run();
  }
}

// Reordering Timer Callback (t-reordering)
void pdcp_entity_rx::reordering_callback::operator()(timer_id_t /*timer_id*/)
{
  if (parent->stopped) {
    parent->logger.log_debug("Re-ordering timer expired after bearer was stopped.");
    return;
  }
  parent->logger.log_info("Reordering timer expired. {}", parent->st);
  parent->handle_t_reordering_expire();
}

/*
 * Header helpers
 */
bool pdcp_entity_rx::read_data_pdu_header(pdcp_data_pdu_header& hdr, const byte_buffer& buf) const
{
  // Check PDU is long enough to extract header
  if (buf.length() <= hdr_len_bytes) {
    logger.log_error("PDU too small to extract header. pdu_len={} hdr_len={}", buf.length(), hdr_len_bytes);
    return false;
  }

  byte_buffer::const_iterator buf_it = buf.begin();

  // Extract RCVD_SN
  switch (cfg.sn_size) {
    case pdcp_sn_size::size12bits:
      hdr.sn = (*buf_it & 0x0fU) << 8U; // first 4 bits SN
      ++buf_it;
      hdr.sn |= (*buf_it & 0xffU); // last 8 bits SN
      ++buf_it;
      break;
    case pdcp_sn_size::size18bits:
      hdr.sn = (*buf_it & 0x03U) << 16U; // first 2 bits SN
      ++buf_it;
      hdr.sn |= (*buf_it & 0xffU) << 8U; // middle 8 bits SN
      ++buf_it;
      hdr.sn |= (*buf_it & 0xffU); // last 8 bits SN
      ++buf_it;
      break;
    default:
      logger.log_error("Invalid SN size config. sn_size={}", cfg.sn_size);
      return false;
  }
  return true;
}

void pdcp_entity_rx::record_reordering_dealy(std::chrono::system_clock::time_point time_of_arrival)
{
  std::chrono::microseconds time_taken =
      std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - time_of_arrival);
  metrics.add_reordering_delay_us((uint32_t)time_taken.count());
}
