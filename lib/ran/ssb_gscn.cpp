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

#include "srsran/ran/ssb_gscn.h"
#include "srsran/ran/band_helper_constants.h"
#include <math.h>

using namespace srsran;
using namespace band_helper;

// GSCN set for band n34, SSB case A, as per TS 38.104, Table 5.4.3.3-1, Note 3.
static const std::array<unsigned, 3> gscn_band_n34_ssb_caseA = {5032, 5043, 5054};

// GSCN set for band n38, SSB case A, as per TS 38.104, Table 5.4.3.3-1, Note 2.
static const std::array<unsigned, 10> gscn_band_n38_ssb_caseA =
    {6432, 6443, 6457, 6468, 6479, 6493, 6507, 6518, 6532, 6543};

// GSCN set for band n39, SSB case A, as per TS 38.104, Table 5.4.3.3-1, Note 4.
static const std::array<unsigned, 14> gscn_band_n39_ssb_caseA =
    {4707, 4715, 4718, 4729, 4732, 4743, 4747, 4754, 4761, 4768, 4772, 4782, 4786, 4793};

// GSCN set for band n46, as per TS 38.104, Table 5.4.3.3-1, Note 5.
static const std::array<unsigned, 32> gscn_band_n46 = {
    // clang-format off
  8996, 9010, 9024, 9038, 9051, 9065, 9079, 9093, 9107, 9121, 9218, 9232, 9246, 9260, 9274, 9288, 9301, 9315, 9329,
  9343, 9357, 9371, 9385, 9402, 9416, 9430, 9444, 9458, 9472, 9485, 9499, 9513
    // clang-format on
};

// GSCN set for band n96, as per TS 38.104, Table 5.4.3.3-1, Note 6.
static const std::array<unsigned, 59> gscn_band_n96 = {
    // clang-format off
  9548, 9562, 9576, 9590, 9603, 9617, 9631, 9645, 9659, 9673, 9687, 9701, 9714, 9728, 9742, 9756, 9770, 9784, 9798,
  9812, 9826, 9840, 9853, 9867, 9881, 9895, 9909, 9923, 9937, 9951, 9964, 9978, 9992, 10006, 10020, 10034, 10048, 10062,
  10076, 10090, 10103,10117, 10131, 10145, 10159, 10173, 10187, 10201, 10214, 10228, 10242, 10256, 10270, 10284, 10298,
  10312, 10325, 10339, 10353
    // clang-format on
};

// GSCN set for band n102, as per TS 38.104, Table 5.4.3.3-1, Note 9.
static const std::array<unsigned, 25> gscn_band_n102 = {
    // clang-format off
  9535, 9548, 9562, 9576, 9590, 9603, 9617, 9631, 9645, 9659, 9673, 9687, 9701, 9714, 9728, 9742, 9756, 9770, 9784,
  9798, 9812, 9826, 9840, 9853, 9867
    // clang-format on
};

struct ssb_gscn_raster {
  nr_band            band;
  subcarrier_spacing scs;
  // Initial value of the GSCN interval, as per Table 5.4.3.3-1, TS 38.104.
  unsigned gscn_first;
  // Step value of the GSCN interval, as per Table 5.4.3.3-1, TS 38.104.
  unsigned gscn_step;
  // Final (valid) value of the GSCN interval, as per Table 5.4.3.3-1, TS 38.104.
  unsigned gscn_last;
};

