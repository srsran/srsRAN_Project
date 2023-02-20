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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/adt/to_array.h"
#include "srsgnb/mac/bsr_format.h"
#include "srsgnb/scheduler/scheduler_feedback_handler.h"

namespace srsran {

/// UL BSR Report for a single Logical Channel Group
struct lcg_bsr_report {
  lcg_id_t lcg_id;
  uint8_t  buffer_size;
};

/// List of reported UL BSRs
using lcg_bsr_report_list = static_vector<lcg_bsr_report, MAX_NOF_LCGS>;

/// Decode Short BSR
inline lcg_bsr_report decode_sbsr(byte_buffer_view payload)
{
  srsgnb_sanity_check(not payload.empty(), "Trying to decode SBSR but payload is empty.");
  lcg_bsr_report sbsr = {};
  sbsr.lcg_id         = uint_to_lcg_id((payload[0] & 0xe0U) >> 5U);
  sbsr.buffer_size    = payload[0] & 0x1fU;
  return sbsr;
}

struct long_bsr_report {
  uint8_t             bitmap; /// the first octet of LBSR and Long Trunc BSR
  lcg_bsr_report_list list;   /// one entry for each reported LCG
};

/// Decode Long BSR
long_bsr_report decode_lbsr(bsr_format format, byte_buffer_view payload);

/// Converts the buffer size field of a BSR (5 or 8-bit Buffer Size field) into Nof Bytes.
/// \param buff_size_index The buffer size field contained in the MAC PDU.
/// \param format The BSR format that determines the buffer size field length.
/// \return The actual buffer size level in Bytes.
uint32_t buff_size_field_to_bytes(size_t buff_size_index, bsr_format format);

/// Converts BSR MAC CE (See 38.321, 6.1.3.1) to Scheduler UL BSR event type.
ul_bsr_lcg_report make_sched_lcg_report(const lcg_bsr_report& ul_bsr_ce, bsr_format bsr_format);

} // namespace srsran
