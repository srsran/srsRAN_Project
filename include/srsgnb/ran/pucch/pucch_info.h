/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/pucch/pucch_constants.h"
#include "srsgnb/ran/resource_block.h"

namespace srsran {

/// \brief Calculates the effective code rate for a PUCCH Format 2 transmission.
/// \param[in] nof_prb           Transmission bandwidth in PRB.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \return The effective code rate of the PUCCH Format 2 transmission.
constexpr float pucch_format2_code_rate(unsigned nof_prb, unsigned nof_symbols, unsigned nof_payload_bits)
{
  // PUCCH format 2 channel bits are modulated as QPSK and mapped to two of every three resource elements.
  const unsigned nof_channel_bits = (nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * 2);

  // Calculate code rate.
  return static_cast<float>(nof_payload_bits) / static_cast<float>(nof_channel_bits);
}

/// \brief Calculates the number of PRBs require for PUCCH format 2.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \param[in] max_nof_prbs      Maximum PUCCH format 2 bandwidth in PRB; it corresponds to \c nrofPRBs, part of
///                              \c PUCCH-format2, TS 38.331.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] max_code_rate     Maximum code rate for PUCCH format 2; it corresponds to \c maxCodeRate, part of
///                              \c PUCCH-FormatConfig, TS 38.331.
/// \return The number of PRBs required for the transmission of nof_payload_bits with PUCCH format 2.
inline unsigned
get_pucch_format2_nof_prbs(unsigned nof_payload_bits, unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate)
{
  if (max_nof_prbs == 1) {
    return 1;
  }
  if (nof_payload_bits == 0) {
    return 0;
  }

  // The maximum value is the maximum payload that does not require addition of CRC, for which this function would need
  // to be reviewed.
  srsgnb_assert(nof_payload_bits < 12, "Maximum number of payload bits supported is 11.");
  // TODO: Review this max payload.

  const unsigned NOF_BITS_QPSK_SYMBOL = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213. The
  // ceil operation guarantees that the number of PRBs is enough to satisfy the effective code rate constraint.
  const unsigned estimated_nof_prbs = static_cast<unsigned>(std::ceil(
      static_cast<float>(nof_payload_bits) /
      (static_cast<float>(pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * NOF_BITS_QPSK_SYMBOL) * max_code_rate)));

  return std::min(estimated_nof_prbs, max_nof_prbs);
}

/// \brief Calculates the maximum payload for a PUCCH Format 2 transmission.
/// \param[in] nof_prb        Transmission bandwidth in PRB.
/// \param[in] nof_symbols    Transmission duration in symbols.
/// \param[in] max_code_rate  Maximum allowed PUCCH Format2 code rate.
/// \return The maximum payload for a PUCCH Format 2 transmission.
inline unsigned get_pucch_format2_max_payload(unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate)
{
  // [Implementation-defined] This is the maximum supported payload is limited to 11 bits, which is the maximum payload
  // that does not require addition of CRC, for which this function would need to be reviewed.
  // TODO: Extend function to larger payloads.
  const unsigned MAX_SUPPORTED_PAYLOAD_BITS = 11;

  const unsigned NOF_BITS_QPSK_SYMBOL = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213; the
  // max payloads is obtained by using the floor operation from the maximum PHY capacity, given the PRBs, symbols and
  // max_code_rate.
  unsigned estimated_payload_bits = static_cast<unsigned>(std::floor(
      static_cast<float>(pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * NOF_BITS_QPSK_SYMBOL * max_nof_prbs) *
      max_code_rate));

  return std::min(estimated_payload_bits, MAX_SUPPORTED_PAYLOAD_BITS);
}

} // namespace srsran
