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

#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/resource_block.h"
#include "srsgnb/ran/ssb_properties.h"

namespace srsgnb {

/// Number of subcarriers within a SSB block, as per TS 38.211, Section 7.4.3.1.
const unsigned NOF_SSB_SUBCARRIERS = NOF_SSB_PRBS * NOF_SUBCARRIERS_PER_RB;
/// Min ARFCN value for range of frequencies 3GHz-24.25GHz, as per Table 5.4.2.1-1, TS 38.104.
const unsigned MIN_ARFCN_3_GHZ_24_5_GHZ = 600000;
/// Min ARFCN value for range of frequencies above 24.25GHz, as per Table 5.4.2.1-1, TS 38.104.
const unsigned MIN_ARFCN_24_5_GHZ_100_GHZ = 2016667;
/// Offet \f$F_{REF-Offs}\f$ expressed in Hz, for 3GHz-24.25GHz freq. range, as per Table 5.4.2.1-1, TS 38.104.
const double N_REF_OFFSET_3_GHZ_24_5_GHZ = 3e9;
/// Freq. step in Hz, corresponding to \f$N\f$ for 0GHz-3GHz freq. range, as per Table 5.4.3.1-1, TS 38.104.
const double N_SIZE_SYNC_RASTER_1_HZ = 1200e3;
/// Freq. step in Hz, corresponding to \f$M\f$ for 0GHz-3GHz freq. range, as per Table 5.4.3.1-1, TS 38.104.
const double M_SIZE_SYNC_RASTER_1_HZ = 50e3;
/// Freq. step in Hz, corresponding to \f$M\f$ for 3GHz-24.25GHz freq. range, as per Table 5.4.3.1-1, TS 38.104.
const double N_SIZE_SYNC_RASTER_2_HZ = 1440e3;
/// Upper-bound for \f$N\f$ within 0GHz-3GHz freq. range, as per Table 5.4.3.1-1, TS 38.104.
const unsigned N_UB_SYNC_RASTER_1 = 2500;
/// Upper-bound for \f$N\f$ within 3GHz-24.25GHz freq. range, as per Table 5.4.3.1-1, TS 38.104.
const unsigned N_UB_SYNC_RASTER_2 = 14757;

/// Contains the parameters defining the SSB position within the band; returned by the DU config generator.
struct ssb_freq_location {
  /// Tells whether the set of parameters represent a valid configuration.
  bool is_valid;
  /// <em>offsetToPointA<\em>, as per Section 4.4.4.2, TS 38.211.
  ssb_offset_to_pointA offset_to_point_A;
  /// \f$k_{SSB}\f$, as per Section 7.4.3.1, TS 38.211.
  ssb_subcarrier_offset k_ssb;
  /// SSB central frequency, or SS_ref (as per TS 38.104, Section 5.4.3.1.), expressed in Hz.
  double ss_ref;
};

/// Class that implements utilities that compute the SSB position within the band.
class ssb_freq_position_generator
{
public:
  explicit ssb_freq_position_generator(unsigned           dl_arfcn,
                                       nr_band            nr_band,
                                       unsigned           n_rbs_,
                                       subcarrier_spacing scs_common,
                                       subcarrier_spacing scs_ssb);

  /// Return the next viable position of the SSB on the raster, so that the SSB falls within the band CRBs and the SSB
  /// subcarriers are aligned with those of the CRBs.
  ssb_freq_location get_next_ssb_location();
  unsigned          get_ssb_first_symbol() const { return ssb_first_symbol; };
  unsigned          get_max_ss0_idx() const { return max_ss0_idx; };

private:
  /// Finds the \f$M\f$ parameter of the sync raster so that the SSB subcarriers are aligned to those of the CRBs.
  unsigned find_M_raster();
  /// Get the SSB central frequency, or SS_ref, given the parameters N, M, as per Table 5.4.3.1-1, TS 38.104.
  double get_ss_ref_hz(unsigned N, unsigned M);

  /// Parameters that are passed to the constructor.
  const unsigned dl_arfcn;
  /// Transmission bandwidth configuration \f$N_{RB}\f$, as per Table 5.3.2-1, TS 38.104.
  const unsigned           n_rbs;
  const subcarrier_spacing scs_common;
  const subcarrier_spacing scs_ssb;

  /// Point A, as per TS 38.211, Section 4.4.4.2, expressed in Hz.
  double point_A_hz;
  /// RF reference frequency (a.k.a. centre frequency), as per TS 38.104, Section 5.4.2.1.
  double f_ref_hz;

  /// Band upper-bound expressed in Hz. We consider the last subcarrier of the last CRB as band upper-bound.
  double bw_ub_hz;
  /// SSB case, as per TS 38.213, Section 4.1.
  ssb_pattern_case ssb_case;
  /// First symbol where the 1st SSB within the burst gets allocated, as per TS 38.211, Section 4.1.
  unsigned ssb_first_symbol;
  /// Maximum value of row index of Table 13-11, TS 38.213 that can be addressed for a specific configuration.
  uint8_t max_ss0_idx;

  /// This is the \f$N\f$ parameter in the synchronization raster, as per Table 5.4.3.1-1, TS 38.104.
  unsigned N_raster;
  /// This is the \f$M\f$ parameter in the synchronization raster, as per Table 5.4.3.1-1, TS 38.104.
  unsigned M_raster;
};

} // namespace srsgnb
