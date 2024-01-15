/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/band_helper.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/ssb_properties.h"

namespace srsran {

/// Number of subcarriers within a SSB block, as per TS 38.211, Section 7.4.3.1.
const unsigned NOF_SSB_SUBCARRIERS = NOF_SSB_PRBS * NOF_SUBCARRIERS_PER_RB;

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
                                       nr_band            nr_band_,
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
  double get_ss_ref_hz(unsigned N, unsigned M) const;
  /// Increase N_raster according to the raster step given by the band, as per Table 5.4.3.3-1, TS 38.104, ver 17.8.0.
  void increase_N_raster();
  /// Implements \ref get_next_ssb_location() for the specific case of bands n46, n96 and n102, which don't have a
  /// standard raster.
  ssb_freq_location get_next_ssb_location_special_raster();

  /// Parameters that are passed to the constructor.
  const unsigned dl_arfcn;
  const nr_band  band;
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
  /// Some bands have a non-regular set of GSCN values for the synchronization raster. This index keeps track of what
  /// GSCN value has been used to generate the latest SSB frequency location.
  size_t gscn_raster_idx;
};

} // namespace srsran
