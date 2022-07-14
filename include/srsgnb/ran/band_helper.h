/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_BAND_HELPER_H
#define SRSGNB_BAND_HELPER_H

/// This header is currently used only by the MAC to compute extra SSB parameters (needed for scheduling) from those
/// provided by DU.

#include <stdint.h>

namespace srsgnb {

enum class duplex_mode;
enum class subcarrier_spacing;
enum class ssb_pattern_case;

namespace band_helper {

/// \brief     Gets the NR band duplex mode.
/// \param[in] band Given band.
/// \return    A valid duplex_mode if the band is valid, duplex_mode::INVALID otherwise.
duplex_mode get_duplex_mode(uint16_t band);

/// \brief     Gets the lowest band that includes a given Downlink ARFCN.
/// \remark    Some bands can be subset of others, e.g., band 2 is a subset of band 25.
/// \param[in] arfcn Given Downlink ARFCN.
/// \return    The band number if the ARFCN is bounded in a band, UINT16_MAX otherwise.
uint16_t get_band_from_dl_arfcn(uint32_t arfcn);

/// @brief Get the respective UL ARFCN of a DL ARFCN.
///
/// For paired spectrum (FDD) the function returns the respective ARFCN in the same band.
/// For unparied spectrum (TDD) the function returns the same ARFCN.
///
/// \param[in] dl_arfcn The DL ARFCN.
/// \return uint32_t the UL ARFCN.
uint32_t get_ul_arfcn_from_dl_arfcn(uint32_t dl_arfcn);

/// \brief Return frequency of given NR-ARFCN in Hz as per TS38.104 5.4.2.1.
/// \param[in] nr_arfcn Given NR-AFCN.
/// \return The frequency in Hz.
double nr_arfcn_to_freq(uint32_t nr_arfcn);

/// \brief Calculates NR-ARFCN of a given frequency as per TS38.104 5.4.2.1.
/// \param[in] freq Given Frequency in Hz.
/// \return The NR-AFCN.
uint32_t freq_to_nr_arfcn(double freq);

/// \brief     Selects the SSB pattern case according to the band number and subcarrier spacing.
/// \remark    Described by TS 38.101-1 Table 5.4.3.3-1: Applicable SS raster entries per operating band.
/// \param[in] band NR Band number.
/// \param[in] scs SSB Subcarrier spacing.
/// \return    The SSB pattern case if band and subcarrier spacing match, invalid otherwise.
ssb_pattern_case get_ssb_pattern(uint16_t band, subcarrier_spacing scs);

/// \brief Selects the lowest SSB subcarrier spacing valid for this band.
/// \param[in] band NR band number.
/// \return The SSB subcarrier spacing.
subcarrier_spacing get_lowest_ssb_scs(uint16_t band);

/// \brief     Returns boolean indicating whether the band is in paired spectrum.
/// \remark    Paired spectrum is FDD, unpaired spectrum is TDD, SUL, SDL.
/// \param[in] band Given band.
/// \return    true for paired specrum, false otherwise.
bool is_paired_spectrum(uint16_t band);

/// @brief Compute the absolute pointA for a NR carrier from its bandwidth and the center frequency
///
/// @param nof_prb Carrier bandwidth in number of RB
/// @param center_freq double Frequency in Hz
/// @return Absolute Point A frequency in Hz
double get_abs_freq_point_a_from_center_freq(uint32_t nof_prb, double center_freq);

/// \brief Compute the absolute frequency point A for a arfcn
///
/// \param[in] band nr frequency band.
/// \param[in] nof_prb Number of PRBs.
/// \param[in] arfcn Given ARFCN.
/// \return frequency point A in arfcn notation.
uint32_t get_abs_freq_point_a_arfcn(uint32_t nof_prb, uint32_t arfcn);

/// \brief Compute the center frequency for a NR carrier from its bandwidth and the absolute pointA.
///
/// \param nof_prb Carrier bandwidth in number of RB.
/// \param freq_point_a_arfcn Absolute Point A frequency ARFCN.
/// \return double Frequency in Hz.
double get_center_freq_from_abs_freq_point_a(uint32_t nof_prb, uint32_t freq_point_a_arfcn);

} // namespace band_helper

} // namespace srsgnb

#endif // SRSGNB_BAND_HELPER_H