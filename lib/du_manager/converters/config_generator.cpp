/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "config_generator.h"
#include "../../scheduler/support/pdcch/pdcch_type0_css_occasions.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include <math.h>

using namespace srsgnb;

using namespace srs_du;

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

// This expressed in Hz.
const double N_REF_OFFSET_3_GHZ_24_5_GHZ = 3e9;

// ------------------ Static functions --------------------.

static double get_f_ssb_0_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref - static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);
}

static double get_f_ssb_ub_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref + static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);
}

static ssb_offset_to_pointA
compute_offset_to_pointA(double f_ssb_0_hz, double point_A_hz, subcarrier_spacing scs_common)
{
  ssb_offset_to_pointA offset_to_pA{0};
  if (scs_common == subcarrier_spacing::kHz15) {
    offset_to_pA = static_cast<unsigned>((f_ssb_0_hz - point_A_hz) * HZ_TO_KHZ) /
                   (NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15));
  } else if (scs_common == subcarrier_spacing::kHz30) {
    offset_to_pA = 2 * static_cast<unsigned>((f_ssb_0_hz - point_A_hz) * HZ_TO_KHZ) /
                   (2 * NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15));
  } else {
    srsgnb_assert(scs_common <= subcarrier_spacing::kHz30, "FR2 not supported");
  }
  return offset_to_pA;
}

static ssb_subcarrier_offset compute_k_ssb(double f_ssb_0_hz, double point_A_hz, ssb_offset_to_pointA offset_to_pA)
{
  ssb_subcarrier_offset k_ssb;
  unsigned              f_crb_ssb_khz =
      point_A_hz * HZ_TO_KHZ +
      static_cast<double>(NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15) * offset_to_pA.to_uint());
  k_ssb = static_cast<unsigned>(f_ssb_0_hz * HZ_TO_KHZ - f_crb_ssb_khz) / scs_to_khz(subcarrier_spacing::kHz15);

  return k_ssb;
}

static unsigned
get_max_coreset0_index(subcarrier_spacing scs_common, subcarrier_spacing scs_ssb, min_channel_bandwidth min_channel_bw)
{
  if (min_channel_bw == min_channel_bandwidth::MHz5 || min_channel_bw == min_channel_bandwidth::MHz10) {
    if (scs_ssb == subcarrier_spacing::kHz15 and scs_common == subcarrier_spacing::kHz15) {
      return 14;
    } else if (scs_ssb == subcarrier_spacing::kHz15 and scs_common == subcarrier_spacing::kHz30) {
      return 13;
    } else if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz15) {
      return 8;
    } else if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz30) {
      return 15;
    }
  } else if (min_channel_bw == min_channel_bandwidth::MHz40) {
    if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz15) {
      return 8;
    } else if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz30) {
      return 9;
    }
  }
  return 0;
}

// --------------- du_config_generator class functions ----------------.

du_config_generator::du_config_generator(unsigned           dl_arfcn_,
                                         unsigned           nr_band_,
                                         unsigned           n_rbs_,
                                         subcarrier_spacing scs_common_,
                                         subcarrier_spacing scs_ssb_) :
  dl_arfcn{dl_arfcn_},
  nr_band{nr_band_},
  n_rbs(n_rbs_),
  scs_common{scs_common_},
  scs_ssb{scs_ssb_},
  ssb_case{band_helper::get_ssb_pattern(nr_band_, scs_ssb)}
{
  f_ref_hz   = band_helper::nr_arfcn_to_freq(dl_arfcn);
  point_A_hz = band_helper::get_abs_freq_point_a_from_f_ref(dl_arfcn, n_rbs, scs_common);
  bw_ub_hz   = point_A_hz + n_rbs * NOF_SUBCARRIERS_PER_RB * scs_to_khz(scs_common) * KHZ_TO_HZ;

  double f_ssb_min_hz = point_A_hz + static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);

  if (dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ) {
    N_raster = static_cast<unsigned>(std::floor(f_ssb_min_hz / N_SIZE_SYNC_RASTER_1_HZ));
  } else if (dl_arfcn >= MIN_ARFCN_3_GHZ_24_5_GHZ && dl_arfcn < MIN_ARFCN_24_5_GHZ_100_GHZ) {
    N_raster =
        static_cast<unsigned>(std::floor((f_ssb_min_hz - N_REF_OFFSET_3_GHZ_24_5_GHZ) / N_SIZE_SYNC_RASTER_2_HZ));
  } else {
    srsgnb_assert(dl_arfcn < MIN_ARFCN_24_5_GHZ_100_GHZ, "FR2 frequencies not supported");
  }

  ssb_first_symbol = ssb_case == ssb_pattern_case::B ? 4 : 2;

  for (unsigned ss0_idx = 0; ss0_idx < 10; ++ss0_idx) {
    searchspace0_idx_list.push_back(ss0_idx);
  }
}

double du_config_generator::get_ss_ref_hz(unsigned N, unsigned M)
{
  double ss_ref =
      dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ
          ? static_cast<double>(N) * N_SIZE_SYNC_RASTER_1_HZ + static_cast<double>(M) * M_SIZE_SYNC_RASTER_1_HZ
          : N_REF_OFFSET_3_GHZ_24_5_GHZ + static_cast<double>(N) * N_SIZE_SYNC_RASTER_2_HZ;
  return ss_ref;
}