// This table implements Table 5.4.3.3-1, TS 38.104.
const size_t                                                  nof_gscn_raster_fr1 = 51U;
static const std::array<ssb_gscn_raster, nof_gscn_raster_fr1> ssb_gscn_raster_table_fr1{{
    // clang-format off
    {nr_band::n1, subcarrier_spacing::kHz15,  5279, 1, 5419},
    {nr_band::n2, subcarrier_spacing::kHz15,  4829, 1, 4969},
    {nr_band::n3, subcarrier_spacing::kHz15,  4517, 1, 4693},
    {nr_band::n5, subcarrier_spacing::kHz15,  2177, 1, 2230},
    {nr_band::n5, subcarrier_spacing::kHz30,  2183, 1, 2224},
    {nr_band::n7, subcarrier_spacing::kHz15,  6554, 1, 6718},
    {nr_band::n8, subcarrier_spacing::kHz15,  2318, 1, 2395},
    {nr_band::n12, subcarrier_spacing::kHz15, 1828, 1, 1858},
    {nr_band::n13, subcarrier_spacing::kHz15, 1871, 1, 1885},
    {nr_band::n14, subcarrier_spacing::kHz30, 1901, 1, 1915},
    {nr_band::n18, subcarrier_spacing::kHz15, 2156, 1, 2182},
    {nr_band::n20, subcarrier_spacing::kHz15, 1982, 1, 2047},
    {nr_band::n24, subcarrier_spacing::kHz15, 3818, 1, 3892},
    {nr_band::n24, subcarrier_spacing::kHz30, 3824, 1, 3886},
    {nr_band::n25, subcarrier_spacing::kHz15, 4829, 1, 4981},
    {nr_band::n26, subcarrier_spacing::kHz15, 2153, 1, 2230},
    {nr_band::n28, subcarrier_spacing::kHz15, 1901, 1, 2002},
    {nr_band::n29, subcarrier_spacing::kHz15, 1798, 1, 1813},
    {nr_band::n30, subcarrier_spacing::kHz15, 5879, 1, 5893},
    {nr_band::n34, subcarrier_spacing::kHz30, 5036, 1, 5050},
    {nr_band::n38, subcarrier_spacing::kHz30, 6437, 1, 6538},
    {nr_band::n39, subcarrier_spacing::kHz30, 4712, 1, 4789},
    {nr_band::n40, subcarrier_spacing::kHz30, 5762, 1, 5989},
    {nr_band::n41, subcarrier_spacing::kHz15, 6246, 3, 6717},
    {nr_band::n41, subcarrier_spacing::kHz30, 6252, 3, 6714},
    {nr_band::n48, subcarrier_spacing::kHz30, 7884, 1, 7982},
    {nr_band::n50, subcarrier_spacing::kHz30, 3590, 1, 3781},
    {nr_band::n51, subcarrier_spacing::kHz15, 3572, 1, 3574},
    {nr_band::n53, subcarrier_spacing::kHz15, 6215, 1, 6232},
    {nr_band::n65, subcarrier_spacing::kHz15, 5279, 1, 5494},
    {nr_band::n66, subcarrier_spacing::kHz15, 5279, 1, 5494},
    {nr_band::n66, subcarrier_spacing::kHz30, 5285, 1, 5488},
    {nr_band::n67, subcarrier_spacing::kHz15, 1850, 1, 1888},
    {nr_band::n70, subcarrier_spacing::kHz15, 4993, 1, 5044},
    {nr_band::n71, subcarrier_spacing::kHz15, 1547, 1, 1624},
    {nr_band::n74, subcarrier_spacing::kHz15, 3692, 1, 3790},
    {nr_band::n75, subcarrier_spacing::kHz15, 3584, 1, 3787},
    {nr_band::n76, subcarrier_spacing::kHz15, 3572, 1, 3574},
    {nr_band::n77, subcarrier_spacing::kHz30, 7711, 1, 8329},
    {nr_band::n78, subcarrier_spacing::kHz30, 7711, 1, 8051},
    {nr_band::n85, subcarrier_spacing::kHz15, 1826, 1, 1858},
    {nr_band::n90, subcarrier_spacing::kHz30, 6252, 1, 6714},
    {nr_band::n91, subcarrier_spacing::kHz15, 3572, 1, 3574},
    {nr_band::n92, subcarrier_spacing::kHz15, 3584, 1, 3787},
    {nr_band::n93, subcarrier_spacing::kHz15, 3572, 1, 3574},
    {nr_band::n94, subcarrier_spacing::kHz15, 3584, 1, 3787},
    {nr_band::n100,subcarrier_spacing::kHz15, 2303, 1, 2307},
    {nr_band::n101,subcarrier_spacing::kHz15, 4754, 1, 4768},
    {nr_band::n101,subcarrier_spacing::kHz30, 4760, 1, 4764},
    {nr_band::n104,subcarrier_spacing::kHz30, 9882, 7, 10358}
    // clang-format on
}};

