/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_integer.h"
#include "srsran/adt/to_array.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/uci/uci_info.h"
#include <set>

namespace srsran {

/// \brief Calculates the total rate matching output sequence length \f$E_{UCI}\f$, as per TS 38.212 Table 6.3.1.4-1.
inline unsigned get_pucch_format2_E_total(unsigned nof_prb, unsigned nof_symbols)
{
  return 16U * nof_symbols * nof_prb;
}

/// \brief Calculates the total rate matching output sequence length \f$E_{UCI}\f$, as per TS 38.212 Table 6.3.1.4-1.
inline unsigned get_pucch_format3_E_total(unsigned nof_prb, unsigned nof_symbols, bool pi2_bpsk)
{
  return (pi2_bpsk ? 12U : 24U) * nof_symbols * nof_prb;
}

/// \brief Calculates the total rate matching output sequence length \f$E_{UCI}\f$, as per TS 38.212 Table 6.3.1.4-1.
inline unsigned get_pucch_format4_E_total(unsigned spreading_factor, unsigned nof_symbols, bool pi2_bpsk)
{
  return (pi2_bpsk ? 12U : 24U) * nof_symbols / spreading_factor;
}

/// \brief Calculates the effective code rate for a PUCCH Format 2 transmission, for CSI of 1 part only.
/// \param[in] nof_prb           Transmission bandwidth in PRBs.
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

/// \brief Calculates the effective code rate for a PUCCH Format 3 transmission, for CSI Part 1 only.
/// \param[in] nof_prb           Transmission bandwidth in PRBs.
/// \param[in] nof_data_symbols  Number of symbols in the resource that contain data.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \return The effective code rate of the PUCCH Format 3 transmission.
inline float
pucch_format3_code_rate(unsigned nof_prb, unsigned nof_data_symbols, bool pi2_bpsk, unsigned nof_payload_bits)
{
  // As per Table 6.3.1.4.1-1, TS 38.212, for UCI of transmissions of CSI  Part 1 onlyof one part only,
  // \f$E_{UCI}\f$ = \f$E_{tot}\f$.
  const unsigned e_uci = get_pucch_format3_E_total(nof_prb, nof_data_symbols, pi2_bpsk);

  // As per TS 38.212 Sections 6.3.1.2.1 and 6.3.1.4.1, the parameter \f$E\f$ used to derive the number of
  // code-blocks is \f$E_{UCI}\f$.
  const unsigned payload_plus_crc_bits = nof_payload_bits + get_uci_nof_crc_bits(nof_payload_bits, e_uci);

  // PUCCH Format 3 channel bits are modulated as either QPSK or pi/2-BPSK and mapped to all REs
  // of the symbols that contain data.
  const unsigned nof_channel_bits = (nof_prb * NRE * nof_data_symbols * (pi2_bpsk ? 1 : 2));

  // Calculate code rate.
  return static_cast<float>(payload_plus_crc_bits) / static_cast<float>(nof_channel_bits);
}

/// \brief Calculates the effective code rate for a PUCCH Format 4 transmission, for CSI of 1 part only.
/// \param[in] spreading_factor  Spreading factor.
/// \param[in] nof_data_symbols  Number of symbols in the resource that contain data.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \return The effective code rate of the PUCCH Format 4 transmission.
inline float
pucch_format4_code_rate(unsigned spreading_factor, unsigned nof_data_symbols, bool pi2_bpsk, unsigned nof_payload_bits)
{
  // As per TS 38.212 Table 6.3.1.4.1-1, for UCI of transmissions of CSI of one part only,
  // \f$E_{UCI}\f$ = \f$E_{tot}\f$.
  const unsigned e_uci = get_pucch_format4_E_total(spreading_factor, nof_data_symbols, pi2_bpsk);

  // As per TS 38.212 Sections 6.3.1.2.1 and 6.3.1.4.1, the parameter \f$E\f$ used to derive the number of
  // code-blocks is \f$E_{UCI}\f$.
  const unsigned payload_plus_crc_bits = nof_payload_bits + get_uci_nof_crc_bits(nof_payload_bits, e_uci);

  // PUCCH Format 4 channel bits are modulated as either QPSK or pi/2-BPSK and mapped to all REs
  // of the symbols that contain data.
  const unsigned nof_channel_bits = (NRE * nof_data_symbols * (pi2_bpsk ? 1 : 2));

  // Calculate code rate.
  return static_cast<float>(payload_plus_crc_bits) / static_cast<float>(nof_channel_bits);
}

/// \brief Calculates the number of PRBs required for a given payload size for PUCCH Format 2.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] max_code_rate     Maximum code rate for PUCCH format 2; it corresponds to \c maxCodeRate, part of
///                              \c PUCCH-FormatConfig, TS 38.331.
/// \return The number of PRBs required for the transmission of nof_payload_bits with PUCCH format 2.
/// \remark The returned number of PRBs is not capped to the maximum value of \ref FORMAT2_MAX_NPRB; it's up to the
///         caller to perform this check.
/// \remark In case nof_payload_bits or nof_symbols is set to 0, the function returns 0.
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
/// \param[in] nof_prb        Transmission bandwidth in PRBs.
/// \param[in] nof_symbols    Transmission duration in symbols.
/// \param[in] max_code_rate  Maximum allowed PUCCH Format 2 code rate.
/// \return The maximum payload for a PUCCH Format 2 transmission.
inline unsigned get_pucch_format2_max_payload(unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate)
{
  const unsigned NOF_BITS_QPSK_SYMBOL = 2;
  // This is derived from the inequality (or constraint) on \f$M^{PUCCH}_{RB,min}\f$, in Section 9.2.5.2, TS 38.213; the
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

/// \brief Calculates the number of OFDM symbols filled with DM-RS in a PUCCH Format 3/4 resource.
/// \param[in] nof_symbols            Transmission duration in OFDM symbols.
/// \param[in] intraslot_freq_hopping Flag indicating if intra slot frequency hopping is enabled. Only used if
///                                   nof_symbols == 4.
/// \param[in] additional_dmrs        Flag indicating if additional DM-RS is enabled. Only used if nof_symbols > 10.
/// \return The number of DM-RS OFDM symbols within the PUCCH Format 3/4 transmission, as per TS 38.211
/// Table 6.4.1.3.3.2-1.
inline unsigned get_pucch_format3_4_nof_dmrs_symbols(bounded_integer<unsigned, 4, 14> nof_symbols,
                                                     bool                             intraslot_freq_hopping,
                                                     bool                             additional_dmrs)
{
  if (nof_symbols == 4) {
    return intraslot_freq_hopping ? 2 : 1;
  }
  if (nof_symbols < 10) {
    return 2;
  }
  return additional_dmrs ? 4 : 2;
}

/// \brief Calculates the number of PRBs required for a given payload size for PUCCH Format 3.
/// \param[in] nof_payload_bits       Total number of payload bits.
/// \param[in] nof_symbols            Transmission duration in symbols.
/// \param[in] max_code_rate          Maximum code rate for PUCCH format 3; it corresponds to \c maxCodeRate, part of
///                                   \c PUCCH-FormatConfig, TS 38.331.
/// \param[in] intraslot_freq_hopping Flag indicating if intra slot frequency hopping is enabled.
/// \param[in] additional_dmrs        Flag indicating if additional DM-RS is enabled.
/// \param[in] pi2_bpsk               Flag indicating if pi/2-BPSK modulation is used.
/// \return The number of PRBs required for the transmission of nof_payload_bits with PUCCH format 3.
/// \remark The returned number of PRBs is not capped to the maximum value of \ref FORMAT3_MAX_NPRB; it's up to the
///         caller to perform this check.
/// \remark In case nof_payload_bits is set to 0, the function returns 0.
inline unsigned get_pucch_format3_max_nof_prbs(unsigned                         nof_payload_bits,
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
  const unsigned mod_order = pi2_bpsk ? 1 : 2;

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
         (nof_dmrs_symbols * NRE * nof_prbs * mod_order)) {
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

/// \brief Calculates the maximum payload for a PUCCH Format 3 transmission.
/// \param[in] max_nof_prbs           Transmission bandwidth in PRBs.
/// \param[in] nof_symbols            Transmission duration in symbols.
/// \param[in] max_code_rate          Maximum allowed PUCCH Format 3 code rate.
/// \param[in] intraslot_freq_hopping Flag indicating if intra slot frequency hopping is enabled.
/// \param[in] additional_dmrs        Flag indicating if additional DM-RS is enabled.
/// \param[in] pi2_bpsk               Flag indicating if pi/2-BPSK modulation is used.
/// \return The maximum payload for a PUCCH Format 3 transmission.
inline unsigned get_pucch_format3_max_payload(unsigned max_nof_prbs,
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

/// Returns the number of possible spreading factors which is a function of the number of symbols.
inline unsigned format1_symb_to_spreading_factor(bounded_integer<unsigned, 4, 14> f1_symbols)
{
  // As per Table 6.3.2.4.1-1, TS 38.211.
  return f1_symbols.to_uint() / 2;
};

} // namespace srsran
