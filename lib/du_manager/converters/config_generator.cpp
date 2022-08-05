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

#define NUM_SUBCARRIERS_PER_RB 12U
#define NUM_SUBCARRIERS_SSB 240U
#define SIZE_SYNC_RASTER_1_HZ 1200e3
#define SIZE_SYNC_RASTER_1_M_HZ 50e3
#define SIZE_SYNC_RASTER_2_HZ 1440e3

using namespace srsgnb;

using namespace srs_du;

class ssb_freq_location_generator;

class du_config_generator
{
  friend class ssb_freq_location_generator;

public:
  explicit du_config_generator(unsigned           dl_arfcn,
                               unsigned           nr_band,
                               unsigned           n_rbs_,
                               subcarrier_spacing scs_common,
                               subcarrier_spacing scs_ssb);

  unsigned           dl_arfcn;
  unsigned           nr_band;
  unsigned           n_rbs;
  subcarrier_spacing scs_common;
  subcarrier_spacing scs_ssb;
  double             point_A_hz;
  double             f_ref_hz;
  /// Band upper-bound, we consider the last subcarrier of the last CRB as band upper-bound.
  double                bw_ub_hz;
  ssb_offset_to_pointA  offset_to_point_A;
  ssb_subcarrier_offset k_ssb;
  unsigned              coreset0_idx;
  unsigned              searchspace0_idx;
  ssb_pattern_case      ssb_case;
  unsigned              ssb_first_symbol;

  std::vector<unsigned> searchspace0_idx_list;
};

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
  bw_ub_hz   = point_A_hz + n_rbs * NUM_SUBCARRIERS_PER_RB * scs_to_khz(scs_common) * 1000;

  ssb_first_symbol = ssb_case == ssb_pattern_case::B ? 4 : 2;

  for (unsigned ss0_idx = 0; ss0_idx < 10; ++ss0_idx) {
    searchspace0_idx_list.push_back(ss0_idx);
  }
}

struct ssb_freq_location {
  bool                  is_valid;
  ssb_offset_to_pointA  offset_to_point_A;
  ssb_subcarrier_offset k_ssb;
};

class ssb_freq_location_generator
{
public:
  explicit ssb_freq_location_generator(const du_config_generator& bw_params);

  ssb_freq_location get_next_ssb();

private:
  unsigned find_M_raster();
  double   get_ss_ref_hz(unsigned N, unsigned M = 0);

  du_config_generator bw_parameters;
  /// This is the \f$N\f$ parameter in the synchronization raster.
  unsigned N_raster;
  /// This is the \f$M\f$ parameter in the synchronization raster.
  unsigned M_raster;
};

ssb_freq_location_generator::ssb_freq_location_generator(const du_config_generator& bw_params) :
  bw_parameters(bw_params), N_raster{0}, M_raster{0}
{
  double f_ssb_min_hz = bw_parameters.point_A_hz +
                        static_cast<double>(scs_to_khz(bw_parameters.scs_ssb) * 1000 * NUM_SUBCARRIERS_SSB / 2);

  if (bw_parameters.dl_arfcn < 600000) {
    N_raster = static_cast<unsigned>(std::floor(f_ssb_min_hz / SIZE_SYNC_RASTER_1_HZ));
  } else if (bw_parameters.dl_arfcn >= 600000 && bw_parameters.dl_arfcn < 2016667) {
    N_raster = static_cast<unsigned>(std::floor((f_ssb_min_hz - 3e9) / SIZE_SYNC_RASTER_2_HZ));
  } else {
    srsgnb_assert(bw_parameters.dl_arfcn < 2016667, "FR2 frequencies not supported");
  }
}

double ssb_freq_location_generator::get_ss_ref_hz(unsigned N, unsigned M)
{
  double ss_ref = bw_parameters.dl_arfcn < 600000 ? static_cast<double>(N) * SIZE_SYNC_RASTER_1_HZ +
                                                        static_cast<double>(M) * SIZE_SYNC_RASTER_1_M_HZ
                                                  : 3e9 + static_cast<double>(N) * SIZE_SYNC_RASTER_2_HZ;
  return ss_ref;
}

