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

#include "pdcp_bearer_logger.h"
#include "pdcp_crypto_token.h"
#include "pdcp_entity_tx_rx_base.h"
#include "pdcp_interconnect.h"
#include "pdcp_metrics_aggregator.h"
#include "pdcp_pdu.h"
#include "pdcp_tx_metrics_impl.h"
#include "pdcp_tx_window.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/security/security.h"
#include "srsran/security/security_engine.h"
#include "srsran/support/timers.h"

namespace srsran {

/// PDCP TX state variables,
/// TS 38.323, section 7.1
struct pdcp_tx_state {
  /// This state variable indicates the COUNT value of the next PDCP SDU to be transmitted. The initial value is 0,
  /// except for SRBs configured with state variables continuation.
  uint32_t tx_next = 0;
  /// This state variable indicates the next COUNT value for which transmission is
  /// still pending. This is used for TX reordering when using parallel ciphering and integrity protection.
  /// NOTE: This is a custom state variable, not specified by the standard.
  uint32_t tx_trans_crypto = 0;
  /// This state variable indicates the COUNT value following the COUNT value associated with the PDCP Data
  /// PDU which triggered reordering. This is used for TX reordering when using parallel ciphering and
  /// integrity protection.
  /// NOTE: This is a custom state variable, not specified by the standard.
  uint32_t tx_reord_crypto = 0;
  /// This state variable indicates the next COUNT value for which we will
  /// receive a transmission notification from the F1/RLC. If TX_TRANS == TX_NEXT,
  /// it means we are not currently waiting for any TX notification.
  /// NOTE: This is a custom state variable, not specified by the standard.
  uint32_t tx_trans = 0;
  /// This state variable indicates the lower edge of the TX window, i.e. the COUNT value of the oldest PDCP SDU in the
  /// TX window for which the discard timer is expected to expire soonest. If TX_NEXT_ACK == TX_NEXT, it means we are
  /// not currently having any discard timers running.
  /// For UM bearers this value is the same as TX_TRANS.
  /// NOTE: This is a custom state variable, not specified by the standard.
  uint32_t tx_next_ack = 0;

  pdcp_tx_state(uint32_t tx_next_,
                uint32_t tx_trans_crypto_,
                uint32_t tx_reord_crypto_,
                uint32_t tx_trans_,
                uint32_t tx_next_ack_) :
    tx_next(tx_next_),
    tx_trans_crypto(tx_trans_crypto_),
    tx_reord_crypto(tx_reord_crypto_),
    tx_trans(tx_trans_),
    tx_next_ack(tx_next_ack_)
  {
  }

