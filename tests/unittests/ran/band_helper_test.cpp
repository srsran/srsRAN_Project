/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace band_helper;

TEST(test_get_band_from_arfcn, mix_bands)
{
  ASSERT_NE(nr_band::invalid, get_band_from_dl_arfcn(530000U));
  ASSERT_EQ(nr_band::invalid, get_band_from_dl_arfcn(5300000U));
  ASSERT_EQ(nr_band::n1, get_band_from_dl_arfcn(423000U));
  ASSERT_EQ(nr_band::n3, get_band_from_dl_arfcn(365000U));
  ASSERT_EQ(nr_band::n5, get_band_from_dl_arfcn(175000U));
  ASSERT_EQ(nr_band::n7, get_band_from_dl_arfcn(530000U));
  ASSERT_EQ(nr_band::n25, get_band_from_dl_arfcn(399000U));
  ASSERT_EQ(nr_band::n34, get_band_from_dl_arfcn(404000U));
  ASSERT_EQ(nr_band::n38, get_band_from_dl_arfcn(515000U));
  ASSERT_EQ(nr_band::n41, get_band_from_dl_arfcn(499902U));
  ASSERT_EQ(nr_band::n41, get_band_from_dl_arfcn(504000U));
  ASSERT_EQ(nr_band::n50, get_band_from_dl_arfcn(286500U));
  ASSERT_EQ(nr_band::n51, get_band_from_dl_arfcn(285500U));
  ASSERT_EQ(nr_band::n70, get_band_from_dl_arfcn(400000U));
  ASSERT_EQ(nr_band::n77, get_band_from_dl_arfcn(630500U));
}

TEST(test_arfcn_freq_conversion, arfcn_to_freq)
{
  ASSERT_DOUBLE_EQ(3489.42e6, nr_arfcn_to_freq(632628));
  // default refPointA.
  ASSERT_DOUBLE_EQ(3508.92e6, nr_arfcn_to_freq(633928));
  // default ARFCN with freq divisible by 11.52 MHz.
  ASSERT_DOUBLE_EQ(3513.6e6, nr_arfcn_to_freq(634240));
  // n28 n67
  ASSERT_DOUBLE_EQ(703.0e6, nr_arfcn_to_freq(140600));
  ASSERT_DOUBLE_EQ(729.0e6, nr_arfcn_to_freq(145800));
  ASSERT_DOUBLE_EQ(768.0e6, nr_arfcn_to_freq(153600));
  ASSERT_DOUBLE_EQ(788.0e6, nr_arfcn_to_freq(157600));
  // n20
  ASSERT_DOUBLE_EQ(791.0e6, nr_arfcn_to_freq(158200));
  ASSERT_DOUBLE_EQ(801.0e6, nr_arfcn_to_freq(160200));
  ASSERT_DOUBLE_EQ(811.0e6, nr_arfcn_to_freq(162200));
  ASSERT_DOUBLE_EQ(842.0e6, nr_arfcn_to_freq(168400));
  // n32 n75
  ASSERT_DOUBLE_EQ(1452.0e6, nr_arfcn_to_freq(290400));
  ASSERT_DOUBLE_EQ(1472.0e6, nr_arfcn_to_freq(294400));

  // n1
  ASSERT_DOUBLE_EQ(1920.0e6, nr_arfcn_to_freq(384000));
  ASSERT_DOUBLE_EQ(1940.15e6, nr_arfcn_to_freq(388030));
  ASSERT_DOUBLE_EQ(1959.15e6, nr_arfcn_to_freq(391830));
  ASSERT_DOUBLE_EQ(2170.0e6, nr_arfcn_to_freq(434000));
  // n3
  ASSERT_DOUBLE_EQ(1710.0e6, nr_arfcn_to_freq(342000));
  ASSERT_DOUBLE_EQ(1740.0e6, nr_arfcn_to_freq(348000));
  ASSERT_DOUBLE_EQ(1805.0e6, nr_arfcn_to_freq(361000));
  ASSERT_DOUBLE_EQ(1842.5e6, nr_arfcn_to_freq(368500));
  ASSERT_DOUBLE_EQ(1880.0e6, nr_arfcn_to_freq(376000));
  // n5
  ASSERT_DOUBLE_EQ(881.5e6, nr_arfcn_to_freq(176300));
  ASSERT_DOUBLE_EQ(836.5e6, nr_arfcn_to_freq(167300));
  // n7 n38
  ASSERT_DOUBLE_EQ(2500.0e6, nr_arfcn_to_freq(500000));
  ASSERT_DOUBLE_EQ(2540.0e6, nr_arfcn_to_freq(508000));
  ASSERT_DOUBLE_EQ(2610.0e6, nr_arfcn_to_freq(522000));
  ASSERT_DOUBLE_EQ(2690.0e6, nr_arfcn_to_freq(538000));

  // n78
  ASSERT_DOUBLE_EQ(3513.6e6, nr_arfcn_to_freq(634240));
}

TEST(test_arfcn_freq_conversion, freq_to_arfcn)
{
  ASSERT_EQ(632628, freq_to_nr_arfcn(3489.42e6));
  ASSERT_EQ(633928, freq_to_nr_arfcn(3508.92e6));
  ASSERT_EQ(634240, freq_to_nr_arfcn(3513.6e6));
  // n28 n67
  ASSERT_EQ(140600, freq_to_nr_arfcn(703.0e6));
  ASSERT_EQ(145800, freq_to_nr_arfcn(729.0e6));
  ASSERT_EQ(153600, freq_to_nr_arfcn(768.0e6));
  ASSERT_EQ(157600, freq_to_nr_arfcn(788.0e6));
  // n5
  ASSERT_EQ(176300, freq_to_nr_arfcn(881.5e6));
}