static double get_f_ssb_0_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref - static_cast<double>(scs_to_khz(scs_ssb) * 1000 * NUM_SUBCARRIERS_SSB / 2);
}

static double get_f_ssb_ub_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref + static_cast<double>(scs_to_khz(scs_ssb) * 1000 * NUM_SUBCARRIERS_SSB / 2);
}

static ssb_offset_to_pointA
compute_offset_to_pointA(double f_ssb_0_hz, double point_A_hz, subcarrier_spacing scs_common)
{
  ssb_offset_to_pointA offset_to_pA{0};
  if (scs_common == subcarrier_spacing::kHz15) {
    offset_to_pA = static_cast<unsigned>((f_ssb_0_hz - point_A_hz) * 1e-3) /
                   (NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15));
  } else if (scs_common == subcarrier_spacing::kHz30) {
    offset_to_pA = 2 * static_cast<unsigned>((f_ssb_0_hz - point_A_hz) * 1e-3) /
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
      point_A_hz * 1e-3 +
      static_cast<double>(NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15) * offset_to_pA.to_uint());
  k_ssb = static_cast<unsigned>(f_ssb_0_hz * 1e-3 - f_crb_ssb_khz) / scs_to_khz(subcarrier_spacing::kHz15);

  return k_ssb;
}

unsigned ssb_freq_location_generator::find_M_raster()
{
  srsgnb_assert(bw_parameters.dl_arfcn < 600000, "This function should be called only for DL-ARFCN < 600000");

  // If M_raster > 0, no need  to find the same value again.
  if (M_raster > 0) {
    return M_raster;
  }

  unsigned M = 1;
  while (M <= 5) {
    double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M);
    double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, bw_parameters.scs_ssb);
    double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, bw_parameters.scs_ssb);

    bool is_ssb_inside_band = (f_ssb_0_hz >= bw_parameters.point_A_hz) && (f_ssb_ub_hz <= bw_parameters.bw_ub_hz);

    if (is_ssb_inside_band) {
      bool is_scs_30khz_spacing =
          bw_parameters.scs_common == subcarrier_spacing::kHz30 && bw_parameters.scs_ssb == subcarrier_spacing::kHz30;
      bool is_multiple_of_scs = is_scs_30khz_spacing
                                    ? fmod(static_cast<unsigned>(f_ssb_0_hz - bw_parameters.point_A_hz),
                                           scs_to_khz(subcarrier_spacing::kHz30) * 1000) == 0.0
                                    : fmod(static_cast<unsigned>(f_ssb_0_hz - bw_parameters.point_A_hz),
                                           scs_to_khz(subcarrier_spacing::kHz15) * 1000) == 0.0;

      if (is_multiple_of_scs) {
        M_raster = M;
        return M_raster;
      }
    }

    // As per TS 38.104, Section 5.4.3.1, increment M by 2 to compute next sync raster.
    M += 2;
  }
  return 0;
}