  bool operator==(const pdcp_tx_state& other) const
  {
    return tx_next == other.tx_next && tx_trans == other.tx_trans && tx_reord_crypto == other.tx_reord_crypto &&
           tx_trans_crypto == other.tx_trans_crypto && tx_next_ack == other.tx_next_ack;
  }
};

/// Helper struct to pass buffer to security functions.
struct pdcp_tx_buffer_info {
  bool              is_retx;
  uint16_t          retx_id; /// ID used to identify if PDU is out of date.
  uint32_t          count;   /// COUNT associated with this SDU/PDU.
  byte_buffer       buf;     /// In/Out parameter for SDU+Header/PDU.
  pdcp_crypto_token token;   /// Crypto token to count in-flight PDUs.
};

/// Helper struct to pass PDUs+metadata to the lower layers.
struct pdcp_tx_pdu_info {
  byte_buffer                           pdu;     /// Buffer for PDU.
  uint32_t                              count;   /// COUNT associated with this SDU/PDU.
  std::chrono::system_clock::time_point sdu_toa; /// Time of arrival of SDU.
};

/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_tx final : public pdcp_entity_tx_rx_base,
                             public pdcp_tx_status_handler,
                             public pdcp_tx_upper_data_interface,
                             public pdcp_tx_upper_control_interface,
                             public pdcp_tx_lower_interface
{
public:
  pdcp_entity_tx(uint32_t                        ue_index,
                 rb_id_t                         rb_id_,
                 pdcp_tx_config                  cfg_,
                 pdcp_tx_lower_notifier&         lower_dn_,
                 pdcp_tx_upper_control_notifier& upper_cn_,
                 timer_factory                   ue_ctrl_timer_factory_,
                 task_executor&                  ue_dl_executor_,
                 task_executor&                  crypto_executor_,
                 uint32_t                        max_nof_crypto_workers_,
                 pdcp_metrics_aggregator&        metrics_agg_);

  ~pdcp_entity_tx() override;

  /// \brief Stop handling SDUs and stop timers
  void stop();

  /// \brief Retrun awaitable to wait for crypto tasks to be
  /// finished.
  manual_event_flag& crypto_awaitable();

  void notify_pdu_processing_stopped() override;
  void restart_pdu_processing() override;

  /// \brief Triggers re-establishment as specified in TS 38.323, section 5.1.2
  void reestablish(security::sec_128_as_config sec_cfg) override;

  /// \brief Get the TX count for status transfer
  pdcp_count_info get_count() const override
  {
    pdcp_count_info count_info;
    uint32_t        count = st.tx_next;
    count_info.sn         = SN(count);
    count_info.hfn        = HFN(count);
    return count_info;
  }

  /// \brief Set the TX count for status transfer
  void set_count(pdcp_count_info count_info) override
  {
    uint32_t count = COUNT(count_info.hfn, count_info.sn);
    if (st.tx_next != 0 || st.tx_trans_crypto != 0 || st.tx_reord_crypto != 0 || st.tx_trans != 0 ||
        st.tx_next_ack != 0) {
      logger.log_warning("Status transfer applied to bearer with non-zero state. st={} count={}", st, count);
    }
    st = {count, count, count, count, count};
    logger.log_info("Setted PDCP TX state. {}", st);
  }

  // Tx/Rx interconnect
  void set_status_provider(pdcp_rx_status_provider* status_provider_) { status_provider = status_provider_; }

  /*
   * SDU/PDU handlers
   */
  void handle_sdu(byte_buffer buf) override;

  void handle_transmit_notification(uint32_t notif_sn) override;
  void handle_delivery_notification(uint32_t notif_sn) override;
  void handle_retransmit_notification(uint32_t notif_sn) override;
  void handle_delivery_retransmitted_notification(uint32_t notif_sn) override;
  void handle_desired_buffer_size_notification(uint32_t desired_bs) override;

  /// \brief Evaluates a PDCP status report
  ///
  /// Ref: Ref: TS 38.323, Sec. 5.4.2, Sec. 6.2.3.1 and Sec. 6.3.{9,10}
  ///
  /// \param status The status report
  void handle_status_report(byte_buffer_chain status);

  // Status handler interface
  void on_status_report(byte_buffer_chain status) override { handle_status_report(std::move(status)); }

  /*
   * Header helpers
   */

  /// \brief Writes the header of a PDCP data PDU according to the content of the associated object
  /// \param[out] buf Reference to a byte_buffer that is appended by the header bytes
  /// \param[in] hdr Reference to a pdcp_data_pdu_header that represents the header content
  [[nodiscard]] bool write_data_pdu_header(byte_buffer& buf, const pdcp_data_pdu_header& hdr) const;

  /*
   * Testing helpers
   */
  void set_state(pdcp_tx_state st_)
  {
    reset();
    st = st_;
  }

  const pdcp_tx_state& get_state() const { return st; }

  uint32_t nof_pdus_in_window() const { return st.tx_next - st.tx_next_ack; }

  /*
   * Security configuration
   */
  void configure_security(security::sec_128_as_config sec_cfg,
                          security::integrity_enabled integrity_enabled_,
                          security::ciphering_enabled ciphering_enabled_) override;

  /// Sends a status report, as specified in TS 38.323, Sec. 5.4.
  void send_status_report();

  /// Performs data recovery, as specified in TS 38.323, Sec. 5.5.
  void data_recovery() override;

  /// \brief Reset state variables to their initial state and drop all discard timers with all stored SDUs.
  void reset();

  /// Retransmits all PDUs. Integrity protection and ciphering is re-applied.
  void retransmit_all_pdus();

  /// Get metrics.
  pdcp_tx_metrics_container get_metrics_and_reset() { return metrics.get_metrics_and_reset(); }

  enum class early_drop_reason { zero_dbs, full_rlc_queue, full_window, no_drop };

private:
  pdcp_bearer_logger              logger;
  const pdcp_tx_config            cfg;
  bool                            stopped         = false;
  pdcp_rx_status_provider*        status_provider = nullptr;
  pdcp_tx_lower_notifier&         lower_dn;
  pdcp_tx_upper_control_notifier& upper_cn;
  timer_factory                   ue_ctrl_timer_factory;
  unique_timer                    discard_timer;
  unique_timer                    crypto_reordering_timer;
  unique_timer                    metrics_timer;

  task_executor& ue_dl_executor;
  task_executor& crypto_executor;

  pdcp_tx_state st                  = {0, 0, 0, 0, 0};
  uint32_t      desired_buffer_size = 0;
  uint32_t      max_nof_crypto_workers;

  /// Id used to identify out of date PDUs after a retransmission.
  uint16_t retransmit_id = 0;

  using sec_engine_vec = std::vector<std::unique_ptr<security::security_engine_tx>>;
  sec_engine_vec sec_engine_pool;

  security::integrity_enabled integrity_enabled = security::integrity_enabled::off;
  security::ciphering_enabled ciphering_enabled = security::ciphering_enabled::off;

  early_drop_reason check_early_drop(const byte_buffer& buf);
  uint32_t          warn_on_drop_count = 0;

  /// Crypto token manager. Used to wait for crypto engine to finish
  /// when destroying DRB.
  pdcp_crypto_token_manager token_mngr;

  /// Apply ciphering and integrity protection to SDU+header buffer.
  /// It will pass this buffer to the crypto engine for parallization.
  void apply_reordering(pdcp_tx_buffer_info buf_info);

  void write_data_pdu_to_lower_layers(pdcp_tx_pdu_info&& pdu, bool is_retx);

  void write_control_pdu_to_lower_layers(byte_buffer buf);

  /// TODO add docs
  void apply_security(pdcp_tx_buffer_info buf_info);

  /// Apply ciphering and integrity protection to the payload
  security::security_result apply_ciphering_and_integrity_protection(byte_buffer buf, uint32_t count);

  uint32_t notification_count_estimation(uint32_t notification_sn) const;

  /// \brief Stops all discard timer up to a PDCP PDU COUNT number that is provided as argument.
  /// \param highest_count Highest PDCP PDU COUNT to which all discard timers shall be stopped.
  void stop_discard_timer(uint32_t highest_count);

  /// \brief Discard one PDU.
  ///
  /// This will notify lower layers of the discard and remove the element from the tx_window, i.e. stop the discard
  /// timer and delete the associated SDU.
  ///
  /// \param count The COUNT value of the PDU to be discarded.
  void discard_pdu(uint32_t count);

  /// \brief Discard timer information and, only for AM, a copy of the SDU for data recovery procedure.
  pdcp_tx_window tx_window;

  /// \brief Get estimated size of a PDU from an SDU
  uint32_t get_pdu_size(const byte_buffer& sdu) const;

  /// \brief Callback ran upon discard timer expiration. If there are still PDUs in the TX window that require
  /// a discard timer, it is responsible to restart the discard timer with the correct timeout.
  void discard_callback();

  /// \brief Callback ran upon crypto reordering timer expiration. This will advance the TX_TRANS_PENDING
  /// state variable to the next not missing PDU. If an in-flight PDU arrives after advancing the window, it will be
  /// discarded.
  void crypto_reordering_timeout();

  /// \brief handle_transmit_notification_impl Common implementation for transmit and retransmit notifications
  ///
  /// \param notif_sn Notified (re)transmitted PDCP PDU sequence number.
  /// \param is_retx Flags whether this is a notification of a ReTx or not
  void handle_transmit_notification_impl(uint32_t notif_sn, bool is_retx);

  /// \brief handle_delivery_notification_impl Common implementation for deliv and deliv retransmitted notifications.
  /// \param notif_sn Notified delivered or retransmitted delivered PDCP PDU sequence number.
  /// \param is_retx Flags whether this is a notification of a ReTx or not
  void handle_delivery_notification_impl(uint32_t notif_sn, bool is_retx);

  void handle_reordering_timeout();

  pdcp_tx_metrics          metrics;
  pdcp_metrics_aggregator& metrics_agg;
};
} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::pdcp_tx_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_tx_state& st, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "tx_next_ack={} tx_trans_crypto={} tx_reord_crypto={} tx_trans={} tx_next={}",
                     st.tx_next_ack,
                     st.tx_trans_crypto,
                     st.tx_reord_crypto,
                     st.tx_trans,
                     st.tx_next);
  }
};

template <>
struct formatter<srsran::pdcp_entity_tx::early_drop_reason> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_entity_tx::early_drop_reason& drop_reason, FormatContext& ctx) const
  {
    switch (drop_reason) {
      case srsran::pdcp_entity_tx::early_drop_reason::zero_dbs:
        return format_to(ctx.out(), "desired buffer size is 0");
      case srsran::pdcp_entity_tx::early_drop_reason::full_rlc_queue:
        return format_to(ctx.out(), "RLC SDU queue is full");
      case srsran::pdcp_entity_tx::early_drop_reason::full_window:
        return format_to(ctx.out(), "PDCP TX window is full");
      case srsran::pdcp_entity_tx::early_drop_reason::no_drop:
        return format_to(ctx.out(), "no drop");
    }
    return format_to(ctx.out(), "unkown");
  }
};
} // namespace fmt
