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

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/ssb_properties.h"
#include <stdint.h>

namespace srsgnb {

enum class duplex_mode;
enum class subcarrier_spacing;
enum class ssb_pattern_case;
enum class bs_channel_bandwidth_fr1;
enum class frequency_range;
enum class min_channel_bandwidth;

struct ssb_freq_location;

const unsigned KHZ_TO_HZ = 1000U;
const double   HZ_TO_KHZ = 1e-3;

/// \brief NR operating bands in FR1 and FR2.
///
/// This enumeration abstracts the NR operating bands for FR1 and FR2 described in TS 38.104, Table 5.2-1 and
/// Table 5.2-2, respectively.
enum class nr_band {
  invalid = 0,
  // FR1 bands.
  n1  = 1,
  n2  = 2,
  n3  = 3,
  n5  = 5,
  n7  = 7,
  n8  = 8,
  n12 = 12,
  n20 = 20,
  n25 = 25,
  n28 = 28,
  n34 = 34,
  n38 = 38,
  n39 = 39,
  n40 = 40,
  n41 = 41,
  n50 = 50,
  n51 = 51,
  n66 = 66,
  n70 = 70,
  n71 = 71,
  n74 = 74,
  n75 = 75,
  n76 = 76,
  n77 = 77,
  n78 = 78,
  n79 = 79,
  n80 = 80,
  n81 = 81,
  n82 = 82,
  n83 = 83,
  n84 = 84,
  n86 = 86,
  // FR2 bands.
  n257 = 257,
  n258 = 258,
  n260 = 260,
  n261 = 261
};

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
ssb_pattern_case get_ssb_pattern(nr_band band, subcarrier_spacing scs);

/// \brief Selects the lowest SSB subcarrier spacing valid for this band.
/// \param[in] band NR band number.
/// \return The SSB subcarrier spacing.
subcarrier_spacing get_lowest_ssb_scs(nr_band band);

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

/// \brief Compute the position of the SSB within the band and the Coreset0/SS0 indices given some initial parameters.
///
/// It returns the first valid combination of parameters such that the SSB and CORESET0 get allocated within the band
/// without overlapping over the same resources.
///
/// \param[in] dl_arfcn is <em>DL-ARFCN<\em> corresponding to \f$F_{REF}\f$, as per TS 38.104, Section 5.4.2.1.
/// \param[in] nr_band is <em>NR operating band<\em>, as per TS 38.104, Table 5.2-1. Only FR1 values are supported.
/// \param[in] n_rbs is <em>Transmission bandwidth<\em> or \f$N_{RB}\f$ in number of RBs, as per TS 38.104, Table 5.2-1.
/// \param[in] scs_common is <em>subCarrierSpacingCommon<\em>, as per TS 38.331.
/// \param[in] scs_ssb is ssb subcarrier spacing.
/// \return The parameters defining the position of the SSB within the band and Coreset0 and SS0 indices for
/// Table 13-[1-6] and Table 13-11, respectively, in TS 38.213.
optional<ssb_coreset0_freq_location> get_ssb_coreset0_freq_location(unsigned           dl_arfcn,
                                                                    nr_band            band,
                                                                    unsigned           n_rbs,
                                                                    subcarrier_spacing scs_common,
                                                                    subcarrier_spacing scs_ssb);

/// \brief Compute a valid CORESET#0 index with maximum number of CRBs, given the following
/// restrictions:
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

} // namespace band_helper

} // namespace srsgnb
