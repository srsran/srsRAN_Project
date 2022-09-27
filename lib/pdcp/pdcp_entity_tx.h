/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdcp_entity_tx_rx_base.h"
#include "pdcp_tx_metrics_impl.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/security/security.h"
#include "srsgnb/support/timers.h"
#include <map>

namespace srsgnb {

/// PDCP TX state variables,
/// TS 38.323, section 7.1
struct pdcp_tx_state {
  /// This state variable indicates the COUNT value of the next PDCP SDU to be transmitted. The initial value is 0,
  /// except for SRBs configured with state variables continuation.
  uint32_t tx_next;
};

/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_tx : public pdcp_entity_tx_rx_base,
                       public pdcp_tx_upper_data_interface,
                       public pdcp_tx_upper_control_interface,
                       public pdcp_tx_lower_interface,
                       public pdcp_tx_metrics
{
public:
  pdcp_entity_tx(uint32_t                        ue_index,
                 lcid_t                          lcid,
                 pdcp_config::pdcp_tx_config     cfg_,
                 pdcp_tx_lower_notifier&         lower_dn,
                 pdcp_tx_upper_control_notifier& upper_cn,
                 timer_manager&                  timers) :
    pdcp_entity_tx_rx_base(lcid, cfg_.rb_type, cfg_.sn_size),
    logger("PDCP", ue_index, lcid),
    cfg(cfg_),
    lower_dn(lower_dn),
    upper_cn(upper_cn),
    timers(timers)
  {
    // Validate configuration
    srsgnb_assert((is_um() && cfg.discard_timer == pdcp_discard_timer::not_configured) || is_am(),
                  "RLC UM with discard timer is un-supported. RLC mode={}, discardTimer={}",
                  cfg.rlc_mode,
                  cfg.discard_timer);
    direction =
        cfg.direction == pdcp_security_direction::uplink ? security_direction::uplink : security_direction::downlink;

    logger.log_info("PDCP TX entity configured. Configuration: {}", cfg);
  }

  /*
   * SDU/PDU handlers
   */
  void handle_sdu(byte_buffer buf) final;

  void stop_discard_timer(uint32_t count) final
  {
    // Remove timer from map
    logger.log_debug("Stopping discard timer for COUNT={}", count);
    discard_timers_map.erase(count);
  }

  /*
   * Header helpers
   */
  bool write_data_pdu_header(byte_buffer& buf, uint32_t count);

  /*
   * Testing helpers
   */
  void     set_state(pdcp_tx_state st_) { st = st_; };
  uint32_t nof_discard_timers() { return discard_timers_map.size(); }

  /*
   * Security configuration
   */
  void set_as_security_config(sec_128_as_config sec_cfg_) final
  {
    sec_cfg = sec_cfg_;
    logger.log_info(
        "Set TX security configuration, integ=NIA{}, cipher=NEA{}", sec_cfg.integ_algo, sec_cfg.cipher_algo);
  };
  void enable_or_disable_security(pdcp_integrity_enabled integ, pdcp_ciphering_enabled cipher) final
  {
    integrity_enabled = integ;
    ciphering_enabled = cipher;
    logger.log_info("Enabled/disabled TX security integrity={}, ciphering={}", integ, cipher);
  }

private:
  bearer_logger               logger;
  pdcp_config::pdcp_tx_config cfg;

  pdcp_tx_lower_notifier&         lower_dn;
  pdcp_tx_upper_control_notifier& upper_cn;
  timer_manager&                  timers;

  pdcp_tx_state      st        = {};
  security_direction direction = security_direction::downlink;

  sec_128_as_config      sec_cfg           = {};
  pdcp_integrity_enabled integrity_enabled = pdcp_integrity_enabled::no;
  pdcp_ciphering_enabled ciphering_enabled = pdcp_ciphering_enabled::no;

  /// Apply ciphering and integrity protection to the payload
  byte_buffer apply_ciphering_and_integrity_protection(byte_buffer hdr, byte_buffer buf, uint32_t count);
  void        integrity_generate(sec_mac& mac, byte_buffer_view buf, uint32_t count);
  byte_buffer cipher_encrypt(byte_buffer_view buf, uint32_t count);

  /// \brief discardTimer
  /// This map is used to store the discard timers that are used by the transmitting side of an PDCP entity
  /// to order lower layers to discard PDCP PDUs if the timer expired. See section 5.2.1 and 7.3 of TS 38.323.
  /// Currently, this is only supported when using RLC AM, as only AM as the ability to stop the timers.
  std::map<uint32_t, unique_timer> discard_timers_map;
  class discard_callback;

  /*
   * RB helpers
   */
  bool is_srb() { return cfg.rb_type == pdcp_rb_type::srb; }
  bool is_drb() { return cfg.rb_type == pdcp_rb_type::drb; }
  bool is_um() { return cfg.rlc_mode == pdcp_rlc_mode::um; }
  bool is_am() { return cfg.rlc_mode == pdcp_rlc_mode::am; }
};

class pdcp_entity_tx::discard_callback
{
public:
  discard_callback(pdcp_entity_tx* parent_, uint32_t count_) : parent(parent_), discard_count(count_) {}
  void operator()(uint32_t timer_id);

private:
  pdcp_entity_tx* parent;
  uint32_t        discard_count;
};
} // namespace srsgnb
