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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/ran/bearer_logger.h"

namespace srsgnb {
/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_tx : public pdcp_tx_upper_data_interface, public pdcp_tx_lower_interface
{
public:
  pdcp_entity_tx(uint32_t                        ue_index,
                 lcid_t                          lcid,
                 pdcp_config::pdcp_tx_config     cfg_,
                 pdcp_tx_lower_notifier&         lower_dn,
                 pdcp_tx_upper_control_notifier& upper_cn) :
    hdr_len_bytes(to_number(cfg_.sn_size) % 8),
    logger("PDCP", ue_index, lcid),
    cfg(cfg_),
    lower_dn(lower_dn),
    upper_cn(upper_cn)
  {
  }

  /*
   * Header helpers
   */
  const uint32_t hdr_len_bytes;
  bool           write_data_pdu_header(byte_buffer& buf, uint32_t count);

private:
  bearer_logger               logger;
  pdcp_config::pdcp_tx_config cfg;

  pdcp_tx_lower_notifier&         lower_dn;
  pdcp_tx_upper_control_notifier& upper_cn;

  void handle_sdu(byte_buffer buf) final
  {
    bool max_hfn_reached = false; // FIXME actually check HFN
    if (max_hfn_reached) {
      upper_cn.on_max_hfn_reached();
    }
    lower_dn.on_new_pdu(std::move(buf));
  }
  void stop_discard_timer(uint32_t count) final {}

  /*
   * RB helpers
   */
  bool is_srb() { return cfg.rb_type == pdcp_rb_type::srb; }
  bool is_drb() { return cfg.rb_type == pdcp_rb_type::drb; }
};
} // namespace srsgnb
