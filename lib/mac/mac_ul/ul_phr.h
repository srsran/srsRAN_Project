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

namespace srsran {

/// UL Power Headroom Report (PHR).
struct phr_report {
  /// Indicates the power headroom level. This field maps to PH in table 6.1.3.8-1 of TS 38.321.
  unsigned ph;
  /// UE configured maximum output power used in computation of Power Headroom level. See TS 38.321, 6.1.3.8 and
  /// TS 38.213, 7.1.1. This field maps to P_CMAX,f,c in table 6.1.3.8-2 of TS 38.321.
  unsigned p_cmax;
};

/// \brief Decode Single Entry PHR.
/// \remark See TS 38.321, 6.1.3.8.
inline phr_report decode_se_phr(byte_buffer_view payload)
{
  srsran_sanity_check(not payload.empty(), "Trying to decode SE-PHR but payload is empty.");
  srsran_sanity_check(payload.length() == 2, "Invalid payload length={} while decoding SE-PHR.", payload.length());
  phr_report se_phr = {};
  se_phr.ph         = payload[0] & 0b00111111U;
  se_phr.p_cmax     = payload[1] & 0b00111111U;
  return se_phr;
}

} // namespace srsran
