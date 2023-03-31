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

#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/resource_block.h"

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

/// \brief Calculates the number of PRBs required for a given payload size for PUCCH format 2.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] max_code_rate     Maximum code rate for PUCCH format 2; it corresponds to \c maxCodeRate, part of
///                              \c PUCCH-FormatConfig, TS 38.331.
/// \return The number of PRBs required for the transmission of nof_payload_bits with PUCCH format 2. If the estimated
/// number of PRBs is more than the maximum allowed value of 16, the function returns -1.
inline int get_pucch_format2_max_nof_prbs(unsigned nof_payload_bits, unsigned nof_symbols, float max_code_rate)
{
  if (nof_payload_bits == 0 or nof_symbols == 0) {
    return 0;
  }

  // The maximum value is the maximum payload that does not require addition of CRC, for which this function would need
  // to be reviewed.
  srsran_assert(nof_payload_bits < 12, "Maximum number of payload bits supported is 11.");
  // TODO: Review this max payload.

  const unsigned NOF_BITS_QPSK_SYMBOL = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213. The
  // ceil operation guarantees that the number of PRBs is enough to satisfy the effective code rate constraint.
  const unsigned estimated_nof_prbs = static_cast<unsigned>(std::ceil(
      static_cast<float>(nof_payload_bits) /
      (static_cast<float>(pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * NOF_BITS_QPSK_SYMBOL) * max_code_rate)));

  return estimated_nof_prbs <= pucch_constants::FORMAT2_MAX_NPRB ? static_cast<int>(estimated_nof_prbs) : -1;
}

/// \brief Calculates the num. of PRBs (capped to the configured max nof PRBs) given the PUCCH Format 2 payload size.
///
/// With respect to \ref get_pucch_format2_max_nof_prbs, this function caps the number of PRBs to the value configured
/// for PUCCH resource Format 2, which can be less than 16. Note that the returned number of PRBs might not be enough to
/// allocate the payload size.
///
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \param[in] max_nof_prbs      Maximum PUCCH format 2 bandwidth in PRB; it corresponds to \c nrofPRBs, part of
///                              \c PUCCH-format2, TS 38.331.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] max_code_rate     Maximum code rate for PUCCH format 2; it corresponds to \c maxCodeRate, part of
///                              \c PUCCH-FormatConfig, TS 38.331.
/// \return The number of PRBs for the PUCCH format 2 resource. Note that the returned number of PRBs might not be
/// enough to allocate the payload size; it's the caller's responsibility to perform this check.
inline unsigned
get_pucch_format2_nof_prbs(unsigned nof_payload_bits, unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate)
{
  if (nof_payload_bits == 0 or nof_symbols == 0) {
    return 0;
  }
  if (max_nof_prbs == 1) {
    return 1;
  }

  // The maximum value is the maximum payload that does not require addition of CRC, for which this function would need
  // to be reviewed.
  srsran_assert(max_nof_prbs <= pucch_constants::FORMAT2_MAX_NPRB, "Maximum number of payload bits supported is 11.");
  // TODO: Review this max payload.

  const int      estimated_nof_prbs = get_pucch_format2_max_nof_prbs(nof_payload_bits, nof_symbols, max_code_rate);
  const unsigned nof_prbs =
      estimated_nof_prbs < 0 ? pucch_constants::FORMAT2_MAX_NPRB : static_cast<unsigned>(estimated_nof_prbs);

  return std::min(nof_prbs, max_nof_prbs);
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
