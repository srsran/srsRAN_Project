/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_test_suite.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

static void test_ssb_belong_to_sync_raster(double ss_ssb_hz)
{
  if (band_helper::freq_to_nr_arfcn(ss_ssb_hz) < MIN_ARFCN_3_GHZ_24_5_GHZ) {
    unsigned ss_ssb_khz = static_cast<unsigned>(ss_ssb_hz * HZ_TO_KHZ);
    for (unsigned M : {1, 3, 5}) {
      unsigned f_ssb_N_raster = ss_ssb_khz - M * static_cast<unsigned>(M_SIZE_SYNC_RASTER_1_HZ * HZ_TO_KHZ);
      unsigned rem_N_raster   = f_ssb_N_raster % static_cast<unsigned>(N_SIZE_SYNC_RASTER_1_HZ * HZ_TO_KHZ);
      if (rem_N_raster == 0) {
        return;
      }
      if (M == 5) {
        ASSERT_EQ(0, rem_N_raster);
      }
    }
  } else if (band_helper::freq_to_nr_arfcn(ss_ssb_hz) < MIN_ARFCN_24_5_GHZ_100_GHZ) {
    unsigned ss_ssb_khz   = static_cast<unsigned>((ss_ssb_hz - N_REF_OFFSET_3_GHZ_24_5_GHZ) * HZ_TO_KHZ);
    unsigned rem_N_raster = ss_ssb_khz % static_cast<unsigned>(N_SIZE_SYNC_RASTER_2_HZ * HZ_TO_KHZ);
    ASSERT_EQ(0, rem_N_raster);
  } else {
    // The ARFCN range above 2016667 (or 24.5GHz) is not supported.
    ASSERT_TRUE(false);
  }
}

void srsgnb::test_ssb_coreset0_allocation(unsigned                            dl_arfcn,
                                          unsigned                            nr_band,
                                          unsigned                            n_rbs,
                                          subcarrier_spacing                  scs_common,
                                          subcarrier_spacing                  scs_ssb,
                                          const srs_du::du_ssb_sib1_location& params)
{
  // Position of SSB central carrier, located at the 120th SSB's subcarrier.
  double f_ref_hz   = band_helper::nr_arfcn_to_freq(dl_arfcn);
  double point_A_hz = band_helper::get_abs_freq_point_a_from_f_ref(f_ref_hz, n_rbs, scs_common);
  double bw_up_hz   = point_A_hz + static_cast<double>(n_rbs * NOF_SUBCARRIERS_PER_RB *
                                                     scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ);

  unsigned crb_ssb    = scs_common == subcarrier_spacing::kHz15 ? params.offset_to_point_A.to_uint()
                                                                : params.offset_to_point_A.to_uint() / 2;
  double   crb_ssb_hz = params.offset_to_point_A.to_uint() *
                      static_cast<double>(scs_to_khz(subcarrier_spacing::kHz15) * NOF_SUBCARRIERS_PER_RB * KHZ_TO_HZ);
  double k_ssb_hz    = params.k_ssb.to_uint() * static_cast<double>(scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ);
  double f_ssb_0_hz  = point_A_hz + crb_ssb_hz + k_ssb_hz;
  double ss_ssb_hz   = f_ssb_0_hz + static_cast<double>(scs_to_khz(scs_ssb) * NOF_SSB_SUBCARRIERS * KHZ_TO_HZ / 2);
  double f_ssb_ub_hz = f_ssb_0_hz + static_cast<double>(scs_to_khz(scs_ssb) * NOF_SSB_SUBCARRIERS * KHZ_TO_HZ);

  // Verify that the first SSB's subcarrier is not below pointA.
  ASSERT_TRUE(point_A_hz <= f_ssb_0_hz);
  // Verify that the last SSB's subcarrier is not above the BW upper-bound.
  ASSERT_TRUE(f_ssb_ub_hz <= bw_up_hz);
  // Verify that the SSB's reference frequency, or SSB, belongs to the sync-raster, as per Table 5.4.3.1-1, TS 38.104.
  test_ssb_belong_to_sync_raster(ss_ssb_hz);

  auto coreset0_cfg = pdcch_type0_css_coreset_get(band_helper::get_min_channel_bw(nr_band, scs_common),
                                                  scs_ssb,
                                                  scs_common,
                                                  params.coreset0_idx,
                                                  params.k_ssb.to_uint());
  // Verify that the CORESET0 doesn't start below CRB0.
  ASSERT_GE(crb_ssb, static_cast<unsigned>(coreset0_cfg.offset));

  // Verify that the CORESET0 doesn't exceed the given bandwidth.
  ASSERT_GE(n_rbs,
            crb_ssb - static_cast<unsigned>(coreset0_cfg.offset) + static_cast<unsigned>(coreset0_cfg.nof_rb_coreset));
}