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

/// This header is currently used only by the MAC to compute extra SSB parameters (needed for scheduling) from those
/// provided by DU.

#include <stdint.h>

namespace srsgnb {

enum class duplex_mode;
enum class subcarrier_spacing;
enum class ssb_pattern_case;
enum class bs_channel_bw_fr1;
enum class frequency_range;

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
/// \param[in] nr_arfcn Given NR-ARFCN.
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

/// @brief Compute the absolute pointA for a NR carrier from its bandwidth and the center frequency.
///
/// \remark The concept of <em>Center frequency<\em> is defined for LTE, while there is explicit mention to it for NR
/// staring from Rel. 15 and onward.
///
/// @param nof_prb Carrier bandwidth in number of RB.
/// @param center_freq double Frequency in Hz.
/// @return Absolute Point A frequency in Hz.
double get_abs_freq_point_a_from_center_freq(uint32_t nof_prb, double center_freq);

/// \brief Compute the absolute frequency point A for a arfcn
///
/// \param[in] band nr frequency band.
/// \param[in] nof_prb Number of PRBs.
/// \param[in] arfcn Given ARFCN.
/// \return frequency point A in arfcn notation.
uint32_t get_abs_freq_point_a_arfcn(uint32_t nof_prb, uint32_t arfcn);

/// \brief Compute the center frequency for a NR carrier from its bandwidth and the absolute pointA.
/// The center frequency should point to the RB with index = ceil(Nrb / 2), where Nrb is the number of RBs of the cell.
///
/// \param nof_prb Carrier bandwidth in number of RB.
/// \param freq_point_a_arfcn Absolute Point A frequency ARFCN.
/// \return double Frequency in Hz.
double get_center_freq_from_abs_freq_point_a(uint32_t nof_prb, uint32_t freq_point_a_arfcn);

/// \brief Compute the Absolute frequency pointA for a NR carrier from the nof RBs, SCS and the RF reference frequency.
///
/// The RF reference frequency is defined in TS 38.104, Section 5.4.2.1.
/// <em>Point A<\em> is defined in TS 38.211, Section 4.4.4.2.
///
/// \param[in] f_ref RF reference frequency, in Hz, as per TS 38.104, Section 5.4.2.1.
/// \param[in] nof_prb corresponds to Transmission bandwidth configuration \f$N_{RB}\f$, as per TS 38.104,
/// Table 5.3.2-1. Only values in Table 5.3.2-1 should be provided.
/// \param[in] scs is the subcarrier spacing of reference for \f$N_{RB}\f$, as per TS 38.104, Table 5.3.2-1.
/// \return absoluteFrequencyPointA as per TS 38.211, Section 4.4.4.2, but measured in Hz instead of being expressed as
/// in ARFCN.
double get_abs_freq_point_a_from_f_ref(double f_ref, uint32_t nof_rbs, subcarrier_spacing scs);

/// \brief Compute the RF reference frequency for a NR carrier from the nof RBs, SCS and the Absolute frequency pointA.
///
/// <em>Absolute frequency pointA<\em> is defined in TS 38.211, Section 4.4.4.2.
/// The RF reference frequency is defined in TS 38.104, Section 5.4.2.1.
///
/// \param[in] abs_freq_point_a absoluteFrequencyPointA as per TS 38.211, Section 4.4.4.2, but measured in Hz instead of
/// being expressed as in ARFCN.
/// \param[in] nof_prb corresponds to Transmission bandwidth configuration \f$N_{RB}\f$, as per TS 38.104,
/// Table 5.3.2-1. Only values in Table 5.3.2-1 should be provided.
/// \param[in] scs is the subcarrier spacing of reference for \f$N_{RB}\f$, as per TS 38.104, Table 5.3.2-1.
/// \return RF reference frequency, in Hz, as per TS 38.104, Section 5.4.2.1.
double get_f_ref_from_abs_freq_point_a(double abs_freq_point_a, uint32_t nof_rbs, subcarrier_spacing scs);

/// \brief Returns the values \f$N_{RB}\f$ from Table 5.3.2-1, TS 38.104.
///
/// \param[in] bw is the <em>BS channel bandwidth<\em>, defined in TS 38.104, Section 5.3.
/// \param[in] scs is the subcarrier spacing of reference for \f$N_{RB}\f$, as per TS 38.104, Table 5.3.2-1.
/// \param[in] fr is frequency range FR1 or FR2.
/// \return \f$N_{RB}\f$, as per TS 38.104, Table 5.3.2-1.
unsigned get_n_rbs_from_bw(bs_channel_bw_fr1 bw, subcarrier_spacing scs, frequency_range fr);

} // namespace band_helper

} // namespace srsgnb
