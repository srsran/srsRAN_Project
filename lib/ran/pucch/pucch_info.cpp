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

#include "srsran/ran/pucch/pucch_info.h"

unsigned srsran::get_pucch_format2_max_nof_prbs(unsigned nof_payload_bits, unsigned nof_symbols, float max_code_rate)
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

  constexpr unsigned nof_bits_qpsk_symbol = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213. The
  // ceil operation guarantees that the number of PRBs is enough to satisfy the effective code rate constraint.
  return static_cast<unsigned>(std::ceil(
      static_cast<float>(payload_plus_crc_bits) /
      (static_cast<float>(pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * nof_bits_qpsk_symbol) * max_code_rate)));
}

unsigned srsran::get_pucch_format2_nof_prbs(unsigned nof_payload_bits,
                                            unsigned max_nof_prbs,
                                            unsigned nof_symbols,
                                            float    max_code_rate)
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

unsigned srsran::get_pucch_format2_max_payload(unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate)
{
  constexpr unsigned nof_bits_qpsk_symbol = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.2, TS 38.213; the
  // max payloads is obtained by using the floor operation from the maximum PHY capacity, given the PRBs, symbols and
  // max_code_rate.
  // NOTE: The maximum number of bits that can be carried by a PUCCH Format 2 resource is 409, which is obtained for 16
  // PRBs, 2 symbols and 0.8 max code rate.
  unsigned estimated_pucch_f2_capacity = static_cast<unsigned>(std::floor(
      static_cast<float>(pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * nof_bits_qpsk_symbol * max_nof_prbs) *
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

unsigned srsran::get_pucch_format3_max_nof_prbs(unsigned                         nof_payload_bits,
                                                bounded_integer<unsigned, 4, 14> nof_symbols,
                                                float                            max_code_rate,
                                                bool                             intraslot_freq_hopping,
                                                bool                             additional_dmrs,
                                                bool                             pi2_bpsk)
{
  static constexpr auto valid_num_prbs          = to_array({1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 16});
  const auto            round_to_valid_nof_prbs = [](unsigned& value) {
    if (value > pucch_constants::FORMAT3_MAX_NPRB) {
      return;
    }

    // Round up to the nearest valid number of PRBs for PUCCH Format 3.
    const int* it = std::lower_bound(valid_num_prbs.begin(), valid_num_prbs.end(), value);
    if (it != valid_num_prbs.end()) {
      value = *it;
    }
  };

  if (nof_payload_bits == 0) {
    return 0;
  }

  const unsigned nof_dmrs_symbols =
      get_pucch_format3_4_nof_dmrs_symbols(nof_symbols, intraslot_freq_hopping, additional_dmrs);
  const unsigned nof_data_symbols = nof_symbols.value() - nof_dmrs_symbols;
  const unsigned mod_order        = pi2_bpsk ? 1 : 2;

  // Compute the number of PRBs first without taking into account the CRC bits.
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.2, TS 38.213. The
  // ceil operation guarantees that the number of PRBs is enough to satisfy the effective code rate constraint.
  unsigned nof_prbs = static_cast<unsigned>(
      std::ceil(static_cast<float>(nof_payload_bits) /
                (static_cast<float>((nof_symbols.value() - nof_dmrs_symbols) * NRE * mod_order) * max_code_rate)));

  round_to_valid_nof_prbs(nof_prbs);

  // The resulting number of PRBs is too big for PUCCH Format 3, so just return.
  if (nof_prbs > pucch_constants::FORMAT3_MAX_NPRB) {
    return nof_prbs;
  }

  // Compute the total number of bits (including CRC) using the resulting PRB number.
  unsigned e_uci = get_pucch_format3_E_total(nof_prbs, nof_symbols.value(), pi2_bpsk);
  // As per Sections 6.3.1.2.1 and 6.3.1.4.1, TS 38.212, the parameter \f$E\f$ used to derive the number of
  // code-blocks is \f$E_{UCI}\f$.
  unsigned payload_plus_crc_bits = nof_payload_bits + get_uci_nof_crc_bits(nof_payload_bits, e_uci);

  // Check that the number of PRBs can hold the total number of bits after rate matching.
  while (static_cast<unsigned>(std::ceil(static_cast<float>(payload_plus_crc_bits) / max_code_rate)) >
         (nof_data_symbols * NRE * nof_prbs * mod_order)) {
    round_to_valid_nof_prbs(++nof_prbs);

    // The resulting number of PRBs is too big for PUCCH Format 3, so just return.
    if (nof_prbs > pucch_constants::FORMAT3_MAX_NPRB) {
      return nof_prbs;
    }

    // Compute the total number of bits (including CRC) using the resulting PRB number.
    e_uci = get_pucch_format3_E_total(nof_prbs, nof_symbols.value(), pi2_bpsk);
    // As per Sections 6.3.1.2.1 and 6.3.1.4.1, TS 38.212, the parameter \f$E\f$ used to derive the number of
    // code-blocks is \f$E_{UCI}\f$.
    payload_plus_crc_bits = nof_payload_bits + get_uci_nof_crc_bits(nof_payload_bits, e_uci);
  }
  return nof_prbs;
}

unsigned srsran::get_pucch_format3_nof_prbs(unsigned nof_payload_bits,
                                            unsigned max_nof_prbs,
                                            unsigned nof_symbols,
                                            float    max_code_rate,
                                            bool     intraslot_freq_hopping,
                                            bool     additional_dmrs,
                                            bool     pi2_bpsk)
{
  if (nof_payload_bits == 0 or nof_symbols == 0) {
    return 0;
  }
  if (max_nof_prbs == 1) {
    return 1;
  }

  const unsigned estimated_nof_prbs = get_pucch_format3_max_nof_prbs(
      nof_payload_bits, nof_symbols, max_code_rate, intraslot_freq_hopping, additional_dmrs, pi2_bpsk);

  return std::min(estimated_nof_prbs, max_nof_prbs);
}

unsigned srsran::get_pucch_format3_max_payload(unsigned max_nof_prbs,
                                               unsigned nof_symbols,
                                               float    max_code_rate,
                                               bool     intraslot_freq_hopping,
                                               bool     additional_dmrs,
                                               bool     pi2_bpsk)
{
  const unsigned nof_dmrs_symbols =
      get_pucch_format3_4_nof_dmrs_symbols(nof_symbols, intraslot_freq_hopping, additional_dmrs);
  const unsigned mod_order = pi2_bpsk ? 1 : 2;

  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213; the
  // max payloads is obtained by using the floor operation from the maximum PHY capacity, given the PRBs, symbols and
  // max_code_rate.
  const unsigned estimated_pucch_f3_capacity = static_cast<unsigned>(std::floor(
      static_cast<float>((nof_symbols - nof_dmrs_symbols) * NRE * mod_order * max_nof_prbs) * max_code_rate));

  // Get the payload depending on the estimated PUCCH F3 capacity (which we define as the nof bits that the PUCCH F3 can
  // carry).

  // Case: no CRC for payload <= 11 bits.
  constexpr unsigned min_capacity_for_more_than_11_bit_payload = 18U;
  constexpr unsigned max_payload_without_crc_addition          = 11U;
  if (estimated_pucch_f3_capacity < min_capacity_for_more_than_11_bit_payload) {
    return std::min(estimated_pucch_f3_capacity, max_payload_without_crc_addition);
  }

  // Case: 6-bit CRC for 12 <= payload <= 19 bits.
  constexpr unsigned min_capacity_for_more_than_19_bit_payload = 31U;
  constexpr unsigned max_payload_with_6_bit_crc_addition       = 19U;
  constexpr unsigned short_crc_length                          = 6U;
  if (estimated_pucch_f3_capacity < min_capacity_for_more_than_19_bit_payload) {
    return std::min(estimated_pucch_f3_capacity - short_crc_length, max_payload_with_6_bit_crc_addition);
  }

  // Case: 11-bit CRC for payload >= 20.
  // Compute the total number of bits (including CRC) using the resulting PRB number.
  const unsigned     e_uci           = get_pucch_format3_E_total(max_nof_prbs, nof_symbols, pi2_bpsk);
  constexpr unsigned long_crc_length = 11U;
  const unsigned     nof_crc_bits    = get_uci_nof_crc_bits(estimated_pucch_f3_capacity - long_crc_length, e_uci);
  return std::min(estimated_pucch_f3_capacity - long_crc_length, estimated_pucch_f3_capacity - nof_crc_bits);
}

unsigned srsran::get_pucch_format4_max_payload(unsigned         nof_symbols,
                                               float            max_code_rate,
                                               bool             intraslot_freq_hopping,
                                               bool             additional_dmrs,
                                               bool             pi2_bpsk,
                                               pucch_f4_occ_len occ_length)
{
  const unsigned nof_dmrs_symbols =
      get_pucch_format3_4_nof_dmrs_symbols(nof_symbols, intraslot_freq_hopping, additional_dmrs);
  const unsigned mod_order        = pi2_bpsk ? 1 : 2;
  const float    spreading_factor = static_cast<unsigned>(occ_length);

  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.1, TS 38.213; the
  // max payloads is obtained by using the floor operation from the maximum PHY capacity, given the PRBs, symbols and
  // max_code_rate.
  // NOTE: The maximum number of bits that can be carried by a PUCCH Format 4 resource is 115, which is obtained for 14
  // symbols, a spreading factor of 2, QPSK, no additional DM-RS and 0.8 max code rate.
  const unsigned estimated_pucch_f4_capacity = static_cast<unsigned>(std::floor(
      static_cast<float>((nof_symbols - nof_dmrs_symbols) * NRE * mod_order) * max_code_rate / spreading_factor));

  // Get the payload depending on the estimated PUCCH F4 capacity (which we define as the nof bits that the PUCCH F4 can
  // carry).
  // NOTE: Given the max capacity of PUCCH, which is 115 bits (see above), the UCI in PUCCH F4 can have max 1 codeword
  // (as per Section 6.3.1.2.1, TS 38.212); this implies only 6-bit and 11-bit can be added as CRC to the PUCCH F4
  // payload.

  // Case: no CRC for payload <= 11 bits.
  constexpr unsigned min_capacity_for_more_than_11_bit_payload = 18U;
  constexpr unsigned max_payload_without_crc_addition          = 11U;
  if (estimated_pucch_f4_capacity < min_capacity_for_more_than_11_bit_payload) {
    return std::min(estimated_pucch_f4_capacity, max_payload_without_crc_addition);
  }

  // Case: 6-bit CRC for 12 <= payload <= 19 bits.
  constexpr unsigned min_capacity_for_more_than_19_bit_payload = 31U;
  constexpr unsigned max_payload_with_6_bit_crc_addition       = 19U;
  constexpr unsigned short_crc_length                          = 6U;
  if (estimated_pucch_f4_capacity < min_capacity_for_more_than_19_bit_payload) {
    return std::min(estimated_pucch_f4_capacity - short_crc_length, max_payload_with_6_bit_crc_addition);
  }

  // Case: 11-bit CRC for payload >= 20.
  const unsigned long_crc_length = 11U;
  return estimated_pucch_f4_capacity - long_crc_length;
}
