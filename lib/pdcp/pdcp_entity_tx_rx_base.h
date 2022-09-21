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
#include "srsgnb/security/security.h"

namespace srsgnb {

// Header length
constexpr size_t pdcp_data_pdu_header_size_12bit = 2;
constexpr size_t pdcp_data_pdu_header_size_18bit = 3;
constexpr size_t pdcp_data_pdu_header_size(pdcp_sn_size sn_size)
{
  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      return pdcp_data_pdu_header_size_12bit;
    case pdcp_sn_size::size18bits:
      return pdcp_data_pdu_header_size_18bit;
  }
  srsgnb_assertion_failure("Cannot determine PDCP data PDU header size: unsupported sn_size {}", to_number(sn_size));
  return pdcp_data_pdu_header_size_12bit;
}

// Window size
constexpr size_t   pdcp_window_size_12bit = 2048;
constexpr size_t   pdcp_window_size_18bit = 131072;
constexpr uint32_t pdcp_window_size(pdcp_sn_size sn_size)
{
  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      return pdcp_window_size_12bit;
    case pdcp_sn_size::size18bits:
      return pdcp_window_size_18bit;
  }
  srsgnb_assertion_failure("Cannot determine PDCP window size: unsupported sn_size {}", to_number(sn_size));
  return pdcp_window_size_12bit;
}

/// Base class used for both TX and RX PDCP entities.
/// Stores common header and SN/HFN helpers
class pdcp_entity_tx_rx_base
{
protected:
  explicit pdcp_entity_tx_rx_base(lcid_t lcid, pdcp_rb_type rb_type, pdcp_sn_size sn_size) :
    lcid(lcid),
    rb_type(rb_type),
    hdr_len_bytes((pdcp_data_pdu_header_size(sn_size))),
    window_size(pdcp_window_size(sn_size)),
    sn_size(to_number(sn_size))
  {
  }

  lcid_t lcid = {};

  /*
   * RB helpers
   */
  const pdcp_rb_type rb_type;
  bool               is_srb() { return rb_type == pdcp_rb_type::srb; }
  bool               is_drb() { return rb_type == pdcp_rb_type::drb; }

  /*
   * Header and window helpers
   */
  const uint32_t hdr_len_bytes;
  const uint32_t window_size;

  /*
   * SN, HFN and COUNT helpers
   */
  const uint32_t sn_size;
  uint32_t       SN(uint32_t count) const { return count & (0xffffffffU >> (32U - sn_size)); }
  uint32_t       HFN(uint32_t count) const { return (count >> sn_size); }
  uint32_t       COUNT(uint32_t hfn, uint32_t sn) const { return (hfn << sn_size) | sn; }
};

} // namespace srsgnb
