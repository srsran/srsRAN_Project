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

#include "srsran/adt/static_vector.h"
#include "srsran/fapi/messages/base_message.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace fapi {

/// Reception data indication PDU information.
struct rx_data_indication_pdu {
  enum class pdu_tag_type : uint8_t { MAC_PDU, offset, custom = 100 };

  uint32_t     handle;
  rnti_t       rnti;
  uint8_t      rapid;
  uint8_t      harq_id;
  uint32_t     pdu_length;
  pdu_tag_type pdu_tag;
  //: TODO: non-conformant, revise
  const uint8_t* data;
};

/// Reception data indication message.
struct rx_data_indication_message : public base_message {
  /// Maximum number of supported UCI PDUs in this message.
  static constexpr unsigned MAX_NUM_ULSCH_PDUS_PER_SLOT = 64;

  uint16_t                                                           sfn;
  uint16_t                                                           slot;
  uint16_t                                                           control_length;
  static_vector<rx_data_indication_pdu, MAX_NUM_ULSCH_PDUS_PER_SLOT> pdus;
};

} // namespace fapi
} // namespace srsran