unsigned du_config_generator::find_M_raster()
{
  srsgnb_assert(dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ, "This function should be called only for DL-ARFCN < 600000");

  // If M_raster > 0, no need  to find the same value again.
  if (M_raster > 0) {
    return M_raster;
  }

  for (unsigned M : {1, 3, 5}) {
    double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M);
    double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, scs_ssb);
    double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, scs_ssb);

    bool is_ssb_inside_band = (f_ssb_0_hz >= point_A_hz) && (f_ssb_ub_hz <= bw_ub_hz);

    if (is_ssb_inside_band) {
      bool is_scs_30khz_spacing = scs_common == subcarrier_spacing::kHz30 && scs_ssb == subcarrier_spacing::kHz30;
      bool is_multiple_of_scs   = is_scs_30khz_spacing ? fmod(static_cast<unsigned>(f_ssb_0_hz - point_A_hz),
                                                            scs_to_khz(subcarrier_spacing::kHz30) * KHZ_TO_HZ) == 0.0
                                                       : fmod(static_cast<unsigned>(f_ssb_0_hz - point_A_hz),
                                                            scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ) == 0.0;

      if (is_multiple_of_scs) {
        M_raster = M;
        return M_raster;
      }
    }

    // As per TS 38.104, Section 5.4.3.1, increment M by 2 to compute next sync raster.
  }
  return 0;
}

ssb_freq_location du_config_generator::get_next_ssb()
{
  ssb_freq_location ssb{.is_valid = false};

  static const unsigned N_raster_max = dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ ? N_UB_SYNC_RASTER_1 : N_UB_SYNC_RASTER_2;

  while (N_raster < N_raster_max) {
    // If M_raster > 0, no need  to find the same value again.
    if (M_raster == 0) {
      M_raster = find_M_raster();
    }

    double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M_raster);
    double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, scs_ssb);
    double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, scs_ssb);

    bool is_ssb_inside_band = (f_ssb_0_hz >= point_A_hz) && (f_ssb_ub_hz <= bw_ub_hz);

    if (is_ssb_inside_band) {
      bool is_scs_30khz_spacing = scs_common == subcarrier_spacing::kHz30 && scs_ssb == subcarrier_spacing::kHz30;
      bool is_multiple_of_scs =
          is_scs_30khz_spacing
              ? fmod(f_ssb_0_hz - point_A_hz, static_cast<double>(scs_to_khz(subcarrier_spacing::kHz30) * KHZ_TO_HZ)) ==
                    0.0
              : fmod(f_ssb_0_hz - point_A_hz, scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ) == 0.0;

      if (is_multiple_of_scs) {
        ssb.offset_to_point_A = compute_offset_to_pointA(f_ssb_0_hz, point_A_hz, scs_common);
        ssb.k_ssb             = compute_k_ssb(f_ssb_0_hz, point_A_hz, ssb.offset_to_point_A);
        ssb.is_valid          = true;
        N_raster++;
        return ssb;
      }
    }

    // Increment N_raster.
    N_raster++;
  }

  return ssb;
}

// -------------- Only function that should be called by the user ---------------.

du_ssb_sib1_location srsgnb::srs_du::get_ssb_sib1_freq_location(unsigned           dl_arfcn,
                                                                unsigned           nr_band,
                                                                unsigned           n_rbs,
                                                                subcarrier_spacing scs_common,
                                                                subcarrier_spacing scs_ssb)
{
  // Get f_ref, point_A from dl_arfcn.
  du_config_generator du_cfg{dl_arfcn, nr_band, n_rbs, scs_common, scs_ssb};

  // Initialized the results
  du_ssb_sib1_location results{.is_valid = false};

  // Select 1st SSB. Select first searchspace0_idx and find viable coreset0
  ssb_freq_location ssb = du_cfg.get_next_ssb();
  while (not ssb.is_valid) {
    unsigned crbs_ssb =
        scs_common == subcarrier_spacing::kHz15 ? ssb.offset_to_point_A.to_uint() : ssb.offset_to_point_A.to_uint() / 2;
    // Iterate over

    // Iterate over the searchSpace0_indices and corresponding configurations.
    for (uint8_t ss0_idx = 0; ss0_idx <= du_cfg.get_max_ss0_idx(); ++ss0_idx) {
      pdcch_type0_css_occasion_pattern1_description ss0_config =
          pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
              .is_fr2 = false, .ss_zero_index = ss0_idx, .nof_symb_coreset = 1});

      unsigned max_cset0_idx =
          get_max_coreset0_index(scs_common, scs_ssb, band_helper::get_min_channel_bw(nr_band, scs_common));

      // Iterate over the coreset0_indices and corresponding configurations.
      for (unsigned cset0_idx = 0; cset0_idx < max_cset0_idx; ++cset0_idx) {
        auto coreset0_cfg = pdcch_type0_css_coreset_get(
            band_helper::get_min_channel_bw(nr_band, scs_common), scs_ssb, scs_common, cset0_idx, ssb.k_ssb.to_uint());

        bool coreset0_not_below_pointA = crbs_ssb >= static_cast<unsigned>(coreset0_cfg.offset);
        bool coreset0_not_above_bw_ub =
            crbs_ssb - static_cast<unsigned>(coreset0_cfg.offset) + coreset0_cfg.nof_rb_coreset <= n_rbs;
        bool ss0_not_overlapping_ssb_symbol =
            ss0_config.offset == 0 ? coreset0_cfg.nof_symb_coreset <= du_cfg.get_ssb_first_symbol() : true;

        // If the SSB and CORESET 0 are witin the
        if (coreset0_not_below_pointA && coreset0_not_above_bw_ub && ss0_not_overlapping_ssb_symbol) {
          results.offset_to_point_A = ssb.offset_to_point_A;
          results.k_ssb             = ssb.k_ssb;
          results.coreset0_idx      = cset0_idx;
          results.searchspace0_idx  = ss0_idx;
          results.is_valid          = true;
          return results;
        }
      }
    }

    ssb = du_cfg.get_next_ssb();
  }

  return results;
}