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
#include "pdcp_rx_metrics_impl.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/security/security_engine.h"
#include "srsran/support/sdu_window.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"

namespace srsran {

/// PDCP RX state variables,
/// TS 38.323, section 7.1
struct pdcp_rx_state {
  /// RX_NEXT indicates the COUNT value of the next PDCP SDU expected to be received.
  uint32_t rx_next;
  /// RX_DELIV indicates the COUNT value of the first PDCP SDU not delivered to the upper layers, but still
  /// waited for.
  uint32_t rx_deliv;
  /// RX_REORD indicates the COUNT value following the COUNT value associated with the PDCP Data PDU which
  /// triggered t-Reordering.
  uint32_t rx_reord;
};

/// Helper structure used to pass RX PDUs to the security engine.
struct pdcp_rx_pdu_info {
  /// The PDU/SDU buffer (PDU header/footer are removed during processing so that finally the SDU remains.)
  byte_buffer buf;
  /// The count value of the PDU
  uint32_t count = 0;
  /// Time of arrival at the PDCP entity
  std::chrono::system_clock::time_point time_of_arrival;
  /// The PDCP crypto token
  pdcp_crypto_token token;
};

/// Structure used to hold RX SDUs in the RX window.
struct pdcp_rx_sdu_info {
  /// The SDU buffer
  byte_buffer buf;
  /// The count value of the SDU
  uint32_t count = 0;
  /// Time of arrival at the PDCP entity
  std::chrono::system_clock::time_point time_of_arrival;
};

/// Base class used for receiving PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_rx final : public pdcp_entity_tx_rx_base,
                             public pdcp_rx_status_provider,
                             public pdcp_rx_lower_interface,
                             public pdcp_rx_upper_control_interface
{
public:
  pdcp_entity_rx(uint32_t                        ue_index,
                 rb_id_t                         rb_id_,
                 pdcp_rx_config                  cfg_,
                 pdcp_rx_upper_data_notifier&    upper_dn_,
                 pdcp_rx_upper_control_notifier& upper_cn_,
                 timer_factory                   ue_ul_timer_factory_,
                 task_executor&                  ue_ul_executor_,
                 task_executor&                  crypto_executor_,
                 uint32_t                        max_nof_crypto_workers_,
                 pdcp_metrics_aggregator&        metrics_agg_);

  ~pdcp_entity_rx() override;

  /// \brief Stop handling PDUs and stops timers
  void stop();

  void handle_pdu(byte_buffer_chain buf) override;

  /// \brief Triggers re-establishment as specified in TS 38.323, section 5.1.2
  void reestablish(security::sec_128_as_config sec_cfg) override;

  /// \brief Get the RX count for status transfer
  pdcp_count_info get_count() const override
  {
    pdcp_count_info count_info;
    uint32_t        count = st.rx_deliv;
    count_info.sn         = SN(count);
    count_info.hfn        = HFN(count);
    return count_info;
  }

  /// \brief Set the RX count for status transfer
  void set_count(pdcp_count_info count_info) override
  {
    uint32_t count = COUNT(count_info.hfn, count_info.sn);
    if (st.rx_next != 0 || st.rx_deliv != 0 || st.rx_reord != 0) {
      logger.log_warning("Status transfer applied to bearer with non-zero state. st={} count={}", st, count);
    }
    st = {count, count, count};
    logger.log_info("Setted PDCP RX state. {}", st);
  }

  /// \brief Retrun awaitable to wait for cripto tasks to be
  /// finished.
  manual_event_flag& crypto_awaitable();

  void notify_pdu_processing_stopped() override;
  void restart_pdu_processing() override;

  // Rx/Tx interconnect
  void set_status_handler(pdcp_tx_status_handler* status_handler_) { status_handler = status_handler_; }

  /// \brief Compiles a PDCP status report
  ///
  /// Ref: TS 38.323, Sec. 5.4.1, Sec. 6.2.3.1 and Sec. 6.3.{9,10}
  ///
  /// \return Control PDU for PDCP status report as a byte_buffer
  byte_buffer compile_status_report() override;

  /*
   * Header helpers
   */

  /// \brief Reads the header of a PDCP data PDU and writes the content into an associated object
  /// \param[out] hdr Reference to a pdcp_data_pdu_header that is filled with the header content
  /// \param[in] buf Reference to the PDU bytes
  /// \return True if header was read successfully, false otherwise
  bool read_data_pdu_header(pdcp_data_pdu_header& hdr, const byte_buffer& buf) const;

  /*
   * Security configuration
   */
  void configure_security(security::sec_128_as_config sec_cfg,
                          security::integrity_enabled integrity_enabled_,
                          security::ciphering_enabled ciphering_enabled_) override;

  /*
   * Testing Helpers
   */
  void          set_state(pdcp_rx_state st_) { st = st_; }
  pdcp_rx_state get_state() const { return st; }
  bool          is_reordering_timer_running() const { return reordering_timer.is_running(); }

private:
  pdcp_bearer_logger   logger;
  const pdcp_rx_config cfg;
  bool                 stopped = false;

  using sec_engine_vec = std::vector<std::unique_ptr<security::security_engine_rx>>;
  sec_engine_vec sec_engine_pool;

  security::integrity_enabled integrity_enabled = security::integrity_enabled::off;
  security::ciphering_enabled ciphering_enabled = security::ciphering_enabled::off;

  pdcp_rx_state st = {};

  /// Rx window
  sdu_window<pdcp_rx_sdu_info, pdcp_bearer_logger> rx_window;

  /// Rx reordering timer
  unique_timer reordering_timer;
  class reordering_callback;
  void handle_t_reordering_expire();

  /// Crypto token manager. Used to wait for crypto engine to finish
  /// when destroying DRB.
  pdcp_crypto_token_manager token_mngr;

  // Handling of different PDU types

  /// \brief Handles a received data PDU.
  /// \param pdu The data PDU to be handled (including header and payload)
  /// \param time_of_arrival The time of arrival at the PDCP entity
  void handle_data_pdu(byte_buffer pdu, std::chrono::system_clock::time_point time_of_arrival);

  void apply_security(pdcp_rx_pdu_info&& pdu_info);

  void apply_reordering(pdcp_rx_pdu_info pdu_info);

  /// \brief Handles a received control PDU.
  /// \param buf The control PDU to be handled (including header and payload)
  void handle_control_pdu(byte_buffer_chain pdu);

  void deliver_all_consecutive_counts();
  void deliver_all_sdus();
  void discard_all_sdus();

  void record_reordering_dealy(std::chrono::system_clock::time_point time_of_arrival);

  /// Apply deciphering and integrity check to the PDU
  security::security_result apply_deciphering_and_integrity_check(byte_buffer buf, uint32_t count);

  /*
   * Notifiers and handlers
   */
  pdcp_tx_status_handler*         status_handler = nullptr;
  pdcp_rx_upper_data_notifier&    upper_dn;
  pdcp_rx_upper_control_notifier& upper_cn;

  timer_factory ue_ul_timer_factory;

  task_executor& ue_ul_executor;
  task_executor& crypto_executor;
  uint32_t       max_nof_crypto_workers;

  pdcp_rx_metrics          metrics;
  pdcp_metrics_aggregator& metrics_agg;
  unique_timer             metrics_timer;

  void log_state(srslog::basic_levels level) { logger.log(level, "RX entity state. {}", st); }
};

// Reordering callback (t-Reordering)
class pdcp_entity_rx::reordering_callback
{
public:
  explicit reordering_callback(pdcp_entity_rx* parent_) : parent(parent_) {}
  void operator()(timer_id_t timer_id);

private:
  pdcp_entity_rx* parent;
};
} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::pdcp_rx_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_rx_state& st, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "rx_next={} rx_deliv={} rx_reord={}", st.rx_next, st.rx_deliv, st.rx_reord);
  }
};

} // namespace fmt
