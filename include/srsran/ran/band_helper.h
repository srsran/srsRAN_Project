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

/// This header is currently used only by the MAC to compute extra SSB parameters (needed for scheduling) from those
/// provided by DU.

#include "srsran/adt/expected.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/band_helper_constants.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/n_ta_offset.h"
#include "srsran/ran/nr_band.h"
#include "srsran/ran/ssb_properties.h"
#include <cstdint>

namespace srsran {

enum class duplex_mode;
enum class subcarrier_spacing;
enum class ssb_pattern_case;
enum class frequency_range;
enum class min_channel_bandwidth;

struct ssb_freq_location;

const std::array<nr_band, 60> all_nr_bands_fr1 = std::array<nr_band, 60>{
    {nr_band::n1,   nr_band::n2,   nr_band::n3,   nr_band::n5,  nr_band::n7,  nr_band::n8,  nr_band::n12, nr_band::n13,
     nr_band::n14,  nr_band::n18,  nr_band::n20,  nr_band::n24, nr_band::n25, nr_band::n26, nr_band::n28, nr_band::n29,
     nr_band::n30,  nr_band::n34,  nr_band::n38,  nr_band::n39, nr_band::n40, nr_band::n41, nr_band::n46, nr_band::n48,
     nr_band::n50,  nr_band::n51,  nr_band::n53,  nr_band::n65, nr_band::n66, nr_band::n67, nr_band::n70, nr_band::n71,
     nr_band::n74,  nr_band::n75,  nr_band::n76,  nr_band::n77, nr_band::n78, nr_band::n79, nr_band::n80, nr_band::n81,
     nr_band::n82,  nr_band::n83,  nr_band::n84,  nr_band::n85, nr_band::n86, nr_band::n89, nr_band::n90, nr_band::n91,
     nr_band::n92,  nr_band::n93,  nr_band::n94,  nr_band::n95, nr_band::n96, nr_band::n97, nr_band::n98, nr_band::n99,
     nr_band::n100, nr_band::n101, nr_band::n102, nr_band::n104}};

constexpr inline uint16_t nr_band_to_uint(nr_band band)
{
  return static_cast<uint16_t>(band);
}

constexpr inline nr_band uint_to_nr_band(unsigned band)
{
  return static_cast<nr_band>(band);
}

namespace band_helper {

/// \brief     Gets the NR band duplex mode.
/// \param[in] band Given band.
/// \return    A valid duplex_mode if the band is valid, duplex_mode::INVALID otherwise.
duplex_mode get_duplex_mode(nr_band band);

/// \brief     Gets the lowest band that includes a given Downlink ARFCN.
/// \remark    Some bands can be subset of others, e.g., band 2 is a subset of band 25.
/// \param[in] arfcn Given Downlink ARFCN.
/// \return    The band number if the ARFCN is bounded in a band, UINT16_MAX otherwise.
nr_band get_band_from_dl_arfcn(uint32_t arfcn);

/// \brief     Returns true if the band is used for shared spectrum channel access.
/// \remark    As per TS 38.104, Table 5.2-1, only bands where Note 3 or Note 4 apply.
bool is_band_for_shared_spectrum(nr_band band);

/// \brief     Returns true if the band should be considered as for 40MHz minimum channel BW.
/// \remark    As per TS 38.101, Table 5.2-1, only bands where Note 17 applies.
bool is_band_40mhz_min_ch_bw_equivalent(nr_band band);

/// \brief     Checks whether a Downlink ARFCN is valid for a given band.
/// \param[in] band Given NR band.
/// \param[in] arfcn Given Downlink ARFCN.
/// \param[in] scs is the subcarrier spacing of reference for \f$N_{RB}\f$, as per TS 38.104, Table 5.3.2-1. Only used
/// for bands n41, n77, n78, n79.
/// \param[in] bw Channel Bandwidth in MHz, which is required to validate some bands' ARFCN values.
/// \return    If the DL ARFCN is invalid for the band, a std::string value is returned with the reason.
error_type<std::string> is_dl_arfcn_valid_given_band(nr_band                  band,
                                                     uint32_t                 arfcn,
                                                     subcarrier_spacing       scs,
                                                     bs_channel_bandwidth_fr1 bw = bs_channel_bandwidth_fr1::MHz10);

/// @brief Get the respective UL ARFCN of a DL ARFCN.
///
/// For paired spectrum (FDD) the function returns the respective ARFCN in the same band.
/// For unparied spectrum (TDD) the function returns the same ARFCN.
///
/// \param[in] dl_arfcn The DL ARFCN.
/// \param[in] band     NR Band. If not given, the band is derived from the dl_arfcn.
/// \return uint32_t the UL ARFCN.
uint32_t get_ul_arfcn_from_dl_arfcn(uint32_t dl_arfcn, optional<nr_band> band);

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
/// \return    The SSB pattern case if band and subcarrier spacing match, \c invalid otherwise.
/// \remark    The functionr returns \c invalid if the band has SUL duplexing.
ssb_pattern_case get_ssb_pattern(nr_band band, subcarrier_spacing scs);

/// \brief Selects the most suitable SSB subcarrier spacing valid for this band.
///
/// The most suitable SSB subcarrier spacing will be the one matching with the common subcarrier spacing if possible,
/// otherwise the lowest SSB subcarrier spacing supported by the band.
///
/// \param[in] band       NR band number.
/// \param[in] scs_common Cell common subcarrier spacing.
/// \return The SSB subcarrier spacing.
subcarrier_spacing get_most_suitable_ssb_scs(nr_band band, subcarrier_spacing scs_common);

/// \brief     Returns boolean indicating whether the band is in paired spectrum.
/// \remark    Paired spectrum is FDD, unpaired spectrum is TDD, SUL, SDL.
/// \param[in] band Given band.
/// \return    true for paired spectrum, false otherwise.
bool is_paired_spectrum(nr_band band);

/// \brief     Returns boolean indicating whether the band belongs to FR1 or FR2.
/// \param[in] band Given band.
/// \remark    The input band must be a valid NR band; the function does not return an error in case of invalid band.
/// \return    the FR indicator, i.e., FR1 or FR2.
frequency_range get_freq_range(nr_band band);

/// @brief Compute the absolute pointA for a NR carrier from its bandwidth and the center frequency.
///
/// \remark The concept of <em>Center frequency<\em> is defined for LTE, while there is explicit mention to it for NR
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
/// \return \e absoluteFrequencyPointA as per TS38.211 Section 4.4.4.2, expressed in Hz.
double get_abs_freq_point_a_from_f_ref(double f_ref, uint32_t nof_rbs, subcarrier_spacing scs);

/// \brief Compute the RF reference frequency for a NR carrier from the nof RBs, SCS and the Absolute frequency pointA.
///
/// <em>Absolute frequency pointA<\em> is defined in TS 38.211, Section 4.4.4.2.
/// The RF reference frequency is defined in TS 38.104, Section 5.4.2.1.
///
/// \param[in] abs_freq_point_a \e absoluteFrequencyPointA as per TS38.211 Section 4.4.4.2, expressed in Hz.
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
unsigned get_n_rbs_from_bw(bs_channel_bandwidth_fr1 bw, subcarrier_spacing scs, frequency_range fr);

/// \brief Returns the minimum BS Channel Bandwidth for a given band and SCS from Table 5.3.5-1, TS 38.104, for FR1.
///
/// \param[in] bw is the <em>BS channel bandwidth<\em>, defined in TS 38.104, Section 5.3.
/// \param[in] scs is the subcarrier spacing of reference for \f$N_{RB}\f$, as per TS 38.104, Table 5.3.2-1.
/// \return The minimum BS channel BW for the given band and SCS, as per TS 38.104, Table 5.3.5-1.
min_channel_bandwidth get_min_channel_bw(nr_band nr_band, subcarrier_spacing scs);

/// Contains the parameters that are returned by the DU config generator.
struct ssb_coreset0_freq_location {
  /// <em>offsetToPointA<\em>, as per Section 4.4.4.2, TS 38.211.
  ssb_offset_to_pointA offset_to_point_A;
  /// \f$k_{SSB}\f$, as per Section 7.4.3.1, TS 38.211.
  ssb_subcarrier_offset k_ssb;
  /// NR-ARFCN corresponding to SSB central frequency, or \f$SS_{ref}\f$, as per TS 38.104, Section 5.4.3.1.
  unsigned ssb_arfcn;
  /// <em>controlResourceSetZero<\em>, as per Section 13, TS 38.213.
  unsigned coreset0_idx;
  /// <em>searchSpaceZero<\em>, as per Section 13, TS 38.213.
  unsigned searchspace0_idx;

