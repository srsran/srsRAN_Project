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

namespace srsgnb {

/// Structure used to represent a PDCP PDU in Tx direction.
/// Depending on the bearer type (SRB/DRB) and PDU type (control/data) the PDU is accompanied with
/// the associated pdcp_count as indicated by has_pdcp_count.
/// For data PDUs on DRBs, has_pdcp_count shall be true and pdcp_count shall be the PDCP COUNT value of the PDU.
/// For control PDUs on DRBs, has_pdcp_count shall be false and pdcp_count is undefined and ignored.
/// For any PDU on SRBs, has_pdcp_count and pdcp_count are undefined and ignored.
struct pdcp_tx_pdu {
  /// The PDCP PDU
  byte_buffer buf;
  /// Indicates whether this PDU is accompanied with a PDCP COUNT value.
  /// This value shall be true for data PDUs on DRBs.
  /// It shall be false for control PDUs on DRBs.
  /// For any PDU on SRBs the value is undefined and shall be ignored.
  /// Ref: TS 38.323 Sec. 6.2.2.1, 6.3.7
  bool has_pdcp_count;
  /// PDCP COUNT value that is associated with this PDU (if applicable).
  /// This value shall be properly set for data PDUs on DRBs.
  /// For control PDUs on DRBs and for any PDU on SRBs the value is undefined and shall be ignored.
  /// Ref: TS 38.323 Sec. 6.2.2, 6.2.3
  uint32_t pdcp_count;

  pdcp_tx_pdu()  = default;
  ~pdcp_tx_pdu() = default;
};

} // namespace srsgnb
