#include "ssb_freq_position_generator.h"
#include "srsran/adt/span.h"

using namespace srsran;

static const std::array<unsigned, 32> gscn_band_n46 = {
    // clang-format off
      8996, 9010, 9024, 9038, 9051, 9065, 9079, 9093, 9107, 9121, 9218, 9232, 9246, 9260, 9274, 9288,
      9301, 9315, 9329, 9343, 9357, 9371, 9385, 9402, 9416, 9430, 9444, 9458, 9472, 9485, 9499, 9513
    // clang-format on
};

static const std::array<unsigned, 59> gscn_band_n96 = {
    // clang-format off
      9548, 9562, 9576, 9590, 9603, 9617, 9631, 9645, 9659, 9673, 9687, 9701, 9714, 9728, 9742, 9756, 9770, 9784, 9798,
      9812, 9826, 9840, 9853, 9867, 9881, 9895, 9909, 9923, 9937, 9951, 9964, 9978, 9992, 10006, 10020, 10034, 10048,
      10062, 10076, 10090, 10103,10117, 10131, 10145, 10159, 10173, 10187, 10201, 10214, 10228, 10242, 10256, 10270,
      10284, 10298, 10312, 10325, 10339, 10353
    // clang-format on
};

static const std::array<unsigned, 25> gscn_band_n102 = {
    // clang-format off
      9535, 9548, 9562, 9576, 9590, 9603, 9617, 9631, 9645, 9659, 9673, 9687, 9701, 9714, 9728, 9742, 9756, 9770, 9784,
      9798, 9812, 9826, 9840, 9853, 9867
    // clang-format on
};

// Compute the frequency (in Hz) corresponding to the first subcarrier of the SSB.
static double get_f_ssb_0_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref - static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);
}

// Compute the frequency (in Hz) corresponding to the last subcarrier of the SSB, or upper-bound (ub).
// NOTE: The difference between get_f_ssb_ub_hz() and get_f_ssb_0_hz() is 1 SCS smaller than  NOF_SSB_SUBCARRIERS *
// SCS_SSB. This is because the distance between the first and last subcarrier (in numbers of subcarriers)  is
// NOF_SSB_SUBCARRIERS - 1.
static double get_f_ssb_ub_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref + static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2 - 1);
}

// Compute the offsetToPointA, depending on the SCScommon.
static ssb_offset_to_pointA
compute_offset_to_pointA(double f_ssb_0_hz, double point_A_hz, subcarrier_spacing scs_common)
{
  ssb_offset_to_pointA offset_to_pA{0};
  if (scs_common == subcarrier_spacing::kHz15) {
    offset_to_pA = static_cast<unsigned>((f_ssb_0_hz - point_A_hz) * HZ_TO_KHZ) /
                   (NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15));
  } else if (scs_common == subcarrier_spacing::kHz30) {
    offset_to_pA = 2 * (static_cast<unsigned>((f_ssb_0_hz - point_A_hz) * HZ_TO_KHZ) /
                        (2 * NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15)));
  } else {
    srsran_assert(scs_common <= subcarrier_spacing::kHz30, "FR2 not supported");
  }
  return offset_to_pA;
}

// Compute the k_SSB, depending on the SCScommon.
static ssb_subcarrier_offset compute_k_ssb(double f_ssb_0_hz, double point_A_hz, ssb_offset_to_pointA offset_to_pA)
{
  ssb_subcarrier_offset k_ssb;
  const double          f_crb_ssb_khz =
      point_A_hz * HZ_TO_KHZ +
      static_cast<double>(NOF_SUBCARRIERS_PER_RB * scs_to_khz(subcarrier_spacing::kHz15) * offset_to_pA.to_uint());
  k_ssb = static_cast<unsigned>(f_ssb_0_hz * HZ_TO_KHZ - f_crb_ssb_khz) / scs_to_khz(subcarrier_spacing::kHz15);

  return k_ssb;
}

