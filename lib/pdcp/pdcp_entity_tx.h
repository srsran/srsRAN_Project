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
#include "pdcp_entity_tx_rx_base.h"
#include "pdcp_interconnect.h"
#include "pdcp_metrics_aggregator.h"
#include "pdcp_pdu.h"
#include "pdcp_tx_metrics_impl.h"
#include "pdcp_tx_window.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/adt/expected.h"
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

  bool operator==(const pdcp_tx_state& other) const
  {
    return tx_next == other.tx_next && tx_trans == other.tx_trans && tx_next_ack == other.tx_next_ack;
  }
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
                 pdcp_metrics_aggregator&        metrics_agg_);

  ~pdcp_entity_tx() override;

  /// \brief Stop handling SDUs and stop timers
  void stop();

  /// \brief Triggers re-establishment as specified in TS 38.323, section 5.1.2
  void reestablish(security::sec_128_as_config sec_cfg) override;

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
  };

  const pdcp_tx_state& get_state() const { return st; };

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

private:
  pdcp_bearer_logger              logger;
  const pdcp_tx_config            cfg;
  bool                            stopped         = false;
  pdcp_rx_status_provider*        status_provider = nullptr;
  pdcp_tx_lower_notifier&         lower_dn;
  pdcp_tx_upper_control_notifier& upper_cn;
  timer_factory                   ue_ctrl_timer_factory;
  unique_timer                    discard_timer;
  unique_timer                    metrics_timer;

  task_executor& ue_dl_executor;
  task_executor& crypto_executor;

  pdcp_tx_state st                  = {};
  uint32_t      desired_buffer_size = 0;

  std::unique_ptr<security::security_engine_tx> sec_engine;

  security::integrity_enabled integrity_enabled = security::integrity_enabled::off;
  security::ciphering_enabled ciphering_enabled = security::ciphering_enabled::off;

  void write_data_pdu_to_lower_layers(pdcp_tx_buf_info&& buf_info, bool is_retx);
  void write_control_pdu_to_lower_layers(byte_buffer buf);

  /// Apply ciphering and integrity protection to the payload
  expected<byte_buffer> apply_ciphering_and_integrity_protection(byte_buffer buf, uint32_t count);

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

  /// \breif Callback ran upon discard timer expiration. If there are still PDUs in the TX window that require
  /// a discard timer, it is responsible to restart the discard timer with the correct timeout.
  void discard_callback();

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
    return format_to(ctx.out(), "tx_next_ack={} tx_trans={} tx_next={}", st.tx_next_ack, st.tx_trans, st.tx_next);
  }
};
} // namespace fmt
