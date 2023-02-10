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

#include "srsgnb/support/srsgnb_assert.h"

namespace srsgnb {

/// HARQ values for UCI PUCCH Format 0 or Format 1.
enum class uci_pucch_f0_or_f1_harq_values { nack, ack, dtx };

inline constexpr const char* to_string(uci_pucch_f0_or_f1_harq_values value)
{
  switch (value) {
    case uci_pucch_f0_or_f1_harq_values::nack:
      return "nack";
    case uci_pucch_f0_or_f1_harq_values::ack:
      return "ack";
    case uci_pucch_f0_or_f1_harq_values::dtx:
      return "dtx";
    default:
      srsgnb_assert(0, "invalid UCI PUCCH Format 0/1 HARQ value={}", value);
      break;
  }
  return "";
}

/// \brief Maps the integer \c beta_offset value for HARQ-ACK reporting into the corresponding float value.
///
/// The integer \c beta_offset value is passed by the \c PUSCH-Config, TS 38.331, and the mapping into the corresponding
/// float value is determined as per Table 9.3-1, TS 38.213.
/// \param beta_uint_val is the integer value as per \c BetaOffsets, TS 38.331.
/// \return The corresponding float value as per Table 9.3-1, TS 38.213.
float beta_harq_ack_to_float(unsigned beta_uint_val);

/// \brief Maps the integer \c beta_offset value for CSI reporting into the corresponding float value.
///
/// The integer \c beta_offset value is passed by the \c PUSCH-Config, TS 38.331, and the mapping into the corresponding
/// float value is determined as per Table 9.3-2, TS 38.213.
/// \param beta_uint_val is the integer value as per \c BetaOffsets, TS 38.331.
/// \return The corresponding float value as per Table 9.3-2, TS 38.213.
float beta_csi_to_float(unsigned beta_uint_val);

/// Indicates CRC result on UCI containing HARQ/CSI Part 1/CSI Part 2 data.
enum class uci_pusch_or_pucch_f2_3_4_detection_status {
  /// Used when CRC was attached.
  crc_pass = 1,
  /// Used when UE is expected to attach CRC.
  crc_failure,
  /// Undetected UCI.
  dtx,
  /// Indicates UCI detection.
  no_dtx,
  /// Indicates that PHY did not check for UCI DTX.
  dtx_not_checked
};

} // namespace srsgnb