TEST(get_ul_arfcn_from_dl_arfcn, mixed_frequencies)
{
  // n5
  ASSERT_EQ(167300, get_ul_arfcn_from_dl_arfcn(176300, {}));

  ASSERT_EQ(500000, get_ul_arfcn_from_dl_arfcn(524000, nr_band::n7));
  ASSERT_EQ(504060, get_ul_arfcn_from_dl_arfcn(528060, nr_band::n7));
  ASSERT_EQ(514000, get_ul_arfcn_from_dl_arfcn(538000, nr_band::n7));

  ASSERT_EQ(383000, get_ul_arfcn_from_dl_arfcn(399000, nr_band::n25));

  // n28. The spectrum of this band overlaps with that of n14. We need to check that the return UL ARFCN belongs to n28.
  ASSERT_EQ(142600, get_ul_arfcn_from_dl_arfcn(153600, nr_band::n28));
  ASSERT_EQ(144608, get_ul_arfcn_from_dl_arfcn(155608, nr_band::n28));

  // For n66, n70, n92, n94, the UL spectrum is smaller than the DL spectrum. When we convert the DL ARFCN
  // to the corresponding UL ARFCN, if the UL ARFCN exceeds the band upper-bound, we return 0.
  ASSERT_EQ(356000, get_ul_arfcn_from_dl_arfcn(436000, nr_band::n66));
  ASSERT_EQ(0, get_ul_arfcn_from_dl_arfcn(440000, nr_band::n66));
  ASSERT_EQ(342000, get_ul_arfcn_from_dl_arfcn(402000, nr_band::n70));
  ASSERT_EQ(0, get_ul_arfcn_from_dl_arfcn(404000, nr_band::n70));
  ASSERT_EQ(172400, get_ul_arfcn_from_dl_arfcn(292400, nr_band::n92));
  ASSERT_EQ(0, get_ul_arfcn_from_dl_arfcn(303400, nr_band::n92));
  ASSERT_EQ(183000, get_ul_arfcn_from_dl_arfcn(293400, nr_band::n94));
  ASSERT_EQ(0, get_ul_arfcn_from_dl_arfcn(303400, nr_band::n94));
}

TEST(test_get_abs_freq_point_a_arfcn, mixed_frequencies)
{
  // n3
  ASSERT_EQ(367564, get_abs_freq_point_a_arfcn(52, 368500));
  // n5
  ASSERT_EQ(175364, get_abs_freq_point_a_arfcn(52, 176300));
  // n7 n38
  ASSERT_EQ(528984, get_abs_freq_point_a_arfcn(52, 529920));
  // n78
  ASSERT_EQ(633928, get_abs_freq_point_a_arfcn(52, 634240));
}

TEST(test_arfcn_freq_conversion, arfcn_to_freq_corner_cases)
{
  const uint32_t max_valid_nr_arfcn = 3279165;

  // Max ARFCN is 3279165 at almost 10 GHz
  ASSERT_DOUBLE_EQ(99.99996e9, nr_arfcn_to_freq(max_valid_nr_arfcn));

  // Invalid ARFCN
  ASSERT_DOUBLE_EQ(0.0, nr_arfcn_to_freq(max_valid_nr_arfcn + 1));
}

TEST(test_center_freq_conversion, freq_center)
{
  ASSERT_DOUBLE_EQ(881.5e6, get_center_freq_from_abs_freq_point_a(52, 175364));
  ASSERT_DOUBLE_EQ(836.5e6, get_center_freq_from_abs_freq_point_a(52, 166364));
}

TEST(test_band_duplexing, all_bands)
{
  for (nr_band b : all_nr_bands_fr1) {
    if (b < nr_band::n29 or b == nr_band::n30 or b == nr_band::n65 or b == nr_band::n66 or
        (b >= srsran::nr_band::n70 and b <= srsran::nr_band::n74) or b == nr_band::n85 or
        (b >= srsran::nr_band::n91 and b <= srsran::nr_band::n94) or b == srsran::nr_band::n100) {
      ASSERT_EQ(duplex_mode::FDD, get_duplex_mode(b));
    } else if ((b >= srsran::nr_band::n34 and b <= srsran::nr_band::n53) or
               (b >= srsran::nr_band::n77 and b <= srsran::nr_band::n79) or b == srsran::nr_band::n90 or
               b == srsran::nr_band::n96 or b > srsran::nr_band::n100) {
      ASSERT_EQ(duplex_mode::TDD, get_duplex_mode(b));
    } else if (b == srsran::nr_band::n29 or b == srsran::nr_band::n67 or b == srsran::nr_band::n75 or
               b == srsran::nr_band::n76) {
      ASSERT_EQ(duplex_mode::SDL, get_duplex_mode(b));
    } else if ((b >= srsran::nr_band::n80 and b <= srsran::nr_band::n84) or b == srsran::nr_band::n86 or
               b == srsran::nr_band::n89 or b == srsran::nr_band::n95 or
               (b >= srsran::nr_band::n97 and b <= srsran::nr_band::n99)) {
      ASSERT_EQ(duplex_mode::SUL, get_duplex_mode(b));
    } else {
      ASSERT_FALSE(true);
    }
  }
}

TEST(test_is_paired_spectrum, paired)
{
  const std::array<nr_band, 10> some_fdd_bands{nr_band::n1,
                                               nr_band::n3,
                                               nr_band::n7,
                                               nr_band::n25,
                                               nr_band::n28,
                                               nr_band::n66,
                                               nr_band::n74,
                                               nr_band::n85,
                                               nr_band::n93,
                                               nr_band::n100};

  for (const auto band : some_fdd_bands) {
    ASSERT_TRUE(is_paired_spectrum(band));
  }
}