ssb_freq_position_generator::ssb_freq_position_generator(unsigned           dl_arfcn_,
                                                         nr_band            nr_band_,
                                                         unsigned           n_rbs_,
                                                         subcarrier_spacing scs_common_,
                                                         subcarrier_spacing scs_ssb_) :
  dl_arfcn{dl_arfcn_},
  band(nr_band_),
  n_rbs(n_rbs_),
  scs_common{scs_common_},
  scs_ssb{scs_ssb_},
  ssb_case{band_helper::get_ssb_pattern(nr_band_, scs_ssb)},
  N_raster{0},
  M_raster{0},
  gscn_raster_idx{0}
{
  // Compute the RF reference frequency, pointA and upper-bound of the band.
  f_ref_hz   = band_helper::nr_arfcn_to_freq(dl_arfcn);
  point_A_hz = band_helper::get_abs_freq_point_a_from_f_ref(f_ref_hz, n_rbs, scs_common);
  // Frequency (in Hz) corresponding to the last subcarrier of the Channel Band, or upper-bound (ub).
  // NOTE: The difference between bw_ub_hz and point_A_hz is 1 SCS smaller than n_rbs * NOF_SUBCARRIERS_PER_RB *
  // SCS_common. This is because the distance between the first and last subcarrier (in numbers of subcarriers)  is
  // NOF_SUBCARRIERS_PER_RB - 1.
  bw_ub_hz = point_A_hz + static_cast<double>(n_rbs * NOF_SUBCARRIERS_PER_RB * scs_to_khz(scs_common) * KHZ_TO_HZ - 1);

  // Lower-bound for the SSB central frequency, or SS_ref, as a function of pointA.
  const double ss_ref_l_bound_hz =
      point_A_hz + static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);

  // Get the starting point of parameter N for the sync-raster. This allows us to avoid the generation of all possible
  // SSB positions in the raster.
  if (dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ) {
    N_raster = static_cast<unsigned>(std::floor(ss_ref_l_bound_hz / N_SIZE_SYNC_RASTER_1_HZ));
  } else if (dl_arfcn >= MIN_ARFCN_3_GHZ_24_5_GHZ && dl_arfcn < MIN_ARFCN_24_5_GHZ_100_GHZ) {
    // Band n79 has a different sync raster step size and need to be handled separately.
    if (band == nr_band::n79) {
      N_raster = GSCN_LB_N_90_BW_40_MHZ - GSCN_LB_SYNC_RASTER_2;
      while (get_ss_ref_hz(N_raster, 0) >= ss_ref_l_bound_hz) {
        increase_N_raster();
      }
    } else {
      N_raster = static_cast<unsigned>(
          std::floor((ss_ref_l_bound_hz - N_REF_OFFSET_3_GHZ_24_5_GHZ) / N_SIZE_SYNC_RASTER_2_HZ));
    }
  } else {
    srsran_assert(dl_arfcn < MIN_ARFCN_24_5_GHZ_100_GHZ, "FR2 frequencies not supported");
  }

  // This constraint comes from the scheduler, which cannot allocate PDCCH starting at symbols different from 0;
  // therefore, as per Table 13-11, TS 38.213, only the searchSpaceZero indices with starting symbol 0 are those with
  // index <= 9.
  max_ss0_idx = 9;

  // As per TS38.213, Section 4.1.
  ssb_first_symbol = ssb_case == ssb_pattern_case::B ? 4 : 2;
}

double ssb_freq_position_generator::get_ss_ref_hz(unsigned N, unsigned M) const
{
  // Get SS_ref given the parameters N, M, as per Table 5.4.3.1-1, TS 38.104.
  const double ss_ref =
      dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ
          ? static_cast<double>(N) * N_SIZE_SYNC_RASTER_1_HZ + static_cast<double>(M) * M_SIZE_SYNC_RASTER_1_HZ
          : N_REF_OFFSET_3_GHZ_24_5_GHZ + static_cast<double>(N) * N_SIZE_SYNC_RASTER_2_HZ;
  return ss_ref;
}

