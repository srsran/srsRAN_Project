/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/uci/uci_info.h"

namespace srsran {

/// \brief Calculates the total rate matching output sequence length \f$E_{UCI}\f$, as per Table 6.3.1.4-1 TS 38.212.
inline unsigned get_pucch_format2_E_total(unsigned nof_prb, unsigned nof_symbols)
{
  return 16U * nof_symbols * nof_prb;
}

/// \brief Calculates the effective code rate for a PUCCH Format 2 transmission, for CSI of 1 part only.
/// \param[in] nof_prb           Transmission bandwidth in PRB.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \return The effective code rate of the PUCCH Format 2 transmission.
inline float pucch_format2_code_rate(unsigned nof_prb, unsigned nof_symbols, unsigned nof_payload_bits)
{
  // As per Table 6.3.1.4.1-1, TS 38.212, for UCI of transmissions of CSI of one part only,
  // \f$E_{UCI}\f$ = \f$E_{tot}\f$.
  // TODO: replace this with a function that returns the e_uci for the general case.
  const unsigned e_uci = get_pucch_format2_E_total(nof_prb, nof_symbols);

  // As per Sections 6.3.1.2.1 and 6.3.1.4.1, TS 38.212, the parameter \f$E\f$ used to derive the number of
  // code-blocks is \f$E_{UCI}\f$.
  const unsigned payload_plus_crc_bits = nof_payload_bits + get_uci_nof_crc_bits(nof_payload_bits, e_uci);

  // PUCCH format 2 channel bits are modulated as QPSK and mapped to two of every three resource elements.
  const unsigned nof_channel_bits = (nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * 2);

  // Calculate code rate.
  return static_cast<float>(payload_plus_crc_bits) / static_cast<float>(nof_channel_bits);
}

/// \brief Calculates the number of PRBs required for a given payload size for PUCCH format 2.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] max_code_rate     Maximum code rate for PUCCH format 2; it corresponds to \c maxCodeRate, part of
///                              \c PUCCH-FormatConfig, TS 38.331.
/// \return The number of PRBs required for the transmission of nof_payload_bits with PUCCH format 2.
/// \remark The returned number of PRBs is not capped to the maximum value of \ref FORMAT2_MAX_NPRB; it's up to the
///         caller to perform this check.
inline unsigned get_pucch_format2_max_nof_prbs(unsigned nof_payload_bits, unsigned nof_symbols, float max_code_rate)
{
  if (nof_payload_bits == 0 or nof_symbols == 0) {
    return 0;
  }

  // We compute \ref e_uci using the max PRBs, as we don't know the actual nof PRBs (computing this is the objective of
  // this function). NOTE: as per Table 6.3.1.4-1 and Table 6.3.1.4.1-1, TS 38.212, the maximum value of \f$E_{UCI}\f$
  // for PUCCH Format 2 is 512. As per Section 6.3.1.2.1, TS 38.212, any value less than 1088 is irrelevant for the CRC
  // length computation, therefore using the maximum number of PRBs instead of the actual number doesn't affect the
  // result.
  const unsigned max_nof_pucch_f2_prbs = 16;
  const unsigned e_uci                 = get_pucch_format2_E_total(max_nof_pucch_f2_prbs, nof_symbols);

  // As per Sections 6.3.1.2.1 and 6.3.1.4.1, TS 38.212, the parameter \f$E\f$ used to derive the number of
  // code-blocks is \f$E_{UCI}\f$.
  const unsigned payload_plus_crc_bits = nof_payload_bits + get_uci_nof_crc_bits(nof_payload_bits, e_uci);

  const unsigned NOF_BITS_QPSK_SYMBOL = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213. The
  // ceil operation guarantees that the number of PRBs is enough to satisfy the effective code rate constraint.
  return static_cast<unsigned>(std::ceil(
      static_cast<float>(payload_plus_crc_bits) /
      (static_cast<float>(pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * NOF_BITS_QPSK_SYMBOL) * max_code_rate)));
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

  const unsigned estimated_nof_prbs = get_pucch_format2_max_nof_prbs(nof_payload_bits, nof_symbols, max_code_rate);

  return std::min(estimated_nof_prbs, max_nof_prbs);
}

/// \brief Calculates the maximum payload for a PUCCH Format 2 transmission.
/// \param[in] nof_prb        Transmission bandwidth in PRB.
/// \param[in] nof_symbols    Transmission duration in symbols.
/// \param[in] max_code_rate  Maximum allowed PUCCH Format2 code rate.
/// \return The maximum payload for a PUCCH Format 2 transmission.
inline unsigned get_pucch_format2_max_payload(unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate)
{
  const unsigned NOF_BITS_QPSK_SYMBOL = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213; the
  // max payloads is obtained by using the floor operation from the maximum PHY capacity, given the PRBs, symbols and
  // max_code_rate.
  // NOTE: The maximum number of bits that can be carried by a PUCCH Format 2 resource is 409, which is obtained for 16
  // PRBs, 2 symbols and 0.8 max code rate.
  unsigned estimated_pucch_f2_capacity = static_cast<unsigned>(std::floor(
      static_cast<float>(pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * NOF_BITS_QPSK_SYMBOL * max_nof_prbs) *
      max_code_rate));

  // Get the payload depending on the estimated PUCCH F2 capacity (which we define as the nof bits that the PUCCH F2 can
  // carry).
  // NOTE: Given the max capacity of PUCCH, which is 409 bits (see above), the UCI in PUCCH F2 can have max 1 codeword
  // (as per Section 6.3.1.2.1, TS 38.212); this implies only 6-bit and 11-bit can be added as CRC to the PUCCH F2
  // payload.

  // Case: no CRC for payload <= 11 bits.
  const unsigned min_capacity_for_more_than_11_bit_payload = 18U;
  const unsigned max_payload_without_crc_addition          = 11U;
  if (estimated_pucch_f2_capacity < min_capacity_for_more_than_11_bit_payload) {
    return std::min(estimated_pucch_f2_capacity, max_payload_without_crc_addition);
  }
  // Case: 6-bit CRC for 12 <= payload <= 19 bits.
  const unsigned min_capacity_for_more_than_19_bit_payload = 31U;
  const unsigned max_payload_with_6_bit_crc_addition       = 19U;
  const unsigned short_crc_length                          = 6U;
  if (estimated_pucch_f2_capacity < min_capacity_for_more_than_19_bit_payload) {
    return std::min(estimated_pucch_f2_capacity - short_crc_length, max_payload_with_6_bit_crc_addition);
  }

  // Case: 11-bit CRC for payload >= 20.
  const unsigned long_crc_length = 11U;
  return estimated_pucch_f2_capacity - long_crc_length;
}

} // namespace srsran
