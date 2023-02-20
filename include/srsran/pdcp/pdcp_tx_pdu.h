/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"

namespace srsran {

/// Structure used to represent a PDCP PDU in Tx direction.
/// Depending on the bearer type (SRB/DRB) and PDU type (control/data) the PDU is accompanied with
/// the associated pdcp_sn.
/// For data PDUs on DRBs, pdcp_sn shall be set to the PDCP sequence number (SN) of the PDU.
/// For control PDUs on DRBs, pdcp_sn shall be unset and ignored.
/// For any PDU on SRBs, pdcp_sn shall be unset and ignored.
struct pdcp_tx_pdu {
  /// The PDCP PDU
  byte_buffer buf;
  /// Optional PDCP sequence number (SN) that is associated with this PDU (if applicable).
  /// This value shall be properly set for data PDUs on DRBs.
  /// For control PDUs on DRBs and for any PDU on SRBs the value is unset and shall be ignored.
  /// Ref: TS 38.323 Sec. 6.2.2, 6.2.3, 6.3.7
  optional<uint32_t> pdcp_sn;
};

} // namespace srsran