// Helper that validates the GSCN of bands with irregular or special rasters.
static error_type<std::string>
validate_irregular_gscn_rasters(unsigned gscn, nr_band band, subcarrier_spacing ssb_scs, bs_channel_bandwidth_fr1 bw)
{
  bool is_gscn_valid = false;

  // This function is based on Table 5.4.3.3-1, TS 38.104.
  if (band == nr_band::n34 and ssb_scs == subcarrier_spacing::kHz15) {
    is_gscn_valid = std::find(gscn_band_n34_ssb_caseA.begin(), gscn_band_n34_ssb_caseA.end(), gscn) !=
                    gscn_band_n34_ssb_caseA.end();
  } else if (band == nr_band::n38 and ssb_scs == subcarrier_spacing::kHz15) {
    is_gscn_valid = std::find(gscn_band_n38_ssb_caseA.begin(), gscn_band_n38_ssb_caseA.end(), gscn) !=
                    gscn_band_n38_ssb_caseA.end();
  } else if (band == nr_band::n39 and ssb_scs == subcarrier_spacing::kHz15) {
    is_gscn_valid = std::find(gscn_band_n39_ssb_caseA.begin(), gscn_band_n39_ssb_caseA.end(), gscn) !=
                    gscn_band_n39_ssb_caseA.end();
  } else if (band == nr_band::n46) {
    is_gscn_valid = std::find(gscn_band_n46.begin(), gscn_band_n46.end(), gscn) != gscn_band_n46.end();
  } else if (band == nr_band::n79) {
    const unsigned n79_narrow_bw_gscn_first = 8475U;
    const unsigned n79_narrow_bw_gscn_last  = 8884U;
    const unsigned n79_large_bw_gscn_first  = 8480U;
    const unsigned n79_large_bw_gscn_step   = 16U;
    const unsigned n79_large_bw_gscn_last   = 8880U;
    if (bw < bs_channel_bandwidth_fr1::MHz40) {
      is_gscn_valid = gscn >= n79_narrow_bw_gscn_first and gscn <= n79_narrow_bw_gscn_last;
    } else {
      is_gscn_valid = gscn >= n79_large_bw_gscn_first and gscn <= n79_large_bw_gscn_last and
                      (gscn - n79_large_bw_gscn_first) % n79_large_bw_gscn_step == 0;
    }
  } else if (band == nr_band::n90 and ssb_scs == subcarrier_spacing::kHz15) {
    const unsigned n90_narrow_bw_gscn_first = 6245U;
    const unsigned n90_narrow_bw_gscn_last  = 6718U;
    const unsigned n90_large_bw_gscn_first  = 6246U;
    const unsigned n90_large_bw_gscn_last   = 6717U;
    if (bw < bs_channel_bandwidth_fr1::MHz10) {
      is_gscn_valid = gscn >= n90_narrow_bw_gscn_first and gscn <= n90_narrow_bw_gscn_last;
    } else {
      is_gscn_valid = gscn >= n90_large_bw_gscn_first and gscn <= n90_large_bw_gscn_last;
    }
  } else if (band == nr_band::n96) {
    is_gscn_valid = std::find(gscn_band_n96.begin(), gscn_band_n96.end(), gscn) != gscn_band_n96.end();
  } else if (band == nr_band::n102) {
    is_gscn_valid = std::find(gscn_band_n102.begin(), gscn_band_n102.end(), gscn) != gscn_band_n102.end();
  }

  return is_gscn_valid ? error_type<std::string>{}
                       : error_type<std::string>{
                             fmt::format("GSCN {} is not valid for band {} with SSB SCS {}", gscn, band, ssb_scs)};
}