ssb_freq_location ssb_freq_location_generator::get_next_ssb()
{
  ssb_freq_location ssb{.is_valid = false};

  static const unsigned N_raster_max = bw_parameters.dl_arfcn < 600000 ? 2500 : 14757;

  while (N_raster < N_raster_max) {
    // If M_raster > 0, no need  to find the same value again.
    if (M_raster == 0) {
      M_raster = find_M_raster();
    }

    double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M_raster);
    double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, bw_parameters.scs_ssb);
    double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, bw_parameters.scs_ssb);

    bool is_ssb_inside_band = (f_ssb_0_hz >= bw_parameters.point_A_hz) && (f_ssb_ub_hz <= bw_parameters.bw_ub_hz);

    if (is_ssb_inside_band) {
      bool is_scs_30khz_spacing =
          bw_parameters.scs_common == subcarrier_spacing::kHz30 && bw_parameters.scs_ssb == subcarrier_spacing::kHz30;
      bool is_multiple_of_scs =
          is_scs_30khz_spacing
              ? fmod(f_ssb_0_hz - bw_parameters.point_A_hz,
                     static_cast<double>(scs_to_khz(subcarrier_spacing::kHz30) * 1000)) == 0.0
              : fmod(f_ssb_0_hz - bw_parameters.point_A_hz, scs_to_khz(subcarrier_spacing::kHz15) * 1000) == 0.0;

      if (is_multiple_of_scs) {
        ssb.offset_to_point_A =
            compute_offset_to_pointA(f_ssb_0_hz, bw_parameters.point_A_hz, bw_parameters.scs_common);
        ssb.k_ssb    = compute_k_ssb(f_ssb_0_hz, bw_parameters.point_A_hz, ssb.offset_to_point_A);
        ssb.is_valid = true;
        N_raster++;
        return ssb;
      }
    }

    // Increment N_raster.
    N_raster++;
  }

  return ssb;
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

du_ssb_sib1_location srsgnb::srs_du::get_ssb_sib1_freq_location(unsigned           dl_arfcn,
                                                                unsigned           nr_band,
                                                                unsigned           n_rbs,
                                                                subcarrier_spacing scs_common,
                                                                subcarrier_spacing scs_ssb)
{
  du_ssb_sib1_location params{.is_valid = false};

  // Get f_ref, point_A from dl_arfcn.
  du_config_generator du_cfg{dl_arfcn, nr_band, n_rbs, scs_common, scs_ssb};

  // Get SSB position.
  ssb_freq_location_generator ssb_generator{du_cfg};

  // Select 1st SSB. Select first searchspace0_idx and find viable coreset0
  ssb_freq_location ssb = ssb_generator.get_next_ssb();
  while (not ssb.is_valid) {
    unsigned crbs_ssb =
        scs_common == subcarrier_spacing::kHz15 ? ssb.offset_to_point_A.to_uint() : ssb.offset_to_point_A.to_uint() / 2;
    // Iterate over

    for (uint8_t ss0_idx = 0; ss0_idx < 9; ++ss0_idx) {
      pdcch_type0_css_occasion_pattern1_description ss0_config =
          pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
              .is_fr2 = false, .ss_zero_index = ss0_idx, .nof_symb_coreset = 1});

      unsigned max_cset0_idx =
          get_max_coreset0_index(scs_common, scs_ssb, band_helper::get_min_channel_bw(nr_band, scs_common));

      for (unsigned cset0_idx = 0; cset0_idx < max_cset0_idx; ++cset0_idx) {
        auto coreset0_cfg = pdcch_type0_css_coreset_get(
            band_helper::get_min_channel_bw(nr_band, scs_common), scs_ssb, scs_common, cset0_idx, ssb.k_ssb.to_uint());

        bool coreset0_not_below_pointA = crbs_ssb >= static_cast<unsigned>(coreset0_cfg.offset);
        bool coreset0_not_above_bw_ub =
            crbs_ssb - static_cast<unsigned>(coreset0_cfg.offset) + coreset0_cfg.nof_rb_coreset <= n_rbs;
        bool ss0_not_overlapping_ssb_symbol =
            ss0_config.offset == 0 ? coreset0_cfg.nof_symb_coreset <= du_cfg.ssb_first_symbol : true;

        if (coreset0_not_below_pointA && coreset0_not_above_bw_ub && ss0_not_overlapping_ssb_symbol) {
          params.offset_to_point_A = ssb.offset_to_point_A;
          params.k_ssb             = ssb.k_ssb;
          params.coreset0_idx      = cset0_idx;
          params.searchspace0_idx  = ss0_idx;
          params.is_valid          = true;
          return params;
        } // end of if
      }   // end of for
    }

    ssb = ssb_generator.get_next_ssb();
  }

  return params;
}