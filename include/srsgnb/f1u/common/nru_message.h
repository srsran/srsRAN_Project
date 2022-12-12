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

#include <cstdint>

namespace srsgnb {

/// NR-U DL User Data (PDU Type 0).
///
/// Ref: TS 38.425 Sec. 5.5.2.1
struct nru_dl_user_data {};

/// NR-U DL Data Delivery Status (PDU Type 1).
///
/// Ref: TS 38.425 Sec. 5.5.2.2
struct nru_dl_data_delivery_status {};

/// NR-U Assistance Information (PDU Type 2).
///
/// Ref: TS 38.425 Sec. 5.5.2.3
struct nru_assistance_information {};

/// NR-U DL message exchanged from node holding a PDCP entity (CU-UP) to the peer node (DU)
struct nru_dl_message {
  // NR-U Payload
  byte_buffer t_pdu;      ///< Transport PDU, e.g. PDCP PDU
  uint32_t    pdcp_count; ///< PDCP count value of the t_pdu
  // NR-U Information
  nru_dl_user_data dl_user_data; ///< NR-U DL User Data
};

/// NR-U UL message exchanged from node the peer node (DU) to the node holding a PDCP entity (CU-UP)
struct nru_ul_message {
  // NR-U Payload
  byte_buffer_slice_chain t_pdu; ///< Transport PDU, e.g. PDCP PDU
  // NR-U Information
  nru_dl_data_delivery_status data_delivery_status;               ///< NR-U DL Data Delivery Status
  bool                        has_data_delivery_status = false;   ///< Flags the presence of DL Data Delivery Status
  nru_assistance_information  assistance_information;             ///< NR-U Assistance Information
  bool                        has_assistance_information = false; ///< Flags the presence of Assistance Information
};

} // namespace srsgnb