error_type<std::string> srsran::band_helper::is_gscn_valid_given_band(unsigned                 gscn,
                                                                      nr_band                  band,
                                                                      subcarrier_spacing       ssb_scs,
                                                                      bs_channel_bandwidth_fr1 bw)
{
  // Search for the GSCN in the table of regular rasters, first.
  for (const ssb_gscn_raster& raster : ssb_gscn_raster_table_fr1) {
    if (raster.band == band and raster.scs == ssb_scs) {
      if (gscn >= raster.gscn_first and gscn <= raster.gscn_last and
          ((gscn - raster.gscn_first) % raster.gscn_step) == 0) {
        return {};
      }
      return {fmt::format("GSCN must be within the interval [{},{}], in steps of {}, for the chosen band",
                          raster.gscn_first,
                          raster.gscn_last,
                          raster.gscn_step)};
    }
  }

  // If the GSCN is not found in the regular rasters, then try to see if it belongs to the irregular ones.
  return validate_irregular_gscn_rasters(gscn, band, ssb_scs, bw);
}

span<const unsigned> srsran::band_helper::get_gscn_special_raster_iterator(nr_band band, subcarrier_spacing ssb_scs)
{
  bool are_input_args_valid = ((band == nr_band::n34 or band == nr_band::n38 or band == nr_band::n39) and
                               ssb_scs == subcarrier_spacing::kHz15) or
                              band == nr_band::n46 or band == nr_band::n96 or band == nr_band::n102;

  srsran_assert(
      are_input_args_valid, "This function cannot be used for band {} with SCS {}", band, scs_to_khz(ssb_scs));

  switch (band) {
    case nr_band::n34:
      return span<const unsigned>{gscn_band_n34_ssb_caseA.begin(), gscn_band_n34_ssb_caseA.end()};
    case nr_band::n38:
      return span<const unsigned>{gscn_band_n38_ssb_caseA.begin(), gscn_band_n38_ssb_caseA.end()};
    case nr_band::n39:
      return span<const unsigned>{gscn_band_n39_ssb_caseA.begin(), gscn_band_n39_ssb_caseA.end()};
    case nr_band::n46:
      return span<const unsigned>{gscn_band_n46.begin(), gscn_band_n46.size()};
    case nr_band::n96:
      return span<const unsigned>{gscn_band_n96.begin(), gscn_band_n96.end()};
    case nr_band::n102:
      return span<const unsigned>{gscn_band_n102.begin(), gscn_band_n102.end()};
    default:
      return span<const unsigned>{};
  }
}

