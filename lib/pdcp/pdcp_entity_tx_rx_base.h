/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "pdcp_sn.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/security/security.h"

namespace srsran {

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
  srsran_assertion_failure("Cannot determine PDCP data PDU header size: unsupported sn_size={}",
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
  srsran_assertion_failure("Cannot determine PDCP window size: unsupported sn_size={}", pdcp_sn_size_to_uint(sn_size));
  return pdcp_window_size_12bit;
}

/// Base class used for both TX and RX PDCP entities.
/// Stores common header and SN/HFN helpers
class pdcp_entity_tx_rx_base
{
protected:
  explicit pdcp_entity_tx_rx_base(rb_id_t       rb_id_,
                                  pdcp_rb_type  rb_type_,
                                  pdcp_rlc_mode rlc_mode_,
                                  pdcp_sn_size  sn_size_) :
    rb_id(rb_id_),
    rb_type(rb_type_),
    rlc_mode(rlc_mode_),
    hdr_len_bytes((pdcp_data_pdu_header_size(sn_size_))),
    window_size(pdcp_window_size(sn_size_)),
    sn_size(sn_size_)
  {
    if (rb_id_.is_drb()) {
      bearer_id = drb_id_to_uint(rb_id_.get_drb_id()) - 1;
    } else if (rb_id.is_srb()) {
      bearer_id = srb_id_to_uint(rb_id_.get_srb_id()) - 1;
    } else {
      srsran_assertion_failure("Invalid RB identity: {}", rb_id_);
    }
  }

  /// BEARER (defined as the radio bearer identifier in TS 33.501. It will use the value RB identity –1 as in TS 38.331)
  /// Ref: TS 38.323 Sec. 5.9 Integrity protection and verification
  uint8_t bearer_id;
  rb_id_t rb_id;

  /*
   * RB helpers
   */
  const pdcp_rb_type  rb_type;
  const pdcp_rlc_mode rlc_mode;
  bool                is_srb() const { return rb_type == pdcp_rb_type::srb; }
  bool                is_drb() const { return rb_type == pdcp_rb_type::drb; }
  bool                is_um() const { return rlc_mode == pdcp_rlc_mode::um; }
  bool                is_am() const { return rlc_mode == pdcp_rlc_mode::am; }

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

} // namespace srsran
