/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/fapi/messages/base_message.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/support/shared_transport_block.h"

namespace srsran {
namespace fapi {

/// Transmission data request PDU information.
struct tx_data_req_pdu {
  tx_data_req_pdu() = default;

  tx_data_req_pdu(uint16_t pdu_index_, uint8_t cw_index_, shared_transport_block pdu_) :
    pdu_index(pdu_index_), cw_index(cw_index_), pdu(std::move(pdu_))
  {
  }

  uint16_t               pdu_index;
  uint8_t                cw_index;
  shared_transport_block pdu;
};

/// Transmission request message.
struct tx_data_request_message : public base_message {
  uint16_t                                                sfn;
  uint16_t                                                slot;
  uint16_t                                                control_length;
  static_vector<tx_data_req_pdu, MAX_PDSCH_PDUS_PER_SLOT> pdus;
};

} // namespace fapi
} // namespace srsran
