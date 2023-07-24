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

#include "ssb_coreset0_freq_pos_checker.h"
#include "lib/ran/ssb_freq_position_generator.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include <gtest/gtest.h>

using namespace srsran;

// Verify that the frequency ss_ssb_hz, or frequency corresponding to the 120th subcarrier of the SSB, is compatible
// with the synchronization raster defined in Table 5.4.3.1-1, TS 38.104.
static void test_ssb_belong_to_sync_raster(double ss_ssb_hz)
{
  if (band_helper::freq_to_nr_arfcn(ss_ssb_hz) < band_helper::MIN_ARFCN_3_GHZ_24_5_GHZ) {
    unsigned ss_ssb_khz = static_cast<unsigned>(ss_ssb_hz * band_helper::HZ_TO_KHZ);
    for (unsigned M : {1, 3, 5}) {
      unsigned f_ssb_N_raster =
          ss_ssb_khz - M * static_cast<unsigned>(band_helper::M_SIZE_SYNC_RASTER_1_HZ * band_helper::HZ_TO_KHZ);
      unsigned rem_N_raster =
          f_ssb_N_raster % static_cast<unsigned>(band_helper::N_SIZE_SYNC_RASTER_1_HZ * band_helper::HZ_TO_KHZ);
      if (rem_N_raster == 0) {
        return;
      }
      if (M == 5) {
        ASSERT_EQ(0, rem_N_raster);
      }
    }
  } else if (band_helper::freq_to_nr_arfcn(ss_ssb_hz) < band_helper::MIN_ARFCN_24_5_GHZ_100_GHZ) {
    unsigned ss_ssb_khz =
        static_cast<unsigned>((ss_ssb_hz - band_helper::N_REF_OFFSET_3_GHZ_24_5_GHZ) * band_helper::HZ_TO_KHZ);
    unsigned rem_N_raster =
        ss_ssb_khz % static_cast<unsigned>(band_helper::N_SIZE_SYNC_RASTER_2_HZ * band_helper::HZ_TO_KHZ);
    ASSERT_EQ(0, rem_N_raster);
  } else {
    // The ARFCN range above 2016667 (or 24.5GHz) is not supported.
    ASSERT_TRUE(false);
  }
}

void srsran::test_ssb_coreset0_allocation(unsigned                                       dl_arfcn,
                                          nr_band                                        nr_band,
                                          unsigned                                       n_rbs,
                                          subcarrier_spacing                             scs_common,
                                          subcarrier_spacing                             scs_ssb,
                                          const band_helper::ssb_coreset0_freq_location& params)
{
  // Position of SSB central carrier, located at the 120th SSB's subcarrier.
  double f_ref_hz   = band_helper::nr_arfcn_to_freq(dl_arfcn);
  double point_A_hz = band_helper::get_abs_freq_point_a_from_f_ref(f_ref_hz, n_rbs, scs_common);
  double bw_up_hz   = point_A_hz + static_cast<double>(n_rbs * NOF_SUBCARRIERS_PER_RB * scs_to_khz(scs_common) *
                                                     band_helper::KHZ_TO_HZ);

  unsigned crb_ssb = scs_common == subcarrier_spacing::kHz15 ? params.offset_to_point_A.to_uint()
                                                             : params.offset_to_point_A.to_uint() / 2;
  double   crb_ssb_hz =
      params.offset_to_point_A.to_uint() *
      static_cast<double>(scs_to_khz(subcarrier_spacing::kHz15) * NOF_SUBCARRIERS_PER_RB * band_helper::KHZ_TO_HZ);
  double k_ssb_hz =
      params.k_ssb.to_uint() * static_cast<double>(scs_to_khz(subcarrier_spacing::kHz15) * band_helper::KHZ_TO_HZ);
  double f_ssb_0_hz = point_A_hz + crb_ssb_hz + k_ssb_hz;
  double ss_ssb_hz =
      f_ssb_0_hz + static_cast<double>(scs_to_khz(scs_ssb) * NOF_SSB_SUBCARRIERS * band_helper::KHZ_TO_HZ / 2);
  double f_ssb_ub_hz =
      f_ssb_0_hz + static_cast<double>(scs_to_khz(scs_ssb) * NOF_SSB_SUBCARRIERS * band_helper::KHZ_TO_HZ);

  // Verify that the first SSB's subcarrier is not below pointA.
  ASSERT_TRUE(point_A_hz <= f_ssb_0_hz);
  // Verify that the last SSB's subcarrier is not above the BW upper-bound.
  ASSERT_TRUE(f_ssb_ub_hz <= bw_up_hz);
  // Verify that the SSB's reference frequency, or SSB, belongs to the sync-raster, as per Table 5.4.3.1-1, TS 38.104.
  test_ssb_belong_to_sync_raster(ss_ssb_hz);

  auto coreset0_cfg =
      pdcch_type0_css_coreset_get(nr_band, scs_ssb, scs_common, params.coreset0_idx, params.k_ssb.to_uint());
  // Verify that the CORESET0 doesn't start below CRB0.
  ASSERT_GE(crb_ssb, static_cast<unsigned>(coreset0_cfg.offset));

  // Verify that the CORESET0 doesn't exceed the given bandwidth.
  ASSERT_GE(n_rbs,
            crb_ssb - static_cast<unsigned>(coreset0_cfg.offset) + static_cast<unsigned>(coreset0_cfg.nof_rb_coreset));
}