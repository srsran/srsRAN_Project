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
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/ran/bearer_logger.h"

namespace srsgnb {
/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class pdcp_entity_rx : public pdcp_rx_lower_interface
{
public:
  pdcp_entity_rx(uint32_t                     ue_index,
                 lcid_t                       lcid,
                 pdcp_config::pdcp_rx_config  cfg_,
                 pdcp_rx_upper_data_notifier& upper_dn_) :
    logger("PDCP", ue_index, lcid), cfg(cfg_), upper_dn(upper_dn_)
  {
    switch (cfg.sn_size) {
      case pdcp_sn_size::size12bits:
        hdr_len_bytes = 2;
        break;
      case pdcp_sn_size::size18bits:
        hdr_len_bytes = 3;
        break;
    }
  }

private:
  bearer_logger               logger;
  pdcp_config::pdcp_rx_config cfg;

  void handle_pdu(byte_buffer buf) final
  {
    logger.log_info("RX PDU of %zd B\n", buf.length());

    // strip away the first two bytes containing the PDCP header
    byte_buffer sdu(buf.begin() + 2, buf.end());
    upper_dn.on_new_sdu(std::move(sdu));
  }

  /*
   * SN, HFN and COUNT helpers
   */
  uint32_t SN(uint32_t count) { return count & (0xFFFFFFFFU >> (32U - to_number(cfg.sn_size))); }

  /*
   * PDU Helpers
   */
  /// \brief Read PDCP SN from PDU.
  /// Does not modify the PDU buffer.
  uint32_t hdr_len_bytes;
  bool     read_data_pdu_header(const byte_buffer& buf, uint32_t& sn);

  pdcp_rx_upper_data_notifier& upper_dn;
};

} // namespace srsgnb