TEST(test_is_paired_spectrum, unpaired)
{
  const std::array<nr_band, 13> some_non_fdd_bands{nr_band::n29,
                                                   nr_band::n34,
                                                   nr_band::n40,
                                                   nr_band::n46,
                                                   nr_band::n51,
                                                   nr_band::n75,
                                                   nr_band::n76,
                                                   nr_band::n77,
                                                   nr_band::n79,
                                                   nr_band::n81,
                                                   nr_band::n84,
                                                   nr_band::n98,
                                                   nr_band::n102};

  for (const auto band : some_non_fdd_bands) {
    ASSERT_FALSE(is_paired_spectrum(band));
  }
}

TEST(test_ssb_pattern, ssb_pattern_case_A)
{
  const std::array<nr_band, 13> case_c_bands{nr_band::n18,
                                             nr_band::n38,
                                             nr_band::n40,
                                             nr_band::n46,
                                             nr_band::n48,
                                             nr_band::n50,
                                             nr_band::n77,
                                             nr_band::n78,
                                             nr_band::n79,
                                             nr_band::n90,
                                             nr_band::n96,
                                             nr_band::n102,
                                             nr_band::n104};

  for (const auto band : all_nr_bands_fr1) {
    // Skip SSB case-C-only bands and SUL bands.
    if (std::any_of(
            case_c_bands.begin(), case_c_bands.end(), [band](nr_band non_a_band) { return band == non_a_band; }) or
        get_duplex_mode(band) == srsran::duplex_mode::SUL) {
      continue;
    }
    ASSERT_EQ(ssb_pattern_case::A, get_ssb_pattern(band, subcarrier_spacing::kHz15));
  }
}

TEST(test_ssb_pattern, ssb_pattern_case_B)
{
  const std::array<nr_band, 3> case_b_bands{nr_band::n5, nr_band::n24, nr_band::n66};

  for (auto band : case_b_bands) {
    ASSERT_EQ(ssb_pattern_case::B, get_ssb_pattern(band, subcarrier_spacing::kHz30));
  }
}

TEST(test_ssb_pattern, ssb_pattern_case_C)
{
  const std::array<nr_band, 16> case_c_bands{nr_band::n34,
                                             nr_band::n38,
                                             nr_band::n39,
                                             nr_band::n40,
                                             nr_band::n41,
                                             nr_band::n46,
                                             nr_band::n48,
                                             nr_band::n50,
                                             nr_band::n77,
                                             nr_band::n78,
                                             nr_band::n79,
                                             nr_band::n90,
                                             nr_band::n96,
                                             nr_band::n101,
                                             nr_band::n102,
                                             nr_band::n104};
  for (auto band : case_c_bands) {
    ASSERT_EQ(ssb_pattern_case::C, get_ssb_pattern(band, subcarrier_spacing::kHz30));
  }
}