double band_helper::get_ss_ref_from_gscn(unsigned gscn)
{
  srsran_assert(gscn >= MIN_GSCN_FREQ_0_3GHZ and gscn <= band_helper::MIN_ARFCN_24_5_GHZ_100_GHZ,
                "GSCN must be within the [{},{}] interval",
                MIN_GSCN_FREQ_0_3GHZ,
                MAX_GSCN_FREQ_24_5_GHZ_100_GHZ);

  double ss_ref = 0;

  if (gscn >= MIN_GSCN_FREQ_0_3GHZ and gscn < MIN_GSCN_FREQ_3GHZ_24_5GHZ) {
    const int gscn_int = static_cast<int>(gscn);
    int       M        = 1;
    // As per Table 5.4.3.1-1, TS 38.104, case 0MHz – 3000MHz, the maximum value for M is 5.
    const int max_M = 5;
    // As per Table 5.4.3.1-1, TS 38.104, case 0MHz – 3000MHz, get parameters N and M from GSCN. Starting from the
    // equation that gives GSCN as a function of N and M, first, we need to find the M value such that
    // GSCN - (M-3)/2 is divisible by 3; then, we use this value for M to compute N = ( GSCN - (M-3)/2 ) / 3.
    while ((gscn_int - (M - 3) / 2) % 3 != 0) {
      if (M > max_M) {
        srsran_terminate("The parameter M to compute GSCN must be one following vales {1, 3, 5}");
        return 0.0;
      }
      M += 2;
    }
    const int N = (gscn_int - (M - 3) / 2) / 3;
    // As per Table 5.4.3.1-1, TS 38.104, case 0MHz – 3000MHz, \f$SS_{ref}\f$ is given as a function of N and M.
    ss_ref = static_cast<double>(N) * 1200000.0 + static_cast<double>(M) * 50000.0;
  } else if (gscn < MIN_GSCN_FREQ_24_5_GHZ_100_GHZ) {
    // As per Table 5.4.3.1-1, TS 38.104, case 3000MHz – 24250MHz, get parameter N from GSCN and use N to compute
    // \f$SS_{ref}\f$.
    const double N = static_cast<double>(gscn - MIN_GSCN_FREQ_3GHZ_24_5GHZ);
    ss_ref         = (3000000 + N * 1440) * 1e3;
  } else if (gscn < MAX_GSCN_FREQ_24_5_GHZ_100_GHZ) {
    // As per Table 5.4.3.1-1, TS 38.104, case 24250MHz – 100000MHz, get parameter N from GSCN and use N to compute
    // \f$SS_{ref}\f$.
    const double N = static_cast<double>(gscn - MIN_GSCN_FREQ_24_5_GHZ_100_GHZ);
    ss_ref         = (24250080 + N * 17280) * 1e3;
  }

  return ss_ref;
}

optional<unsigned> srsran::band_helper::get_gscn_from_ss_ref(double ss_ref_hz)
{
  // The following is based on Table 5.4.3.1-1, TS 38.104.
  if (ss_ref_hz < N_REF_OFFSET_3_GHZ_24_5_GHZ) {
    // Check what value of M is used, if any valid.
    for (const int M : {1, 3, 5}) {
      // Get the value of N; at this point, we don't know if N is a valid value, hence we need to consider a double with
      // possibly a fractional part.
      double estimated_N = (ss_ref_hz - static_cast<double>(M) * M_SIZE_SYNC_RASTER_1_HZ) / N_SIZE_SYNC_RASTER_1_HZ;
      // If the estimated N is has no fractional part, then it is a valid value.
      if (static_cast<double>(static_cast<unsigned>(estimated_N)) == estimated_N) {
        return static_cast<unsigned>(3 * static_cast<int>(estimated_N) + (M - 3) / 2);
      }
    }
  } else if (ss_ref_hz < N_REF_OFFSET_24_5_GHZ_100_GHZ) {
    // Get the value of N; at this point, we don't know if N is a valid value, hence we need to consider a double with
    // possibly a fractional part.
    double estimated_N = (ss_ref_hz - N_REF_OFFSET_3_GHZ_24_5_GHZ) / N_SIZE_SYNC_RASTER_2_HZ;
    // If the estimated N is has no fractional part, then it is a valid value.
    if (static_cast<double>(static_cast<unsigned>(estimated_N)) == estimated_N) {
      return MIN_GSCN_FREQ_3GHZ_24_5GHZ + static_cast<unsigned>(estimated_N);
    }
  } else if (ss_ref_hz <= MAX_RASTER_FREQ) {
    // Get the value of N; at this point, we don't know if N is a valid value, hence we need to consider a double with
    // possibly a fractional part.
    double estimated_N = (ss_ref_hz - BASE_FREQ_GSCN_RASTER_24_5_GHZ_100_GHZ) / N_SIZE_SYNC_RASTER_3_HZ;
    // If the estimated N is has no fractional part, then it is a valid value.
    if (static_cast<double>(static_cast<unsigned>(estimated_N)) == estimated_N) {
      return MIN_GSCN_FREQ_24_5_GHZ_100_GHZ + static_cast<unsigned>(estimated_N);
    }
  }

  return nullopt;
}
