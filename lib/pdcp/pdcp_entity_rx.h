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
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/support/timers.h"
#include <map>

namespace srsgnb {

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

/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class pdcp_entity_rx : public pdcp_entity_tx_rx_base,
                       public pdcp_rx_lower_interface,
                       public pdcp_rx_upper_control_interface
{
public:
  pdcp_entity_rx(uint32_t                     ue_index,
                 lcid_t                       lcid,
                 pdcp_config::pdcp_rx_config  cfg_,
                 pdcp_rx_upper_data_notifier& upper_dn_,
                 timer_manager&               timers);

  void handle_pdu(byte_buffer buf) final;

  /*
   * Header helpers
   */
  bool read_data_pdu_header(const byte_buffer& buf, uint32_t& sn) const;
  void discard_data_header(byte_buffer& buf) const;
  void extract_mac(byte_buffer& buf, sec_mac& mac) const;

  /*
   * Security configuration
   */
  void set_as_security_config(sec_128_as_config sec_cfg_) final { sec_cfg = sec_cfg_; };
  void enable_or_disable_security(pdcp_integrity_enabled integ, pdcp_ciphering_enabled cipher) final
  {
    integrity_enabled = integ;
    ciphering_enabled = cipher;
  }

  /*
   * Testing Helpers
   */
  void set_direction(security_direction direction_) { direction = direction_; };
  void set_state(pdcp_rx_state st_) { st = st_; }

private:
  bearer_logger               logger;
  pdcp_config::pdcp_rx_config cfg;

  sec_128_as_config      sec_cfg           = {};
  security_direction     direction         = security_direction::uplink;
  pdcp_integrity_enabled integrity_enabled = pdcp_integrity_enabled::no;
  pdcp_ciphering_enabled ciphering_enabled = pdcp_ciphering_enabled::no;

  pdcp_rx_state st = {};

  // Reordering queue and timer.
  std::map<uint32_t, byte_buffer> reorder_queue;
  unique_timer                    reordering_timer;
  class reordering_callback;

  void deliver_all_consecutive_counts();

  bool        integrity_verify(byte_buffer_view buf, uint32_t count, const sec_mac& mac);
  byte_buffer cipher_decrypt(byte_buffer_view msg, uint32_t count);

  /*
   * Notifiers
   */
  pdcp_rx_upper_data_notifier& upper_dn;

  timer_manager& timers;
};

// Reordering callback (t-Reordering)
class pdcp_entity_rx::reordering_callback
{
public:
  explicit reordering_callback(pdcp_entity_rx* parent_) : parent(parent_) {}
  void operator()(uint32_t timer_id);

private:
  pdcp_entity_rx* parent;
};
} // namespace srsgnb