TEST(test_ssb_pattern, ssb_pattern_case_invalid)
{
  ASSERT_GT(ssb_pattern_case::invalid, get_ssb_pattern(nr_band::n1, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::invalid, get_ssb_pattern(nr_band::n3, subcarrier_spacing::kHz30));
}

TEST(test_get_point_a_from_f_req, scs_kHz15)
{
  // Band n1, BWs are {5MHz, 10MHz, 15MHz, 20MHz}.
  ASSERT_DOUBLE_EQ(2122150000.0, get_abs_freq_point_a_from_f_ref(2124400000.0, 25, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2105420000.0, get_abs_freq_point_a_from_f_ref(2110100000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2108090000.0, get_abs_freq_point_a_from_f_ref(2115200000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2150860000.0, get_abs_freq_point_a_from_f_ref(2160400000.0, 106, subcarrier_spacing::kHz15));

  // Band n78, BWs are {10MHz, 15MHz, 20MHz, 30MHz, 40MHz, 50MHz}.
  ASSERT_DOUBLE_EQ(3615675000.0, get_abs_freq_point_a_from_f_ref(3620355000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3613245000.0, get_abs_freq_point_a_from_f_ref(3620355000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3610815000.0, get_abs_freq_point_a_from_f_ref(3620355000.0, 106, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3605955000.0, get_abs_freq_point_a_from_f_ref(3620355000.0, 160, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3600915000.0, get_abs_freq_point_a_from_f_ref(3620355000.0, 216, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3596055000.0, get_abs_freq_point_a_from_f_ref(3620355000.0, 270, subcarrier_spacing::kHz15));
}

TEST(test_get_point_a_from_f_req, scs_kHz30)
{
  // Band n3, BWs are {10MHz, 15MHz, 20MHz, 25MHz, 30MHz}.
  ASSERT_DOUBLE_EQ(1821380000.0, get_abs_freq_point_a_from_f_ref(1825700000.0, 24, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1844460000.0, get_abs_freq_point_a_from_f_ref(1851300000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1860220000.0, get_abs_freq_point_a_from_f_ref(1869400000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1837800000.0, get_abs_freq_point_a_from_f_ref(1849500000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1837800000.0, get_abs_freq_point_a_from_f_ref(1849500000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1842060000.0, get_abs_freq_point_a_from_f_ref(1856100000.0, 78, subcarrier_spacing::kHz30));

  // Band n78, BWs that are tested are {15MHz, 20MHz, 30MHz, 50MHz, 80MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(3697320000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3694980000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3690120000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 78, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3680220000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 133, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3665100000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 217, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3655020000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 273, subcarrier_spacing::kHz30));
}

TEST(test_get_point_a_from_f_req, scs_kHz60)
{
  // Band n41, BWs are {40MHz, 50MHz, 60MHz, 70MHz, 80MHz, 90MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(2671620000.0, get_abs_freq_point_a_from_f_ref(2689980000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2666580000.0, get_abs_freq_point_a_from_f_ref(2689980000.0, 65, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2661540000.0, get_abs_freq_point_a_from_f_ref(2689980000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2656500000.0, get_abs_freq_point_a_from_f_ref(2689980000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2651460000.0, get_abs_freq_point_a_from_f_ref(2689980000.0, 107, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2646420000.0, get_abs_freq_point_a_from_f_ref(2689980000.0, 121, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2641380000.0, get_abs_freq_point_a_from_f_ref(2689980000.0, 135, subcarrier_spacing::kHz60));

  // Band n78, BWs that are tested are {10MHz, 40MHz, 60MHz, 70MHz, 90MHz}.
  ASSERT_DOUBLE_EQ(3700200000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 11, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3685800000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3675720000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3670680000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3660600000.0, get_abs_freq_point_a_from_f_ref(3704160000.0, 121, subcarrier_spacing::kHz60));
}

TEST(test_get_f_req_from_f_req_point_a, scs_kHz15)
{
  // Band n1, BWs are {5MHz, 10MHz, 15MHz, 20MHz}.
  ASSERT_DOUBLE_EQ(2124400000.0, get_f_ref_from_abs_freq_point_a(2122150000.0, 25, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2110100000.0, get_f_ref_from_abs_freq_point_a(2105420000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2115200000.0, get_f_ref_from_abs_freq_point_a(2108090000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2160400000.0, get_f_ref_from_abs_freq_point_a(2150860000.0, 106, subcarrier_spacing::kHz15));

  // Band n78, BWs are {10MHz, 15MHz, 20MHz, 30MHz, 40MHz, 50MHz}.
  ASSERT_DOUBLE_EQ(3620355000.0, get_f_ref_from_abs_freq_point_a(3615675000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0, get_f_ref_from_abs_freq_point_a(3613245000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0, get_f_ref_from_abs_freq_point_a(3610815000.0, 106, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0, get_f_ref_from_abs_freq_point_a(3605955000.0, 160, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0, get_f_ref_from_abs_freq_point_a(3600915000.0, 216, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0, get_f_ref_from_abs_freq_point_a(3596055000.0, 270, subcarrier_spacing::kHz15));
}

TEST(test_get_f_req_from_f_req_point_a, scs_kHz30)
{
  // Band n3, BWs are {10MHz, 15MHz, 20MHz, 25MHz, 30MHz}.
  ASSERT_DOUBLE_EQ(1825700000.0, get_f_ref_from_abs_freq_point_a(1821380000.0, 24, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1851300000.0, get_f_ref_from_abs_freq_point_a(1844460000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1869400000.0, get_f_ref_from_abs_freq_point_a(1860220000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1849500000.0, get_f_ref_from_abs_freq_point_a(1837800000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1849500000.0, get_f_ref_from_abs_freq_point_a(1837800000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1856100000.0, get_f_ref_from_abs_freq_point_a(1842060000.0, 78, subcarrier_spacing::kHz30));

  // Band n78, BWs that are tested are {15MHz, 20MHz, 30MHz, 50MHz, 80MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3697320000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3694980000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3690120000.0, 78, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3680220000.0, 133, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3665100000.0, 217, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3655020000.0, 273, subcarrier_spacing::kHz30));
}

TEST(test_get_f_req_from_f_req_point_a, scs_kHz60)
{
  // Band n41, BWs are {40MHz, 50MHz, 60MHz, 70MHz, 80MHz, 90MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(2689980000.0, get_f_ref_from_abs_freq_point_a(2671620000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0, get_f_ref_from_abs_freq_point_a(2666580000.0, 65, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0, get_f_ref_from_abs_freq_point_a(2661540000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0, get_f_ref_from_abs_freq_point_a(2656500000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0, get_f_ref_from_abs_freq_point_a(2651460000.0, 107, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0, get_f_ref_from_abs_freq_point_a(2646420000.0, 121, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0, get_f_ref_from_abs_freq_point_a(2641380000.0, 135, subcarrier_spacing::kHz60));

  // Band n78, BWs that are tested are {10MHz, 40MHz, 60MHz, 70MHz, 90MHz}.
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3700200000.0, 11, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3685800000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3675720000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3670680000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0, get_f_ref_from_abs_freq_point_a(3660600000.0, 121, subcarrier_spacing::kHz60));
}

TEST(test_get_n_rbs_from_bw, scs_15kHz)
{
  ASSERT_EQ(25, get_n_rbs_from_bw(bs_channel_bandwidth::MHz5, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(52, get_n_rbs_from_bw(bs_channel_bandwidth::MHz10, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(79, get_n_rbs_from_bw(bs_channel_bandwidth::MHz15, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(106, get_n_rbs_from_bw(bs_channel_bandwidth::MHz20, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(133, get_n_rbs_from_bw(bs_channel_bandwidth::MHz25, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(160, get_n_rbs_from_bw(bs_channel_bandwidth::MHz30, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(188, get_n_rbs_from_bw(bs_channel_bandwidth::MHz35, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(216, get_n_rbs_from_bw(bs_channel_bandwidth::MHz40, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(242, get_n_rbs_from_bw(bs_channel_bandwidth::MHz45, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(270, get_n_rbs_from_bw(bs_channel_bandwidth::MHz50, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(0, get_n_rbs_from_bw(bs_channel_bandwidth::MHz60, subcarrier_spacing::kHz15, frequency_range::FR1));
}

TEST(test_get_n_rbs_from_bw, scs_30kHz)
{
  ASSERT_EQ(11, get_n_rbs_from_bw(bs_channel_bandwidth::MHz5, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(24, get_n_rbs_from_bw(bs_channel_bandwidth::MHz10, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(38, get_n_rbs_from_bw(bs_channel_bandwidth::MHz15, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(51, get_n_rbs_from_bw(bs_channel_bandwidth::MHz20, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(65, get_n_rbs_from_bw(bs_channel_bandwidth::MHz25, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(78, get_n_rbs_from_bw(bs_channel_bandwidth::MHz30, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(92, get_n_rbs_from_bw(bs_channel_bandwidth::MHz35, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(106, get_n_rbs_from_bw(bs_channel_bandwidth::MHz40, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(119, get_n_rbs_from_bw(bs_channel_bandwidth::MHz45, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(133, get_n_rbs_from_bw(bs_channel_bandwidth::MHz50, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(162, get_n_rbs_from_bw(bs_channel_bandwidth::MHz60, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(189, get_n_rbs_from_bw(bs_channel_bandwidth::MHz70, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(217, get_n_rbs_from_bw(bs_channel_bandwidth::MHz80, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(245, get_n_rbs_from_bw(bs_channel_bandwidth::MHz90, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(273, get_n_rbs_from_bw(bs_channel_bandwidth::MHz100, subcarrier_spacing::kHz30, frequency_range::FR1));
}

TEST(test_get_n_rbs_from_bw, scs_60kHz)
{
  ASSERT_EQ(0, get_n_rbs_from_bw(bs_channel_bandwidth::MHz5, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(11, get_n_rbs_from_bw(bs_channel_bandwidth::MHz10, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(18, get_n_rbs_from_bw(bs_channel_bandwidth::MHz15, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(24, get_n_rbs_from_bw(bs_channel_bandwidth::MHz20, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(31, get_n_rbs_from_bw(bs_channel_bandwidth::MHz25, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(38, get_n_rbs_from_bw(bs_channel_bandwidth::MHz30, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(44, get_n_rbs_from_bw(bs_channel_bandwidth::MHz35, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(51, get_n_rbs_from_bw(bs_channel_bandwidth::MHz40, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(58, get_n_rbs_from_bw(bs_channel_bandwidth::MHz45, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(65, get_n_rbs_from_bw(bs_channel_bandwidth::MHz50, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(79, get_n_rbs_from_bw(bs_channel_bandwidth::MHz60, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(93, get_n_rbs_from_bw(bs_channel_bandwidth::MHz70, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(107, get_n_rbs_from_bw(bs_channel_bandwidth::MHz80, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(121, get_n_rbs_from_bw(bs_channel_bandwidth::MHz90, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(135, get_n_rbs_from_bw(bs_channel_bandwidth::MHz100, subcarrier_spacing::kHz60, frequency_range::FR1));
}

TEST(test_get_n_rbs_from_bw, scs_120kHz_fr2)
{
  ASSERT_EQ(66, get_n_rbs_from_bw(bs_channel_bandwidth::MHz50, subcarrier_spacing::kHz60, frequency_range::FR2));
  ASSERT_EQ(132, get_n_rbs_from_bw(bs_channel_bandwidth::MHz100, subcarrier_spacing::kHz60, frequency_range::FR2));
  ASSERT_EQ(264, get_n_rbs_from_bw(bs_channel_bandwidth::MHz200, subcarrier_spacing::kHz60, frequency_range::FR2));
}

TEST(test_get_n_rbs_from_bw, scs_120kHz)
{
  ASSERT_EQ(32, get_n_rbs_from_bw(bs_channel_bandwidth::MHz50, subcarrier_spacing::kHz120, frequency_range::FR2));
  ASSERT_EQ(66, get_n_rbs_from_bw(bs_channel_bandwidth::MHz100, subcarrier_spacing::kHz120, frequency_range::FR2));
  ASSERT_EQ(132, get_n_rbs_from_bw(bs_channel_bandwidth::MHz200, subcarrier_spacing::kHz120, frequency_range::FR2));
  ASSERT_EQ(264, get_n_rbs_from_bw(bs_channel_bandwidth::MHz400, subcarrier_spacing::kHz120, frequency_range::FR2));
}

TEST(test_get_n_rbs_from_bw, invalid_cases)
{
  ASSERT_EQ(0, get_n_rbs_from_bw(bs_channel_bandwidth::MHz200, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(0, get_n_rbs_from_bw(bs_channel_bandwidth::MHz400, subcarrier_spacing::kHz60, frequency_range::FR2));
  ASSERT_EQ(0, get_n_rbs_from_bw(bs_channel_bandwidth::MHz400, subcarrier_spacing::kHz15, frequency_range::FR2));
  ASSERT_EQ(0, get_n_rbs_from_bw(bs_channel_bandwidth::MHz400, subcarrier_spacing::kHz240, frequency_range::FR2));
  ASSERT_EQ(0, get_n_rbs_from_bw(bs_channel_bandwidth::MHz100, subcarrier_spacing::kHz120, frequency_range::FR1));
}

TEST(test_get_min_channel_bw, invalid_cases)
{
  ASSERT_EQ(min_channel_bandwidth::MHz5, get_min_channel_bw(nr_band::n1, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n1, subcarrier_spacing::kHz30));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n1, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n1, subcarrier_spacing::kHz120));

  ASSERT_EQ(min_channel_bandwidth::MHz5, get_min_channel_bw(nr_band::n3, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n3, subcarrier_spacing::kHz30));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n3, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n3, subcarrier_spacing::kHz120));

  ASSERT_EQ(min_channel_bandwidth::MHz5, get_min_channel_bw(nr_band::n7, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n7, subcarrier_spacing::kHz30));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n7, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n7, subcarrier_spacing::kHz120));

  ASSERT_EQ(min_channel_bandwidth::MHz5, get_min_channel_bw(nr_band::n20, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n20, subcarrier_spacing::kHz30));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n20, subcarrier_spacing::kHz60));

  ASSERT_EQ(min_channel_bandwidth::MHz5, get_min_channel_bw(nr_band::n76, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n76, subcarrier_spacing::kHz30));

  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n78, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n78, subcarrier_spacing::kHz30));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n78, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n78, subcarrier_spacing::kHz120));

  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n79, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n79, subcarrier_spacing::kHz30));
  ASSERT_EQ(min_channel_bandwidth::MHz10, get_min_channel_bw(nr_band::n79, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n79, subcarrier_spacing::kHz120));

  ASSERT_EQ(min_channel_bandwidth::MHz20, get_min_channel_bw(nr_band::n102, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::MHz20, get_min_channel_bw(nr_band::n102, subcarrier_spacing::kHz30));
  ASSERT_EQ(min_channel_bandwidth::MHz20, get_min_channel_bw(nr_band::n102, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n102, subcarrier_spacing::kHz120));

  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n257, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n258, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n260, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n261, subcarrier_spacing::kHz60));
  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n257, subcarrier_spacing::kHz120));
  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n258, subcarrier_spacing::kHz120));
  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n260, subcarrier_spacing::kHz120));
  ASSERT_EQ(min_channel_bandwidth::MHz50, get_min_channel_bw(nr_band::n261, subcarrier_spacing::kHz120));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n257, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n258, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n260, subcarrier_spacing::kHz15));
  ASSERT_EQ(min_channel_bandwidth::invalid, get_min_channel_bw(nr_band::n261, subcarrier_spacing::kHz15));
}

TEST(test_is_valid_ssb_arfcn, mixed_bands)
{
  // ARFCN 427970 is a valid SSB ARFCN for n1, expect no error.
  ASSERT_TRUE(is_ssb_arfcn_valid_given_band(427970U, nr_band::n1, subcarrier_spacing::kHz15).has_value());
  // ARFCN 433970 is NOT a valid SSB ARFCN for n1, expect an error.
  ASSERT_FALSE(is_ssb_arfcn_valid_given_band(433970U, nr_band::n1, subcarrier_spacing::kHz15).has_value());

  // ARFCN 427970 is a valid SSB ARFCN for n1, expect no error.
  ASSERT_TRUE(is_ssb_arfcn_valid_given_band(755712U, nr_band::n46, subcarrier_spacing::kHz30).has_value());
  // ARFCN 433970 is NOT a valid SSB ARFCN for n1, expect an error.
  ASSERT_FALSE(is_ssb_arfcn_valid_given_band(785856U, nr_band::n46, subcarrier_spacing::kHz30).has_value());
}

TEST(test_get_ssb_l_max, test_different_ssb_cases)
{
  // Case A.
  ASSERT_EQ(4, get_ssb_l_max(nr_band::n3, subcarrier_spacing::kHz15, 363640U));
  // Case B.
  ASSERT_EQ(8, get_ssb_l_max(nr_band::n5, subcarrier_spacing::kHz30, 176800U));
  // Case C, f < f_cutoff.
  ASSERT_EQ(4, get_ssb_l_max(nr_band::n50, subcarrier_spacing::kHz30, 296400U));
  // Case C, f > f_cutoff.
  ASSERT_EQ(8, get_ssb_l_max(nr_band::n104, subcarrier_spacing::kHz30, 860000U));
}

TEST(test_dl_arfcn_validator, test_different_fdd_bands)
{
  // Get a random raster point from the band raster. If outside_raster is true, the function returns a point outside the
  // raster.
  auto get_rnd_raster_point = [](span<const uint32_t> raster, bool outside_raster = false) -> uint32_t {
    const uint32_t raster_step = raster[1];
    if (outside_raster) {
      return test_rgen::bernoulli(0.5) ? raster.front() - raster_step : raster.back() + raster_step;
    }
    const unsigned nof_raster_points = (raster.back() - raster.front()) / raster_step + 1;
    return test_rgen::uniform_int<unsigned>(0, nof_raster_points - 1) * raster_step + raster.front();
  };

  // Definition of UL rasters for different bands.
  constexpr std::array<uint32_t, 3> n1_raster  = {422000, 20, 434000};
  constexpr std::array<uint32_t, 3> n2_raster  = {386000, 20, 398000};
  constexpr std::array<uint32_t, 3> n3_raster  = {361000, 20, 376000};
  constexpr std::array<uint32_t, 3> n5_raster  = {173800, 20, 178800};
  constexpr std::array<uint32_t, 3> n7_raster  = {524000, 20, 538000};
  constexpr std::array<uint32_t, 3> n14_raster = {151600, 20, 153600};
  constexpr std::array<uint32_t, 3> n28_raster = {151600, 20, 160600};

  // FDD bands
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n1, get_rnd_raster_point(n1_raster), subcarrier_spacing::kHz15)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n1, get_rnd_raster_point(n1_raster, true), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n2, get_rnd_raster_point(n2_raster), subcarrier_spacing::kHz15)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n2, get_rnd_raster_point(n2_raster, true), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n3, get_rnd_raster_point(n3_raster), subcarrier_spacing::kHz15)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n3, get_rnd_raster_point(n3_raster, true), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n5, get_rnd_raster_point(n5_raster), subcarrier_spacing::kHz15)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n5, get_rnd_raster_point(n5_raster, true), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n7, get_rnd_raster_point(n7_raster), subcarrier_spacing::kHz15)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n7, get_rnd_raster_point(n7_raster, true), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n14, get_rnd_raster_point(n14_raster), subcarrier_spacing::kHz15)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n14, get_rnd_raster_point(n14_raster, true), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n28, get_rnd_raster_point(n28_raster), subcarrier_spacing::kHz15)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n28, get_rnd_raster_point(n28_raster, true), subcarrier_spacing::kHz15)
          .has_value());
  // Single UL ARFCN for n28, BW 40Mhz.
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n28, 155608, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz40)
          .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n28, 155608, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz20)
          .has_value());
}

TEST(test_dl_arfcn_validator, test_different_tdd_bands_with_regular_raster)
{
  // Get a random raster point from the band raster. If outside_raster is true, the function returns a point outside the
  // raster.
  auto get_rnd_raster_point = [](span<const uint32_t> raster, bool outside_raster = false) -> uint32_t {
    const uint32_t raster_step = raster[1];
    if (outside_raster) {
      return test_rgen::bernoulli(0.5) ? raster.front() - raster_step : raster.back() + raster_step;
    }
    const unsigned nof_raster_points = (raster.back() - raster.front()) / raster_step + 1;
    return test_rgen::uniform_int<unsigned>(0, nof_raster_points - 1) * raster_step + raster.front();
  };

  // Definition of UL rasters for different bands.
  constexpr std::array<uint32_t, 3> n34_raster  = {402000, 20, 405000};
  constexpr std::array<uint32_t, 3> n38_raster  = {514000, 20, 524000};
  constexpr std::array<uint32_t, 3> n39_raster  = {376000, 20, 384000};
  constexpr std::array<uint32_t, 3> n40_raster  = {460000, 20, 480000};
  constexpr std::array<uint32_t, 3> n50_raster  = {286400, 20, 303400};
  constexpr std::array<uint32_t, 3> n51_raster  = {285400, 20, 286400};
  constexpr std::array<uint32_t, 3> n53_raster  = {496700, 20, 499000};
  constexpr std::array<uint32_t, 3> n100_raster = {183880, 20, 185000};

  // TDD bands.
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n34, get_rnd_raster_point(n34_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n34, get_rnd_raster_point(n34_raster, true), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n38, get_rnd_raster_point(n38_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n38, get_rnd_raster_point(n38_raster, true), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n39, get_rnd_raster_point(n39_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n39, get_rnd_raster_point(n39_raster, true), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n40, get_rnd_raster_point(n40_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n40, get_rnd_raster_point(n40_raster, true), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n50, get_rnd_raster_point(n50_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n50, get_rnd_raster_point(n50_raster, true), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n51, get_rnd_raster_point(n51_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n51, get_rnd_raster_point(n51_raster, true), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n53, get_rnd_raster_point(n53_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n53, get_rnd_raster_point(n53_raster, true), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(nr_band::n100, get_rnd_raster_point(n100_raster), subcarrier_spacing::kHz30)
                  .has_value());
  ASSERT_FALSE(
      is_dl_arfcn_valid_given_band(nr_band::n100, get_rnd_raster_point(n100_raster, true), subcarrier_spacing::kHz30)
          .has_value());
}

TEST(test_dl_arfcn_validator, test_tdd_bands_with_sparse_raster)
{
  // Band n46.
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n46, 788668U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz10)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n46, 769332U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz10)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n46, 774668U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz20)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n46, 782000U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz20)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n46, 770000U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz40)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n46, 755334U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz40)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n46, 778668U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz60)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n46, 746000U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz60)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n46, 746000U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz80)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n46, 791000U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz80)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n46, 791000U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz100)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n46, 782000U, subcarrier_spacing::kHz15, srsran::bs_channel_bandwidth::MHz100)
                   .has_value());

  // Band n96.
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n96, 838332U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz20)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n96, 827000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz20)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n96, 821668U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz40)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n96, 857000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz40)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n96, 873000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz60)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n96, 859000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz60)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n96, 815000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz80)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n96, 798332U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz80)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n96, 870332U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz100)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n96, 841668U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz100)
                   .has_value());

  // Band n102.
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n102, 805000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz20)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n102, 797668U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz20)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n102, 808332U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz40)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n102, 805000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz40)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n102, 803668U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz60)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n102, 800332U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz60)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n102, 825668U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz80)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n102, 852000U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz80)
                   .has_value());
  ASSERT_TRUE(is_dl_arfcn_valid_given_band(
                  nr_band::n102, 814332U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz100)
                  .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n102, 798332U, subcarrier_spacing::kHz30, srsran::bs_channel_bandwidth::MHz100)
                   .has_value());
}

TEST(test_dl_arfcn_validator, test_tdd_bands_with_scs_dependent_raster)
{
  // Get a random raster point from the band raster. If outside_raster is true, the function returns a point outside the
  // raster.
  auto get_rnd_raster_point = [](span<const uint32_t> raster, bool outside_raster = false) -> uint32_t {
    const uint32_t raster_step = raster[1];
    if (outside_raster) {
      return test_rgen::bernoulli(0.5) ? raster.front() - raster_step : raster.back() + raster_step;
    }
    const unsigned nof_raster_points = (raster.back() - raster.front()) / raster_step + 1;
    return test_rgen::uniform_int<unsigned>(0, nof_raster_points - 1) * raster_step + raster.front();
  };

  // Definition of UL rasters for different bands.
  constexpr std::array<uint32_t, 3> n41_raster_scs_15  = {499200, 3, 537999};
  constexpr std::array<uint32_t, 3> n41_raster_scs_30  = {499200, 6, 537996};
  constexpr std::array<uint32_t, 3> n48_raster_scs_15  = {636667, 1, 646666};
  constexpr std::array<uint32_t, 3> n48_raster_scs_30  = {636668, 2, 646666};
  constexpr std::array<uint32_t, 3> n77_raster_scs_15  = {620000, 1, 680000};
  constexpr std::array<uint32_t, 3> n77_raster_scs_30  = {620000, 2, 680000};
  constexpr std::array<uint32_t, 3> n78_raster_scs_15  = {620000, 1, 653333};
  constexpr std::array<uint32_t, 3> n78_raster_scs_30  = {620000, 2, 653332};
  constexpr std::array<uint32_t, 3> n79_raster_scs_15  = {693334, 1, 733333};
  constexpr std::array<uint32_t, 3> n79_raster_scs_30  = {693334, 2, 733332};
  constexpr std::array<uint32_t, 3> n90_raster_scs_15  = {499200, 3, 537999};
  constexpr std::array<uint32_t, 3> n90_raster_scs_30  = {499200, 6, 537996};
  constexpr std::array<uint32_t, 3> n104_raster_scs_15 = {828334, 1, 875000};
  constexpr std::array<uint32_t, 3> n104_raster_scs_30 = {828334, 2, 875000};

  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n41, get_rnd_raster_point(n41_raster_scs_15), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n41, get_rnd_raster_point(n41_raster_scs_15, true), subcarrier_spacing::kHz15)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n41, get_rnd_raster_point(n41_raster_scs_30), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n41, get_rnd_raster_point(n41_raster_scs_30, true), subcarrier_spacing::kHz30)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n48, get_rnd_raster_point(n48_raster_scs_15), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n48, get_rnd_raster_point(n48_raster_scs_15, true), subcarrier_spacing::kHz15)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n48, get_rnd_raster_point(n48_raster_scs_30), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n48, get_rnd_raster_point(n48_raster_scs_30, true), subcarrier_spacing::kHz30)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n77, get_rnd_raster_point(n77_raster_scs_15), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n77, get_rnd_raster_point(n77_raster_scs_15, true), subcarrier_spacing::kHz15)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n77, get_rnd_raster_point(n77_raster_scs_30), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n77, get_rnd_raster_point(n77_raster_scs_30, true), subcarrier_spacing::kHz30)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n78, get_rnd_raster_point(n78_raster_scs_15), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n78, get_rnd_raster_point(n78_raster_scs_15, true), subcarrier_spacing::kHz15)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n78, get_rnd_raster_point(n78_raster_scs_30), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n78, get_rnd_raster_point(n78_raster_scs_30, true), subcarrier_spacing::kHz30)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n79, get_rnd_raster_point(n79_raster_scs_15), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n79, get_rnd_raster_point(n79_raster_scs_15, true), subcarrier_spacing::kHz15)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n79, get_rnd_raster_point(n79_raster_scs_30), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n79, get_rnd_raster_point(n79_raster_scs_30, true), subcarrier_spacing::kHz30)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n90, get_rnd_raster_point(n90_raster_scs_15), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n90, get_rnd_raster_point(n90_raster_scs_15, true), subcarrier_spacing::kHz15)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n90, get_rnd_raster_point(n90_raster_scs_30), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n90, get_rnd_raster_point(n90_raster_scs_30, true), subcarrier_spacing::kHz30)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n104, get_rnd_raster_point(n104_raster_scs_15), subcarrier_spacing::kHz15)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n104, get_rnd_raster_point(n104_raster_scs_15, true), subcarrier_spacing::kHz15)
                   .has_value());
  ASSERT_TRUE(
      is_dl_arfcn_valid_given_band(nr_band::n104, get_rnd_raster_point(n104_raster_scs_30), subcarrier_spacing::kHz30)
          .has_value());
  ASSERT_FALSE(is_dl_arfcn_valid_given_band(
                   nr_band::n104, get_rnd_raster_point(n104_raster_scs_30, true), subcarrier_spacing::kHz30)
                   .has_value());
}

TEST(test_ul_arfcn_validator, test_different_bands)
{
  // Get a random raster point from the band raster. If outside_raster is true, the function returns a point outside the
  // raster.
  auto get_rnd_raster_point = [](span<const uint32_t> raster, bool outside_raster = false) -> uint32_t {
    const uint32_t raster_step = raster[1];
    if (outside_raster) {
      return test_rgen::bernoulli(0.5) ? raster.front() - raster_step : raster.back() + raster_step;
    }
    const unsigned nof_raster_points = (raster.back() - raster.front()) / raster_step + 1;
    return test_rgen::uniform_int<unsigned>(0, nof_raster_points - 1) * raster_step + raster.front();
  };

  // Definition of UL rasters for different bands.
  constexpr std::array<uint32_t, 3> n2_raster  = {370000, 20, 382000};
  constexpr std::array<uint32_t, 3> n3_raster  = {342000, 20, 357000};
  constexpr std::array<uint32_t, 3> n7_raster  = {500000, 20, 514000};
  constexpr std::array<uint32_t, 3> n14_raster = {157600, 20, 159600};
  constexpr std::array<uint32_t, 3> n28_raster = {140600, 20, 149600};

  ASSERT_TRUE(is_ul_arfcn_valid_given_band(nr_band::n2, get_rnd_raster_point(n2_raster)).has_value());
  ASSERT_FALSE(is_ul_arfcn_valid_given_band(nr_band::n2, get_rnd_raster_point(n2_raster, true)).has_value());
  ASSERT_TRUE(is_ul_arfcn_valid_given_band(nr_band::n3, get_rnd_raster_point(n3_raster)).has_value());
  ASSERT_FALSE(is_ul_arfcn_valid_given_band(nr_band::n3, get_rnd_raster_point(n3_raster, true)).has_value());
  ASSERT_TRUE(is_ul_arfcn_valid_given_band(nr_band::n7, get_rnd_raster_point(n7_raster)).has_value());
  ASSERT_FALSE(is_ul_arfcn_valid_given_band(nr_band::n7, get_rnd_raster_point(n7_raster, true)).has_value());
  ASSERT_TRUE(is_ul_arfcn_valid_given_band(nr_band::n14, get_rnd_raster_point(n14_raster)).has_value());
  ASSERT_FALSE(is_ul_arfcn_valid_given_band(nr_band::n14, get_rnd_raster_point(n14_raster, true)).has_value());
  ASSERT_TRUE(is_ul_arfcn_valid_given_band(nr_band::n28, get_rnd_raster_point(n28_raster)).has_value());
  ASSERT_FALSE(is_ul_arfcn_valid_given_band(nr_band::n28, get_rnd_raster_point(n28_raster, true)).has_value());
  // Single UL ARFCN for n28, BW 40Mhz.
  ASSERT_TRUE(is_ul_arfcn_valid_given_band(nr_band::n28, 144608, bs_channel_bandwidth::MHz40).has_value());
  // Let's provide a n14 frequency to n28.
  ASSERT_FALSE(is_ul_arfcn_valid_given_band(nr_band::n28, 157600).has_value());
}
