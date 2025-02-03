/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/messages/base_message.h"

namespace srsran {
namespace fapi {

/// Downlink TTI response pdu information.
struct dl_tti_response_pdu {
  uint16_t pdu_index;
  uint8_t  cw;
  uint32_t tb_crc;
};

/// Downlink TTI response message.
struct dl_tti_response_message : public base_message {
  uint16_t                                              sfn;
  uint16_t                                              slot;
  uint16_t                                              num_pdus;
  std::array<dl_tti_response_pdu, MAX_DL_PDUS_PER_SLOT> pdus;
};

} // namespace fapi
} // namespace srsran