  bool operator==(const ssb_coreset0_freq_location& other) const
  {
    return offset_to_point_A == other.offset_to_point_A and k_ssb == other.k_ssb and ssb_arfcn == other.ssb_arfcn and
           coreset0_idx == other.coreset0_idx and searchspace0_idx == other.searchspace0_idx;
  }
};

/// \brief Searches for the combination of CORESET#0 Index and SSB parameters that maximizes the number of CORESET#0 RBs
/// that do not intersect with the SSB.
///
/// \param[in] dl_arfcn is <em>DL-ARFCN<\em> corresponding to \f$F_{REF}\f$, as per TS 38.104, Section 5.4.2.1.
/// \param[in] nr_band is <em>NR operating band<\em>, as per TS 38.104, Table 5.2-1. Only FR1 values are supported.
/// \param[in] n_rbs is <em>Transmission bandwidth<\em> or \f$N_{RB}\f$ in number of RBs, as per TS 38.104, Table 5.2-1.
/// \param[in] scs_common is <em>subCarrierSpacingCommon<\em>, as per TS 38.331.
/// \param[in] scs_ssb is ssb subcarrier spacing.
/// \param[in] ss0_idx SearchSpace#0 index.
/// \return The parameters defining the position of the SSB within the band and Coreset0 and SS0 indices for
/// Table 13-[1-6] and Table 13-11, respectively, in TS 38.213.
optional<ssb_coreset0_freq_location> get_ssb_coreset0_freq_location(unsigned           dl_arfcn,
                                                                    nr_band            band,
                                                                    unsigned           n_rbs,
                                                                    subcarrier_spacing scs_common,
                                                                    subcarrier_spacing scs_ssb,
                                                                    uint8_t            ss0_idx);

/// \brief Fetches parameters defining the position of the SSB within the band for a given Coreset0 and SS0 indices.
///
/// \param[in] dl_arfcn is <em>DL-ARFCN<\em> corresponding to \f$F_{REF}\f$, as per TS 38.104, Section 5.4.2.1.
/// \param[in] nr_band is <em>NR operating band<\em>, as per TS 38.104, Table 5.2-1. Only FR1 values are supported.
/// \param[in] n_rbs is <em>Transmission bandwidth<\em> or \f$N_{RB}\f$ in number of RBs, as per TS 38.104, Table 5.2-1.
/// \param[in] scs_common is <em>subCarrierSpacingCommon<\em>, as per TS 38.331.
/// \param[in] scs_ssb is ssb subcarrier spacing.
/// \param[in] ss0_idx SearchSpace#0 index.
/// \param[in] cset0_idx CORESET#0 index.
/// \return The parameters defining the position of the SSB within the band for Table 13-[1-6] and Table 13-11,
/// respectively, in TS 38.213 if CORESET#0 RBs does not completely intersect with the SSB. Returns nullopt otherwise.
optional<ssb_coreset0_freq_location> get_ssb_coreset0_freq_location(unsigned           dl_arfcn,
                                                                    nr_band            band,
                                                                    unsigned           n_rbs,
                                                                    subcarrier_spacing scs_common,
                                                                    subcarrier_spacing scs_ssb,
                                                                    uint8_t            ss0_idx,
                                                                    unsigned           cset0_idx);

/// \brief Searches the CORESET#0 index that maximizes the number of CORESET#0 RBs that do not intersect with the SSB,
/// given an SSB configuration and the following restrictions:
/// - The CORESET#0 CRBs must fall in between pointA and the cell max CRB.
/// - The CORESET#0 number of symbols must equal or lower than the SSB first symbol offset.
/// - The number of symbols of CORESET#0 must match \c nof_coreset0_symb if defined.
///
/// \param[in] band is <em>NR operating band<\em>, as per TS 38.104, Table 5.2-1. Only FR1 values are supported.
/// \param[in] n_rbs is <em>Transmission bandwidth<\em> or \f$N_{RB}\f$ in number of RBs, as per TS 38.104, Table 5.2-1.
/// \param[in] scs_common is <em>subCarrierSpacingCommon<\em>, as per TS 38.331.
/// \param[in] scs_ssb is ssb subcarrier spacing.
/// \param[in] offset_to_point_A SSB offset to point A.
/// \param[in] k_ssb k_SSB value.
/// \param[in] ssb_first_symbol SSB first symbol.
/// \param[in] ss0_idx SearchSpace#0 index.
/// \param[in] nof_coreset0_symb Number of symbols required for returned CORESET#0, if defined.
/// \return CORESET#0 index if a valid candidate was found. Empty optional otherwise.
optional<unsigned> get_coreset0_index(nr_band               band,
                                      unsigned              n_rbs,
                                      subcarrier_spacing    scs_common,
                                      subcarrier_spacing    scs_ssb,
                                      ssb_offset_to_pointA  offset_to_point_A,
                                      ssb_subcarrier_offset k_ssb,
                                      uint8_t               ssb_first_symbol,
                                      uint8_t               ss0_idx,
                                      optional<unsigned>    nof_coreset0_symb = {});

/// \brief Compute the number of CORESET#0 CRBs that do not intersect with the SSB CRBs.
unsigned get_nof_coreset0_rbs_not_intersecting_ssb(unsigned              cset0_idx,
                                                   nr_band               band,
                                                   subcarrier_spacing    scs_common,
                                                   subcarrier_spacing    scs_ssb,
                                                   ssb_offset_to_pointA  offset_to_point_A,
                                                   ssb_subcarrier_offset k_ssb);

/// \brief Returns the n_ta_offset for a given band.
/// \param[in] band is <em>NR operating band<\em>, as per TS 38.104, Table 5.2-1. Only FR1 values are supported.
/// \return Value of n_ta_offset.
n_ta_offset get_ta_offset(nr_band band);

/// \brief Returns SSB ARFCN for a given cell configuration.
/// \param[in] dl_arfcn is <em>DL-ARFCN<\em> corresponding to \f$F_{REF}\f$, as per TS 38.104, Section 5.4.2.1.
/// \param[in] nr_band is <em>NR operating band<\em>, as per TS 38.104, Table 5.2-1. Only FR1 values are supported.
/// \param[in] n_rbs is <em>Transmission bandwidth<\em> or \f$N_{RB}\f$ in number of RBs, as per TS 38.104, Table 5.2-1.
/// \param[in] scs_common is <em>subCarrierSpacingCommon<\em>, as per TS 38.331.
/// \param[in] scs_ssb is ssb subcarrier spacing.
/// \param[in] offset_to_point_A SSB Offset to PointA.
/// \param[in] k_ssb Subcarrier offset of SSB.
/// \return SSB ARFCN if cell configuration is valid, nullopt otherwise.
optional<unsigned> get_ssb_arfcn(unsigned              dl_arfcn,
                                 nr_band               band,
                                 unsigned              n_rbs,
                                 subcarrier_spacing    scs_common,
                                 subcarrier_spacing    scs_ssb,
                                 ssb_offset_to_pointA  offset_to_point_A,
                                 ssb_subcarrier_offset k_ssb);

/// \brief Validate the SSB ARFCN for a given band.
///
/// \remark The validity of the GSCN raster is based on the GSCN value, as per Section 5.4.3.1, TS 38.104. The ARFCN is
/// considered valid if the corresponding \f$SS_{ref}\f$ maps to a valid GSCN value.
/// \param[in] ssb_arfcn ARFCN value of the SSB.
/// \param[in] band NR band.
/// \param[in] ssb_scs SSB subcarrier spacing.
/// \param[in] bw Channel Bandwidth in MHz, which is required to validate some bands' ARFCN values.
/// \return If the ARFCN (GSCN) is invalid for the band, a std::string value is returned with the reason.
error_type<std::string> is_ssb_arfcn_valid_given_band(uint32_t                 ssb_arfcn,
                                                      nr_band                  band,
                                                      subcarrier_spacing       scs,
                                                      bs_channel_bandwidth_fr1 bw = bs_channel_bandwidth_fr1::invalid);

} // namespace band_helper

} // namespace srsran
