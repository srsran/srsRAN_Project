#include "ssb_freq_position_generator.h"

using namespace srsgnb;

// Compute the frequency (in Hz) corresponding to the first subcarrier of the SSB.
static double get_f_ssb_0_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref - static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);
}

// Compute the frequency (in Hz) corresponding to the last subcarrier of the SSB, or upper-bound (ub).
static double get_f_ssb_ub_hz(double ss_ref, subcarrier_spacing scs_ssb)
{
  return ss_ref + static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);
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
    srsgnb_assert(scs_common <= subcarrier_spacing::kHz30, "FR2 not supported");
  }
  return offset_to_pA;
}

// Compute the k_SSB, depending on the SCScommon.
static ssb_subcarrier_offset compute_k_ssb(double f_ssb_0_hz, double point_A_hz, ssb_offset_to_pointA offset_to_pA)
{
  ssb_subcarrier_offset k_ssb;
  unsigned              f_crb_ssb_khz =
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
  n_rbs(n_rbs_),
  scs_common{scs_common_},
  scs_ssb{scs_ssb_},
  ssb_case{band_helper::get_ssb_pattern(nr_band_, scs_ssb)},
  N_raster{0},
  M_raster{0}
{
  // Compute the RF reference frequency, pointA and upper-bound of the band.
  f_ref_hz   = band_helper::nr_arfcn_to_freq(dl_arfcn);
  point_A_hz = band_helper::get_abs_freq_point_a_from_f_ref(f_ref_hz, n_rbs, scs_common);
  bw_ub_hz   = point_A_hz + n_rbs * NOF_SUBCARRIERS_PER_RB * scs_to_khz(scs_common) * KHZ_TO_HZ;

  // Lower-bound for the SSB central frequency, or SS_ref, as a function of pointA.
  double ss_ref_l_bound_hz =
      point_A_hz + static_cast<double>(scs_to_khz(scs_ssb) * KHZ_TO_HZ * NOF_SSB_SUBCARRIERS / 2);

  // Get the starting point of parameter N for the sync-raster. This allows us to avoid the generation of all possible
  // SSB positions in the raster.
  if (dl_arfcn < MIN_ARFCN_3_GHZ_24_5_GHZ) {
    N_raster = static_cast<unsigned>(std::floor(ss_ref_l_bound_hz / N_SIZE_SYNC_RASTER_1_HZ));
  } else if (dl_arfcn >= MIN_ARFCN_3_GHZ_24_5_GHZ && dl_arfcn < MIN_ARFCN_24_5_GHZ_100_GHZ) {
    N_raster =
        static_cast<unsigned>(std::floor((ss_ref_l_bound_hz - N_REF_OFFSET_3_GHZ_24_5_GHZ) / N_SIZE_SYNC_RASTER_2_HZ));
  } else {
    srsgnb_assert(dl_arfcn < MIN_ARFCN_24_5_GHZ_100_GHZ, "FR2 frequencies not supported");
  }

  // This constraint comes from the scheduler, which cannot allocate PDCCH starting at symbols different from 0;
  // therefore, as per Table 13-11, TS 38.213, only the searchSpaceZero indices with starting symbol 0 are those with
  // index <= 9.
  max_ss0_idx = 9;

  // As per TS38.213, Section 4.1.
  ssb_first_symbol = ssb_case == ssb_pattern_case::B ? 4 : 2;
}

double ssb_freq_position_generator::get_ss_ref_hz(unsigned N, unsigned M)
{
  // Get SS_ref given the parameters N, M, as per Table 5.4.3.1-1, TS 38.104.
  double ss_ref =
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

  for (unsigned M : {1, 3, 5}) {
    double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M);
    double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, scs_ssb);
    double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, scs_ssb);

    bool is_ssb_inside_band = (f_ssb_0_hz >= point_A_hz) && (f_ssb_ub_hz <= bw_ub_hz);

    // An SSB centre frequency SS_ref is satisfactory if (i) if the SSB falls within the band and (ii) SS_ref falls into
    // the subcarrier grid of CRBs; or, equivalently, if the distance of the first SSB's subcarrier from pointA can be
    // expressed as a multiple of the min(SCS_common, SCS_SSB).
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

ssb_freq_location ssb_freq_position_generator::get_next_ssb_location()
{
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

    double f_ssb_N_M_hz = get_ss_ref_hz(N_raster, M_raster);
    double f_ssb_0_hz   = get_f_ssb_0_hz(f_ssb_N_M_hz, scs_ssb);
    double f_ssb_ub_hz  = get_f_ssb_ub_hz(f_ssb_N_M_hz, scs_ssb);

    // An SSB centre frequency SS_ref is satisfactory if (i) if the SSb falls within the band and (ii) SS_ref falls into
    // the subcarrier grid of CRBs; or, equivalently, if the distance of the first SSB's subcarrier from pointA can be
    // expressed as a multiple of SCS.
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
        ssb.ss_ref            = f_ssb_N_M_hz;
        N_raster++;
        return ssb;
      }
    }

    // Increment N_raster.
    N_raster++;
  }

  return ssb;
}