unsigned ssb_freq_position_generator::find_M_raster()
{
  // If M_raster > 0, no need  to find the same value again.
  if (M_raster > 0) {
    return M_raster;
  }

  for (const unsigned M : {1, 3, 5}) {
    const double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M);
    const double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, scs_ssb);
    const double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, scs_ssb);

    const bool is_ssb_inside_band = (f_ssb_0_hz >= point_A_hz) && (f_ssb_ub_hz <= bw_ub_hz);

    // An SSB centre frequency SS_ref is satisfactory if (i) if the SSB falls within the band and (ii) SS_ref falls into
    // the subcarrier grid of CRBs; or, equivalently, if the distance of the first SSB's subcarrier from pointA can be
    // expressed as a multiple of the min(SCS_common, SCS_SSB).
    // NOTES:
    // 1) As the TS reports no explicit definition of how to determine the condition "SSB falls within the band", this
    // is determined based on the position of the first SSB's subcarrier and first Channel Band subcarrier, for the
    // lower bound; and based on the position of the last SSB's subcarrier and last Channel Band subcarrier, for the
    // upper bound.
    // 2) Condition "SS_ref falls into the subcarrier grid of CRBs" is implementation defined and comes from PHY
    // constraints.
    if (is_ssb_inside_band) {
      const bool is_scs_30khz_spacing = scs_common == subcarrier_spacing::kHz30 && scs_ssb == subcarrier_spacing::kHz30;
      const bool is_multiple_of_scs   = is_scs_30khz_spacing
                                            ? fmod(static_cast<unsigned>(f_ssb_0_hz - point_A_hz),
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

void ssb_freq_position_generator::increase_N_raster()
{
  const bool is_band_40mhz_or_above = (scs_common == subcarrier_spacing::kHz15 and n_rbs >= 216U) or
                                      (scs_common == subcarrier_spacing::kHz15 and n_rbs >= 106U) or
                                      (scs_common == subcarrier_spacing::kHz15 and n_rbs >= 51U);

  // N_raster increase as per Table 5.4.3.3-1, TS 38.104, ver.17.8.0.
  if (band == nr_band::n79 and is_band_40mhz_or_above) {
    N_raster += 16U;
  } else if (band == nr_band::n104) {
    N_raster += 7U;
  } else {
    ++N_raster;
  }
}

ssb_freq_location ssb_freq_position_generator::get_next_ssb_location()
{
  if (band == nr_band::n46 or band == nr_band::n96 or band == nr_band::n102) {
    return get_next_ssb_location_special_raster();
  }

  ssb_freq_location ssb{.is_valid = false};

  static const unsigned N_raster_max = dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ ? N_UB_SYNC_RASTER_1 : N_UB_SYNC_RASTER_2;

  while (N_raster < N_raster_max) {
    // If M_raster > 0, no need  to find the same value again.
    // NOTE: (i) Once the value of M is found, all the possible SSB positions within the raster that are viable will be
    // obtained for the same value of M, by varying N.
    //      (ii) For NR-bands above 3GHz, M is not used.
    if (M_raster == 0) {
      M_raster = find_M_raster();
    }

    const double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M_raster);
    const double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, scs_ssb);
    const double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, scs_ssb);

    // An SSB centre frequency SS_ref is satisfactory if (i) if the SSB falls within the band and (ii) SS_ref falls into
    // the subcarrier grid of CRBs; or, equivalently, if the distance of the first SSB's subcarrier from pointA can be
    // expressed as a multiple of SCS.
    // NOTES:
    // 1) As the TS reports no explicit definition of how to determine the condition "SSB falls within the band", this
    // is determined based on the position of the first SSB's subcarrier and first Channel Band subcarrier, for the
    // lower bound; and based on the position of the last SSB's subcarrier and last Channel Band subcarrier, for the
    // upper bound.
    // 2) Condition "SS_ref falls into the subcarrier grid of CRBs" is implementation defined and comes from PHY
    // constraints.
    const bool is_ssb_inside_band = (f_ssb_0_hz >= point_A_hz) && (f_ssb_ub_hz <= bw_ub_hz);

    if (is_ssb_inside_band) {
      const bool is_scs_30khz_spacing = scs_common == subcarrier_spacing::kHz30 && scs_ssb == subcarrier_spacing::kHz30;
      const bool is_multiple_of_scs =
          is_scs_30khz_spacing
              ? fmod(f_ssb_0_hz - point_A_hz, static_cast<double>(scs_to_khz(subcarrier_spacing::kHz30) * KHZ_TO_HZ)) ==
                    0.0
              : fmod(f_ssb_0_hz - point_A_hz, scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ) == 0.0;

      if (is_multiple_of_scs) {
        ssb.offset_to_point_A = compute_offset_to_pointA(f_ssb_0_hz, point_A_hz, scs_common);
        ssb.k_ssb             = compute_k_ssb(f_ssb_0_hz, point_A_hz, ssb.offset_to_point_A);
        ssb.is_valid          = true;
        ssb.ss_ref            = f_ssb_N_M_hz;
        // Increment N_raster according to the band.
        increase_N_raster();
        return ssb;
      }
    }

    // Increment N_raster.
    increase_N_raster();
  }

  return ssb;
}

