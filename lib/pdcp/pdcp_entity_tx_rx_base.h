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

#include "pdcp_bearer_logger.h"
#include "pdcp_sn.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/security/security.h"

namespace srsgnb {

// Header length
constexpr size_t pdcp_data_pdu_header_size_12bit = 2;
constexpr size_t pdcp_data_pdu_header_size_18bit = 3;
constexpr size_t pdcp_data_pdu_header_size_max   = pdcp_data_pdu_header_size_18bit;
constexpr size_t pdcp_data_pdu_header_size(pdcp_sn_size sn_size)
{
  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      return pdcp_data_pdu_header_size_12bit;
    case pdcp_sn_size::size18bits:
      return pdcp_data_pdu_header_size_18bit;
  }
  srsgnb_assertion_failure("Cannot determine PDCP data PDU header size: unsupported sn_size {}",
                           pdcp_sn_size_to_uint(sn_size));
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
  srsgnb_assertion_failure("Cannot determine PDCP window size: unsupported sn_size {}", pdcp_sn_size_to_uint(sn_size));
  return pdcp_window_size_12bit;
}

/// Base class used for both TX and RX PDCP entities.
/// Stores common header and SN/HFN helpers
class pdcp_entity_tx_rx_base
{
protected:
  explicit pdcp_entity_tx_rx_base(rb_id_t rb_id_, pdcp_rb_type rb_type_, pdcp_sn_size sn_size_) :
    rb_id(rb_id_),
    rb_type(rb_type_),
    hdr_len_bytes((pdcp_data_pdu_header_size(sn_size_))),
    window_size(pdcp_window_size(sn_size_)),
    sn_size(sn_size_)
  {
    if (rb_id_.is_drb()) {
      bearer_id = drb_id_to_uint(rb_id_.get_drb_id()) - 1;
    } else if (rb_id.is_srb()) {
      bearer_id = srb_id_to_uint(rb_id_.get_srb_id()) - 1;
    } else {
      srsgnb_assertion_failure("Invalid RB identity: {}", rb_id_);
    }
  }

  /// BEARER (defined as the radio bearer identifier in TS 33.501. It will use the value RB identity –1 as in TS 38.331)
  /// Ref: TS 38.323 Sec. 5.9 Integrity protection and verification
  uint8_t bearer_id;
  rb_id_t rb_id;

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
   * COUNT overflow helpers
   */
  bool max_count_notified = false;
  bool max_count_overflow = false;

  /*
   * SN, HFN and COUNT helpers
   */
  const pdcp_sn_size sn_size;
  uint32_t           SN(uint32_t count) const { return pdcp_compute_sn(count, sn_size); }
  uint32_t           HFN(uint32_t count) const { return pdcp_compute_hfn(count, sn_size); }
  uint32_t           COUNT(uint32_t hfn, uint32_t sn) const { return pdcp_compute_count(hfn, sn, sn_size); }
};

} // namespace srsgnb
