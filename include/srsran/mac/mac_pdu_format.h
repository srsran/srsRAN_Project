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

#include "lcid_dl_sch.h"

namespace srsran {

constexpr unsigned FIXED_SIZED_MAC_CE_SUBHEADER_SIZE = 1;
constexpr unsigned MAC_SDU_SUBHEADER_LENGTH_THRES    = 256;
constexpr unsigned MIN_MAC_SDU_SUBHEADER_SIZE        = 2;
constexpr unsigned MAX_MAC_SDU_SUBHEADER_SIZE        = 3;

/// \brief Derive MAC SDU subheader size in bytes as per TS38.321, Section 6.1.
inline unsigned get_mac_sdu_subheader_size(unsigned payload)
{
  return payload == 0                                ? 0
         : payload >= MAC_SDU_SUBHEADER_LENGTH_THRES ? MAX_MAC_SDU_SUBHEADER_SIZE
                                                     : MIN_MAC_SDU_SUBHEADER_SIZE;
}

/// \brief Derive MAC SDU total size in bytes (includes subheader).
inline unsigned get_mac_sdu_required_bytes(unsigned payload)
{
  return payload + get_mac_sdu_subheader_size(payload);
}

inline unsigned get_mac_sdu_payload_size(unsigned mac_sdu_size)
{
  if (mac_sdu_size <= MAC_SDU_SUBHEADER_LENGTH_THRES - 1 + MIN_MAC_SDU_SUBHEADER_SIZE) {
    return mac_sdu_size - MIN_MAC_SDU_SUBHEADER_SIZE;
  }
  return mac_sdu_size - MAX_MAC_SDU_SUBHEADER_SIZE;
}

} // namespace srsran
