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

/// Base class used for both TX and RX PDCP entities.
/// Stores common header and SN/HFN helpers
class pdcp_entity_tx_rx_base
{
protected:
  explicit pdcp_entity_tx_rx_base(pdcp_sn_size sn_size) :
    hdr_len_bytes(to_number(sn_size) % 8), sn_size(to_number(sn_size))
  {
  }

  /*
   * Header helpers
   */
  const uint32_t hdr_len_bytes;

  /*
   * SN, HFN and COUNT helpers
   */
  const uint32_t sn_size;
  uint32_t       SN(uint32_t count) const { return count & (0xffffffffU >> (32U - sn_size)); }
  uint32_t       HFN(uint32_t count) const { return (count >> sn_size); }
  uint32_t       COUNT(uint32_t hfn, uint32_t sn) const { return (hfn << sn_size) | sn; }
};

} // namespace srsgnb
