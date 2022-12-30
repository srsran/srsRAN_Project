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
#include "srsgnb/adt/optional.h"

namespace srsgnb {

/// Structure used to represent a PDCP PDU in Tx direction.
/// Depending on the bearer type (SRB/DRB) and PDU type (control/data) the PDU is accompanied with
/// the associated pdcp_count.
/// For data PDUs on DRBs, pdcp_count shall be set to the PDCP COUNT value of the PDU.
/// For control PDUs on DRBs, pdcp_count shall be unset and ignored.
/// For any PDU on SRBs, pdcp_count shall be unset and ignored.
struct pdcp_tx_pdu {
  /// The PDCP PDU
  byte_buffer buf;
  /// Optional PDCP COUNT value that is associated with this PDU (if applicable).
  /// This value shall be properly set for data PDUs on DRBs.
  /// For control PDUs on DRBs and for any PDU on SRBs the value is unset and shall be ignored.
  /// Ref: TS 38.323 Sec. 6.2.2, 6.2.3, 6.3.7
  optional<uint32_t> pdcp_count;
};

} // namespace srsgnb
