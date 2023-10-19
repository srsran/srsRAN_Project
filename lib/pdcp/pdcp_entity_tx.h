/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "pdcp_pdu.h"
#include "pdcp_tx_metrics_impl.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/adt/expected.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/security/security.h"
#include "srsran/support/timers.h"
#include <map>

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
};

/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_tx final : public pdcp_entity_tx_rx_base,
                             public pdcp_tx_status_handler,
                             public pdcp_tx_upper_data_interface,
                             public pdcp_tx_upper_control_interface,
                             public pdcp_tx_lower_interface,
                             public pdcp_tx_metrics
{
public:
  pdcp_entity_tx(uint32_t                        ue_index,
                 rb_id_t                         rb_id_,
                 pdcp_tx_config                  cfg_,
                 pdcp_tx_lower_notifier&         lower_dn_,
                 pdcp_tx_upper_control_notifier& upper_cn_,
                 timer_factory                   timers_) :
    pdcp_entity_tx_rx_base(rb_id_, cfg_.rb_type, cfg_.rlc_mode, cfg_.sn_size),
    logger("PDCP", {ue_index, rb_id_, "DL"}),
    cfg(cfg_),
    lower_dn(lower_dn_),
    upper_cn(upper_cn_),
    timers(timers_)
  {
    // Validate configuration
    if (is_srb() && (cfg.sn_size != pdcp_sn_size::size12bits)) {
      report_error("PDCP SRB with invalid sn_size. {}", cfg);
    }
    if (is_srb() && is_um()) {
      report_error("PDCP SRB cannot be used with RLC UM. {}", cfg);
    }
    if (is_srb() && cfg.discard_timer.has_value()) {
      logger.log_error("Invalid SRB config with discard_timer={}", cfg.discard_timer.value());
    }
    if (is_drb() && !cfg.discard_timer.has_value()) {
      logger.log_error("Invalid DRB config, discard_timer is not configured");
    }

    direction = cfg.direction == pdcp_security_direction::uplink ? security::security_direction::uplink
                                                                 : security::security_direction::downlink;
    logger.log_info("PDCP configured. {}", cfg);
  }

  /// \brief Triggers re-establishment as specified in TS 38.323, section 5.1.2
  void reestablish(security::sec_128_as_config sec_cfg) override;

  // Tx/Rx interconnect
  void set_status_provider(pdcp_rx_status_provider* status_provider_) { status_provider = status_provider_; }

  /*
   * SDU/PDU handlers
   */
  void handle_sdu(byte_buffer sdu) final;

  void handle_transmit_notification(uint32_t notif_sn) override;
  void handle_delivery_notification(uint32_t notif_sn) override;

  /// \brief Evaluates a PDCP status report
  ///
  /// Ref: Ref: TS 38.323, Sec. 5.4.2, Sec. 6.2.3.1 and Sec. 6.3.{9,10}
  ///
  /// \param status The status report
  void handle_status_report(byte_buffer_chain status);

  // Status handler interface
  void on_status_report(byte_buffer_chain status) final { handle_status_report(std::move(status)); }

  /*
   * Header helpers
   */

  /// \brief Writes the header of a PDCP data PDU according to the content of the associated object
  /// \param[out] buf Reference to a byte_buffer that is appended by the header bytes
  /// \param[in] hdr Reference to a pdcp_data_pdu_header that represents the header content
  SRSRAN_NODISCARD bool write_data_pdu_header(byte_buffer& buf, const pdcp_data_pdu_header& hdr) const;

  /*
   * Testing helpers
   */
  void     set_state(pdcp_tx_state st_) { st = st_; };
  uint32_t nof_discard_timers() { return discard_timers_map.size(); }

  /*
   * Security configuration
   */
  void configure_security(security::sec_128_as_config sec_cfg_) final
  {
    srsran_assert((is_srb() && sec_cfg_.domain == security::sec_domain::rrc) ||
                      (is_drb() && sec_cfg_.domain == security::sec_domain::up),
                  "Invalid sec_domain={} for {} in {}",
                  sec_cfg.domain,
                  rb_type,
                  rb_id);
    // The 'NULL' integrity protection algorithm (nia0) is used only for SRBs and for the UE in limited service mode,
    // see TS 33.501 [11] and when used for SRBs, integrity protection is disabled for DRBs. In case the ′NULL'
    // integrity protection algorithm is used, 'NULL' ciphering algorithm is also used.
    // Ref: TS 38.331 Sec. 5.3.1.2
    if ((sec_cfg_.integ_algo == security::integrity_algorithm::nia0) &&
        (is_drb() || (is_srb() && sec_cfg_.cipher_algo != security::ciphering_algorithm::nea0))) {
      logger.log_error(
          "Integrity algorithm NIA0 is only permitted for SRBs configured with NEA0. is_srb={} NIA{} NEA{}",
          is_srb(),
          sec_cfg_.integ_algo,
          sec_cfg_.cipher_algo);
    }

    sec_cfg = sec_cfg_;
    logger.log_info(
        "Security configured: NIA{} NEA{} domain={}", sec_cfg.integ_algo, sec_cfg.cipher_algo, sec_cfg.domain);
    if (sec_cfg.k_128_int.has_value()) {
      logger.log_info(sec_cfg.k_128_int.value().data(), 16, "128 K_int");
    }
    logger.log_info(sec_cfg.k_128_enc.data(), 16, "128 K_enc");
  };

  void set_integrity_protection(security::integrity_enabled integrity_enabled_) final
  {
    if (integrity_enabled_ == security::integrity_enabled::on) {
      if (!sec_cfg.k_128_int.has_value()) {
        logger.log_error("Cannot enable integrity protection: Integrity key is not configured.");
        return;
      }
      if (!sec_cfg.integ_algo.has_value()) {
        logger.log_error("Cannot enable integrity protection: Integrity algorithm is not configured.");
        return;
      }
    }
    integrity_enabled = integrity_enabled_;
    logger.log_info("Set integrity_enabled={}", integrity_enabled);
  }
  void set_ciphering(security::ciphering_enabled ciphering_enabled_) final
  {
    ciphering_enabled = ciphering_enabled_;
    logger.log_info("Set ciphering_enabled={}", ciphering_enabled);
  }

  /// Sends a status report, as specified in TS 38.323, Sec. 5.4.
  void send_status_report();

  /// Performs data recovery, as specified in TS 38.323, Sec. 5.5.
  void data_recovery() override;

  /// Discard all PDUs, delivery information and discard timers.
  void discard_all_pdus();

  /// Retransmits all PDUs. Integrity protection and ciphering is re-applied.
  void retransmit_all_pdus();

private:
  pdcp_bearer_logger   logger;
  const pdcp_tx_config cfg;

  pdcp_rx_status_provider*        status_provider = nullptr;
  pdcp_tx_lower_notifier&         lower_dn;
  pdcp_tx_upper_control_notifier& upper_cn;
  timer_factory                   timers;

  pdcp_tx_state                st        = {};
  security::security_direction direction = security::security_direction::downlink;

  security::sec_128_as_config sec_cfg           = {};
  security::integrity_enabled integrity_enabled = security::integrity_enabled::off;
  security::ciphering_enabled ciphering_enabled = security::ciphering_enabled::off;

  void write_data_pdu_to_lower_layers(uint32_t count, byte_buffer buf);
  void write_control_pdu_to_lower_layers(byte_buffer buf);

  /// Apply ciphering and integrity protection to the payload
  expected<byte_buffer>
              apply_ciphering_and_integrity_protection(byte_buffer hdr, const byte_buffer& sdu, uint32_t count);
  void        integrity_generate(security::sec_mac& mac, byte_buffer_view buf, uint32_t count);
  byte_buffer cipher_encrypt(byte_buffer_view buf, uint32_t count);

  uint32_t notification_count_estimation(uint32_t notification_sn);

  /// \brief Stops all discard timer up to a PDCP PDU COUNT number that is provided as argument.
  /// \param highest_count Highest PDCP PDU COUNT to which all discard timers shall be stopped.
  void stop_discard_timer(uint32_t highest_count);

  /// Discard timer information. We keep both the discard timer
  /// and a copy of the SDU for the data recovery procedure (for AM only).
  struct discard_info {
    uint32_t     count;
    byte_buffer  sdu;
    unique_timer discard_timer;
  };

  /// \brief discardTimer
  /// This map is used to store the discard timers that are used by the transmitting side of an PDCP entity
  /// to order lower layers to discard PDCP PDUs if the timer expired. See section 5.2.1 and 7.3 of TS 38.323.
  /// Currently, this is only supported when using RLC AM, as only AM has the ability to stop the timers.
  std::map<uint32_t, discard_info> discard_timers_map;
  class discard_callback;
};

class pdcp_entity_tx::discard_callback
{
public:
  discard_callback(pdcp_entity_tx* parent_, uint32_t count_) : parent(parent_), discard_count(count_) {}
  void operator()(timer_id_t timer_id);

private:
  pdcp_entity_tx* parent;
  uint32_t        discard_count;
};
} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::pdcp_tx_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_tx_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "tx_trans={} tx_next={}", st.tx_trans, st.tx_next);
  }
};
} // namespace fmt