ssb_freq_location ssb_freq_position_generator::get_next_ssb_location_special_raster()
{
  srsran_assert(band == nr_band::n46 or band == nr_band::n96 or band == nr_band::n102,
                "Special sync. raster is only for bands n46, n96, and n102");

  ssb_freq_location ssb{.is_valid = false};

  // Helper that retrieves the size from the specific std::array depending on the band.
  const auto get_gscn_list_size = [band_ = band]() {
    if (band_ == nr_band::n46) {
      return gscn_band_n46.size();
    } else if (band_ == nr_band::n96) {
      return gscn_band_n96.size();
    } else if (band_ == nr_band::n102) {
      return gscn_band_n102.size();
    }
    return static_cast<size_t>(0);
  };

  // Helper that retrieves the element from the specific std::array depending on the band.
  const auto get_gscn_list_item = [band_ = band](size_t idx) {
    if (band_ == nr_band::n46) {
      srsran_assert(idx < gscn_band_n46.size(), "GSCN list index exceeds its size");
      return gscn_band_n46[idx];
    } else if (band_ == nr_band::n96) {
      srsran_assert(idx < gscn_band_n96.size(), "GSCN list index exceeds its size");
      return gscn_band_n96[idx];
    } else if (band_ == nr_band::n102) {
      srsran_assert(idx < gscn_band_n102.size(), "GSCN list index exceeds its size");
      return gscn_band_n102[idx];
    }
    return 0U;
  };

  if (gscn_raster_idx >= get_gscn_list_size()) {
    return ssb;
  }

  for (; gscn_raster_idx < get_gscn_list_size(); ++gscn_raster_idx) {
    const unsigned gscn         = get_gscn_list_item(gscn_raster_idx);
    const double   f_ssb_N_M_hz = band_helper::get_ss_ref_from_gscn(gscn);
    const double   f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, scs_ssb);
    const double   f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, scs_ssb);

    // An SSB centre frequency SS_ref is satisfactory if (i) if the SSB falls within the band and (ii) SS_ref falls
    // into the subcarrier grid of CRBs; or, equivalently, if the distance of the first SSB's subcarrier from pointA
    // can be expressed as a multiple of SCS. NOTES: 1) As the TS reports no explicit definition of how to determine
    // the condition "SSB falls within the band", this is determined based on the position of the first SSB's
    // subcarrier and first Channel Band subcarrier, for the lower bound; and based on the position of the last SSB's
    // subcarrier and last Channel Band subcarrier, for the upper bound. 2) Condition "SS_ref falls into the
    // subcarrier grid of CRBs" is implementation defined and comes from PHY constraints.
    const bool is_ssb_inside_band = (f_ssb_0_hz >= point_A_hz) && (f_ssb_ub_hz <= bw_ub_hz);

    if (is_ssb_inside_band) {
      const bool is_scs_30khz_spacing = scs_common == subcarrier_spacing::kHz30 && scs_ssb == subcarrier_spacing::kHz30;
      const bool is_multiple_of_scs =
          is_scs_30khz_spacing
              ? fmod(f_ssb_0_hz - point_A_hz, static_cast<double>(scs_to_khz(subcarrier_spacing::kHz30) * KHZ_TO_HZ)) ==
                    0.0
              : fmod(f_ssb_0_hz - point_A_hz, scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ) == 0.0;

      if (is_multiple_of_scs) {
        ssb.offset_to_point_A = compute_offset_to_pointA(f_ssb_0_hz, point_A_hz, scs_common);
        ssb.k_ssb             = compute_k_ssb(f_ssb_0_hz, point_A_hz, ssb.offset_to_point_A);
        ssb.is_valid          = true;
        ssb.ss_ref            = f_ssb_N_M_hz;
        // Increment N_raster according to the band.
        ++gscn_raster_idx;
        return ssb;
      }
    }
  }

  return ssb;
}