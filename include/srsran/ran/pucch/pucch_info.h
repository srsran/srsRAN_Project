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

#pragma once

#include "srsran/adt/bounded_integer.h"
#include "srsran/adt/to_array.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/uci/uci_info.h"

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
unsigned get_pucch_format2_max_nof_prbs(unsigned nof_payload_bits, unsigned nof_symbols, float max_code_rate);

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
unsigned
get_pucch_format2_nof_prbs(unsigned nof_payload_bits, unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate);

/// \brief Calculates the maximum payload for a PUCCH Format 2 transmission.
/// \param[in] max_nof_prbs  Transmission bandwidth in PRBs.
/// \param[in] nof_symbols   Transmission duration in symbols.
/// \param[in] max_code_rate Maximum allowed PUCCH Format 2 code rate.
/// \return The maximum payload (in bits) for a PUCCH Format 2 transmission.
unsigned get_pucch_format2_max_payload(unsigned max_nof_prbs, unsigned nof_symbols, float max_code_rate);

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
unsigned get_pucch_format3_max_nof_prbs(unsigned                         nof_payload_bits,
                                        bounded_integer<unsigned, 4, 14> nof_symbols,
                                        float                            max_code_rate,
                                        bool                             intraslot_freq_hopping,
                                        bool                             additional_dmrs,
                                        bool                             pi2_bpsk);

/// \brief Calculates the num. of PRBs (capped to the configured max nof PRBs) given the PUCCH Format 3 payload size.
///
/// With respect to \ref get_pucch_format3_max_nof_prbs, this function caps the number of PRBs to the value configured
/// for PUCCH resource Format 3, which can be less than 16. Note that the returned number of PRBs might not be enough to
/// allocate the payload size.
///
/// \param[in] nof_payload_bits       Total number of payload bits.
/// \param[in] max_nof_prbs           Maximum PUCCH format 3 bandwidth in PRB; it corresponds to \c nrofPRBs, part of
///                                   \c PUCCH-format3, TS 38.331.
/// \param[in] nof_symbols            Transmission duration in symbols.
/// \param[in] max_code_rate          Maximum code rate for PUCCH format 3; it corresponds to \c maxCodeRate, part of
///                                   \c PUCCH-FormatConfig, TS 38.331.
/// \param[in] intraslot_freq_hopping Flag indicating if intra slot frequency hopping is enabled.
/// \param[in] additional_dmrs        Flag indicating if additional DM-RS is enabled.
/// \param[in] pi2_bpsk               Flag indicating if pi/2-BPSK modulation is used.
/// \return The number of PRBs for the PUCCH format 3 resource. Note that the returned number of PRBs might not be
/// enough to allocate the payload size; it's the caller's responsibility to perform this check.
unsigned get_pucch_format3_nof_prbs(unsigned nof_payload_bits,
                                    unsigned max_nof_prbs,
                                    unsigned nof_symbols,
                                    float    max_code_rate,
                                    bool     intraslot_freq_hopping,
                                    bool     additional_dmrs,
                                    bool     pi2_bpsk);

/// \brief Calculates the maximum payload for a PUCCH Format 3 transmission.
/// \param[in] max_nof_prbs           Transmission bandwidth in PRBs.
/// \param[in] nof_symbols            Transmission duration in symbols.
/// \param[in] max_code_rate          Maximum allowed PUCCH Format 3 code rate.
/// \param[in] intraslot_freq_hopping Flag indicating if intra slot frequency hopping is enabled.
/// \param[in] additional_dmrs        Flag indicating if additional DM-RS is enabled.
/// \param[in] pi2_bpsk               Flag indicating if pi/2-BPSK modulation is used.
/// \return The maximum payload (in bits) for a PUCCH Format 3 transmission.
unsigned get_pucch_format3_max_payload(unsigned max_nof_prbs,
                                       unsigned nof_symbols,
                                       float    max_code_rate,
                                       bool     intraslot_freq_hopping,
                                       bool     additional_dmrs,
                                       bool     pi2_bpsk);

/// \brief Calculates the maximum payload for a PUCCH Format 4 transmission.
/// \param[in] nof_symbols            Transmission duration in symbols.
/// \param[in] max_code_rate          Maximum allowed PUCCH Format 4 code rate.
/// \param[in] intraslot_freq_hopping Flag indicating if intra slot frequency hopping is enabled.
/// \param[in] additional_dmrs        Flag indicating if additional DM-RS is enabled.
/// \param[in] pi2_bpsk               Flag indicating if pi/2-BPSK modulation is used.
/// \param[in] occ_length             OCC length.
/// \return The maximum payload (in bits) for a PUCCH Format 4 transmission.
unsigned get_pucch_format4_max_payload(unsigned         nof_symbols,
                                       float            max_code_rate,
                                       bool             intraslot_freq_hopping,
                                       bool             additional_dmrs,
                                       bool             pi2_bpsk,
                                       pucch_f4_occ_len occ_length);

/// Returns the number of possible spreading factors which is a function of the number of symbols.
inline unsigned format1_symb_to_spreading_factor(bounded_integer<unsigned, 4, 14> f1_symbols)
{
  // As per Table 6.3.2.4.1-1, TS 38.211.
  return f1_symbols.to_uint() / 2;
}

} // namespace srsran
