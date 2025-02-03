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

#include "srsran/adt/static_vector.h"
#include "srsran/fapi/messages/base_message.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace fapi {

/// Reception data indication PDU information.
struct crc_ind_pdu {
  /// Maximum number of supported CBs in this message.
  static constexpr unsigned MAX_NUM_CB_PER_TTI_BYTES = 192;

  uint32_t                                         handle;
  rnti_t                                           rnti;
  uint8_t                                          rapid;
  uint8_t                                          harq_id;
  bool                                             tb_crc_status_ok;
  uint16_t                                         num_cb;
  static_vector<uint8_t, MAX_NUM_CB_PER_TTI_BYTES> cb_crc_status;
  int16_t                                          ul_sinr_metric;
  uint16_t                                         timing_advance_offset;
  int16_t                                          timing_advance_offset_ns;
  uint16_t                                         rssi;
  uint16_t                                         rsrp;
};

/// CRC indication message.
struct crc_indication_message : public base_message {
  /// Maximum number of supported CRCs in this message.
  static constexpr unsigned MAX_NUM_CRCS_PER_SLOT = 32;

  uint16_t                                          sfn;
  uint16_t                                          slot;
  static_vector<crc_ind_pdu, MAX_NUM_CRCS_PER_SLOT> pdus;
};

} // namespace fapi
} // namespace srsran
