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

#include "srsgnb/du/du_cell_config.h"

namespace srsgnb {

const unsigned NOF_SSB_SUBCARRIERS        = NOF_SSB_PRBS * NOF_SUBCARRIERS_PER_RB;
const unsigned KHZ_TO_HZ                  = 1000;
const double   HZ_TO_KHZ                  = 1e-3;
const unsigned MIN_ARFCN_3_GHZ_24_5_GHZ   = 600000;
const unsigned MIN_ARFCN_24_5_GHZ_100_GHZ = 2016667;
const double   N_SIZE_SYNC_RASTER_1_HZ    = 1200e3;
const unsigned N_UB_SYNC_RASTER_1         = 2500;
const unsigned N_UB_SYNC_RASTER_2         = 14757;
const double   M_SIZE_SYNC_RASTER_1_HZ    = 50e3;
const double   N_SIZE_SYNC_RASTER_2_HZ    = 1440e3;

/// This is expressed in Hz.
const double N_REF_OFFSET_3_GHZ_24_5_GHZ = 3e9;

namespace srs_du {

/// Contains the parameters that are returned by the DU config generator.
struct du_ssb_sib1_location {
  /// Tells whether the set of parameters represent a valid configuration.
  bool                  is_valid;
  ssb_offset_to_pointA  offset_to_point_A;
  ssb_subcarrier_offset k_ssb;
  unsigned              coreset0_idx;
  unsigned              searchspace0_idx;
};

/// Contains the parameters defining the SSB position within the band; returned by the DU config generator.
struct ssb_freq_location {
  /// Tells whether the set of parameters represent a valid configuration.
  bool                  is_valid;
  ssb_offset_to_pointA  offset_to_point_A;
  ssb_subcarrier_offset k_ssb;
};

/// Class that implements utilities that compute the SSB position within the band.
class du_config_generator
{
public:
  explicit du_config_generator(unsigned           dl_arfcn,
                               unsigned           nr_band,
                               unsigned           n_rbs_,
                               subcarrier_spacing scs_common,
                               subcarrier_spacing scs_ssb);

  /// Return the next viable position of the SSB on the raster, so that the SSB falls within the band and the SSB
  /// subcarriers are aligned with those of the CRBs
  ssb_freq_location get_next_ssb();
  unsigned          get_ssb_first_symbol() const { return ssb_first_symbol; };
  unsigned          get_max_ss0_idx() const { return max_ss0_idx; };

private:
  /// Finds the \f$M\f$ parameter of the sync raster so that the SSB subcarriers are aligned to those of the CRBs.
  unsigned find_M_raster();
  // Get the SSB central frequency, or SS_ref, given the parameters N, M, as per Table 5.4.3.1-1, TS 38.104.
  double get_ss_ref_hz(unsigned N, unsigned M);

  /// Parameters that are passed to the constructor.
  const unsigned dl_arfcn;
  const unsigned nr_band;
  /// Transmission bandwidth configuration N_RB, as per Table 5.3.2-1, TS 38.104.
  const unsigned           n_rbs;
  const subcarrier_spacing scs_common;
  const subcarrier_spacing scs_ssb;

  /// Parameters that are initialized by the constructor.
  double point_A_hz;
  /// RF reference frequency (a.k.a. centre frequency), as per TS 38.104, Section 5.4.2.1.
  double f_ref_hz;

  /// Band upper-bound, we consider the last subcarrier of the last CRB as band upper-bound.
  double           bw_ub_hz;
  ssb_pattern_case ssb_case;
  /// First symbol where the 1st SSB within the burst gets allocated, as per TS 38.21., Section 4.1.
  unsigned ssb_first_symbol;
  /// Maximum value of row index of Table 13-11, TS 38.213 that can be addressed for a specific configuration.
  uint8_t max_ss0_idx;

  /// This is the \f$N\f$ parameter in the synchronization raster, as per Table 5.4.3.1-1, TS 38.104.
  unsigned N_raster;
  /// This is the \f$M\f$ parameter in the synchronization raster, as per Table 5.4.3.1-1, TS 38.104.
  unsigned M_raster;
};

/// \brief Compute the position of the SSB within the band and the Coreset0/SS0 indices given some initial parameters.
/// \return The parameters defining the position of the SSB within the band and Coreset0/SS0 indices.
du_ssb_sib1_location get_ssb_sib1_freq_location(unsigned           dl_arfcn,
                                                unsigned           nr_band,
                                                unsigned           n_rbs,
                                                subcarrier_spacing scs_common,
                                                subcarrier_spacing scs_ssb);

} // namespace srs_du

} // namespace srsgnb
