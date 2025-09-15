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

#include "srsran/ran/band_helper.h"
#include "ssb/ssb_freq_position_generator.h"
#include "srsran/adt/interval.h"
#include "srsran/adt/span.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsran/ran/ssb/ssb_gscn.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/std.h"

using namespace srsran;

using namespace band_helper;

namespace {

/// Possible values of delta f_raster in Table 5.4.2.3-1 and Table 5.4.2.3-2
enum class delta_freq_raster {
  /// For bands with 2 possible values for delta_f_raster (e.g. 15 and 30 kHz), the lowest is chosen.
  DEFAULT = 0,
  kHz15,
  kHz30,
  kHz60,
  kHz100,
  kHz120
};

/// NR operating band and DL ARFCN lower-bound and upper-bound. See Table 5.4.2.3-1 in TS 38.104.
struct nr_band_raster {
  nr_band           band;
  delta_freq_raster delta_f_rast;
  uint32_t          ul_nref_first;
  uint32_t          ul_nref_step;
  uint32_t          ul_nref_last;
  uint32_t          dl_nref_first;
  uint32_t          dl_nref_step;
  uint32_t          dl_nref_last;
};

} // namespace

/// From Tables 5.4.2.3-1 and 5.4.2.3-2 in TS 38.104, table with NR operating FR1 and FR2 band and related ARFCN
/// lower-bound and upper-bound. (FDD, TDD or SDL).
/// NTN bands from Table 5.4.2.3-1 in TS 38.108
///
/// NOTE: It only includes FDD, TDD, and SDL bands.
/// NOTE: Band 2 is a subset of band 25.
/// NOTE: Band 41 has two different Freq raster, we only consider raster 15kHz.
/// NOTE: FR2 bands have two different Freq raster, we only consider raster 120kHz.
static constexpr unsigned                                    nof_nr_DL_bands = 84;
static constexpr std::array<nr_band_raster, nof_nr_DL_bands> nr_band_table   = {{
    // clang-format off
    {nr_band::n1,    delta_freq_raster::kHz100, 384000, 20,  396000,  422000, 20,  434000},
    {nr_band::n2,    delta_freq_raster::kHz100, 370000, 20,  382000,  386000, 20,  398000},
    {nr_band::n3,    delta_freq_raster::kHz100, 342000, 20,  357000,  361000, 20,  376000},
    {nr_band::n5,    delta_freq_raster::kHz100, 164800, 20,  169800,  173800, 20,  178800},
    {nr_band::n7,    delta_freq_raster::kHz100, 500000, 20,  514000,  524000, 20,  538000},
    {nr_band::n8,    delta_freq_raster::kHz100, 176000, 20,  183000,  185000, 20,  192000},
    {nr_band::n12,   delta_freq_raster::kHz100, 139800, 20,  143200,  145800, 20,  149200},
    {nr_band::n13,   delta_freq_raster::kHz100, 155400, 20,  157400,  149200, 20,  151200},
    {nr_band::n14,   delta_freq_raster::kHz100, 157600, 20,  159600,  151600, 20,  153600},
    {nr_band::n18,   delta_freq_raster::kHz100, 163000, 20,  166000,  172000, 20,  175000},
    {nr_band::n20,   delta_freq_raster::kHz100, 166400, 20,  172400,  158200, 20,  164200},
    {nr_band::n25,   delta_freq_raster::kHz100, 370000, 20,  383000,  386000, 20,  399000},
    {nr_band::n24,   delta_freq_raster::kHz100, 325300, 20,  332100,  305000, 20,  311800},
    {nr_band::n26,   delta_freq_raster::kHz100, 162800, 20,  169800,  171800, 20,  178800},
    {nr_band::n28,   delta_freq_raster::kHz100, 140600, 20,  149600,  151600, 20,  160600},
    {nr_band::n28,   delta_freq_raster::kHz100, 144608,  0,  144608,  155608,  0,  155608},
    {nr_band::n29,   delta_freq_raster::kHz100,      0,  0,       0,  143400, 20,  145600},
    {nr_band::n30,   delta_freq_raster::kHz100, 461000, 20,  463000,  470000, 20,  472000},
    {nr_band::n34,   delta_freq_raster::kHz100, 402000, 20,  405000,  402000, 20,  405000},
    {nr_band::n38,   delta_freq_raster::kHz100, 514000, 20,  524000,  514000, 20,  524000},
    {nr_band::n39,   delta_freq_raster::kHz100, 376000, 20,  384000,  376000, 20,  384000},
    {nr_band::n40,   delta_freq_raster::kHz100, 460000, 20,  480000,  460000, 20,  480000},
    {nr_band::n41,   delta_freq_raster::kHz15,  499200,  3,  537999,  499200,  3,  537999},
    {nr_band::n41,   delta_freq_raster::kHz30,  499200,  6,  537996,  499200,  6,  537996},
    // Band n46 has a raster definded in Table 5.4.2.3-1, TS 38.104, but according to the table notes, only a few ARFCN
    // values are valid. This range is reported below for completeness, but only the applicable ARFCN given in the
    // notes will be used.
    {nr_band::n46,   delta_freq_raster::kHz15,  743334,  1,  795000,  743334,  1,  795000},
    {nr_band::n48,   delta_freq_raster::kHz15,  636667,  1,  646666,  636667,  1,  646666},
    {nr_band::n48,   delta_freq_raster::kHz30,  636668,  2,  646666,  636668,  2,  646666},
    {nr_band::n50,   delta_freq_raster::kHz100, 286400, 20,  303400,  286400, 20,  303400},
    {nr_band::n51,   delta_freq_raster::kHz100, 285400, 20,  286400,  285400, 20,  286400},
    {nr_band::n53,   delta_freq_raster::kHz100, 496700, 20,  499000,  496700, 20,  499000},
    {nr_band::n65,   delta_freq_raster::kHz100, 384000, 20,  402000,  422000, 20,  440000},
    {nr_band::n66,   delta_freq_raster::kHz100, 342000, 20,  356000,  422000, 20,  440000},
    {nr_band::n67,   delta_freq_raster::kHz100,      0,  0,       0,  147600, 20,  151600},
    {nr_band::n70,   delta_freq_raster::kHz100, 339000, 20,  342000,  399000, 20,  404000},
    {nr_band::n71,   delta_freq_raster::kHz100, 132600, 20,  139600,  123400, 20,  130400},
    {nr_band::n74,   delta_freq_raster::kHz100, 285400, 20,  294000,  295000, 20,  303600},
    {nr_band::n75,   delta_freq_raster::kHz100,      0,  0,       0,  286400, 20,  303400},
    {nr_band::n76,   delta_freq_raster::kHz100,      0,  0,       0,  285400, 20,  286400},
    {nr_band::n77,   delta_freq_raster::kHz15,  620000,  1,  680000,  620000,  1,  680000},
    {nr_band::n77,   delta_freq_raster::kHz30,  620000,  2,  680000,  620000,  2,  680000},
    {nr_band::n78,   delta_freq_raster::kHz15,  620000,  1,  653333,  620000,  1,  653333},
    {nr_band::n78,   delta_freq_raster::kHz30,  620000,  2,  653332,  620000,  2,  653332},
    {nr_band::n79,   delta_freq_raster::kHz15,  693334,  1,  733333,  693334,  1,  733333},
    {nr_band::n79,   delta_freq_raster::kHz30,  693334,  2,  733332,  693334,  2,  733332},
    {nr_band::n80,   delta_freq_raster::kHz100, 342000, 20,  357000,       0,  0,       0},
    {nr_band::n81,   delta_freq_raster::kHz100, 176000, 20,  183000,       0,  0,       0},
    {nr_band::n82,   delta_freq_raster::kHz100, 166400, 20,  172400,       0,  0,       0},
    {nr_band::n83,   delta_freq_raster::kHz100, 140600, 20,  149600,       0,  0,       0},
    {nr_band::n84,   delta_freq_raster::kHz100, 384000, 20,  396000,       0,  0,       0},
    {nr_band::n85,   delta_freq_raster::kHz100, 139600, 20,  143200,  145600, 20,  149200},
    {nr_band::n86,   delta_freq_raster::kHz100, 342000, 20,  356000,       0,  0,       0},
    {nr_band::n89,   delta_freq_raster::kHz100, 164800, 20,  169800,       0,  0,       0},
    {nr_band::n90,   delta_freq_raster::kHz15,  499200,  3,  537999,  499200,  3,  537999},
    {nr_band::n90,   delta_freq_raster::kHz30,  499200,  6,  537996,  499200,  6,  537996},
    {nr_band::n90,   delta_freq_raster::kHz100, 499200, 20,  538000,  499200, 20,  538000},
    {nr_band::n91,   delta_freq_raster::kHz100, 166400, 20,  172400,  285400, 20,  286400},
    {nr_band::n92,   delta_freq_raster::kHz100, 166400, 20,  172400,  286400, 20,  303400},
    {nr_band::n93,   delta_freq_raster::kHz100, 176000, 20,  183000,  285400, 20,  286400},
    {nr_band::n94,   delta_freq_raster::kHz100, 176000, 20,  183000,  286400, 20,  303400},
    {nr_band::n95,   delta_freq_raster::kHz100, 402000, 20,  405000,       0,  0,       0},
    // Band n96 has a raster definded in Table 5.4.2.3-1, TS 38.104, but according to the table notes, only a few ARFCN
    // values are valid. This range is reported below for completeness, but only the applicable ARFCN given in the
    // notes will be used.
    {nr_band::n96,   delta_freq_raster::kHz15,  795000,  1,  875000,  795000,  1,  875000},
    {nr_band::n97,   delta_freq_raster::kHz100, 460000, 20,  480000,       0,  0,       0},
    {nr_band::n98,   delta_freq_raster::kHz100, 376000, 20,  384000,       0,  0,       0},
    {nr_band::n99,   delta_freq_raster::kHz100, 325300, 20,  332100,       0,  0,       0},
    {nr_band::n100,  delta_freq_raster::kHz100, 174880, 20,  176000,  183880, 20,  185000},
    {nr_band::n101,  delta_freq_raster::kHz100, 380000, 20,  382000,  380000, 20,  382000},
    // Band n102 has a raster definded in Table 5.4.2.3-1, TS 38.104, but according to the table notes, only a few
    // ARFCN values are valid. This range is reported below for completeness, but only the applicable ARFCN given in
    // the notes will be used.
    {nr_band::n102,  delta_freq_raster::kHz15,  796334, 1 ,  828333,  796334,  1,  828333},
    {nr_band::n104,  delta_freq_raster::kHz15,  828334, 1 ,  875000,  828334,  1,  875000},
    {nr_band::n104,  delta_freq_raster::kHz30,  828334, 2 ,  875000,  828334,  2,  875000},
    {nr_band::n254,  delta_freq_raster::kHz100, 322000, 20,  325300,  496700, 20,  500000},
    {nr_band::n255,  delta_freq_raster::kHz100, 325300, 20,  332100,  305000, 20,  311800},
    {nr_band::n256,  delta_freq_raster::kHz100, 396000, 20,  402000,  434000, 20,  440000},
    {nr_band::n257,  delta_freq_raster::kHz60,  2054166, 1, 2104165, 2054166,  1, 2104165},
    {nr_band::n257,  delta_freq_raster::kHz120, 2054167, 2, 2104165, 2054167,  2, 2104165},
    {nr_band::n258,  delta_freq_raster::kHz60,  2016667, 1, 2070832, 2016667,  1, 2070832},
    {nr_band::n258,  delta_freq_raster::kHz120, 2016667, 2, 2070831, 2016667,  2, 2070831},
    {nr_band::n259,  delta_freq_raster::kHz60,  2270833, 1, 2337499, 2270833,  1, 2337499},
    {nr_band::n259,  delta_freq_raster::kHz120, 2270833, 2, 2337499, 2270833,  2, 2337499},
    {nr_band::n260,  delta_freq_raster::kHz60,  2229166, 1, 2279165, 2229166,  1, 2279165},
    {nr_band::n260,  delta_freq_raster::kHz120, 2229167, 2, 2279165, 2229167,  2, 2279165},
    {nr_band::n261,  delta_freq_raster::kHz60,  2070833, 1, 2084999, 2070833,  1, 2084999},
    {nr_band::n261,  delta_freq_raster::kHz120, 2070833, 2, 2084999, 2070833,  2, 2084999},
    {nr_band::n262,  delta_freq_raster::kHz60,  2399166, 1, 2415832, 2399166,  1, 2415832},
    {nr_band::n262,  delta_freq_raster::kHz120, 2399167, 2, 2415831, 2399167,  2, 2415831},
    // clang-format on
}};

namespace {

/// NR operating band with related Duplex Mode. See TS 38.101-1 Table 5.2-1 for FR1 and Table 5.2-2 for FR2.
struct nr_operating_band {
  nr_band     band;
  duplex_mode duplex;
};

} // namespace

static constexpr unsigned                                             nof_nr_operating_band = 68;
static constexpr std::array<nr_operating_band, nof_nr_operating_band> nr_operating_bands    = {{
    // clang-format off
    {nr_band::n1,  duplex_mode::FDD},
    {nr_band::n2,  duplex_mode::FDD},
    {nr_band::n3,  duplex_mode::FDD},
    {nr_band::n5,  duplex_mode::FDD},
    {nr_band::n7,  duplex_mode::FDD},
    {nr_band::n8,  duplex_mode::FDD},
    {nr_band::n12, duplex_mode::FDD},
    {nr_band::n13, duplex_mode::FDD},
    {nr_band::n14, duplex_mode::FDD},
    {nr_band::n18, duplex_mode::FDD},
    {nr_band::n20, duplex_mode::FDD},
    {nr_band::n24, duplex_mode::FDD},
    {nr_band::n25, duplex_mode::FDD},
    {nr_band::n26, duplex_mode::FDD},
    {nr_band::n28, duplex_mode::FDD},
    {nr_band::n29, duplex_mode::SDL},
    {nr_band::n30, duplex_mode::FDD},
    {nr_band::n34, duplex_mode::TDD},
    {nr_band::n38, duplex_mode::TDD},
    {nr_band::n39, duplex_mode::TDD},
    {nr_band::n40, duplex_mode::TDD},
    {nr_band::n41, duplex_mode::TDD},
    {nr_band::n46, duplex_mode::TDD},
    {nr_band::n48, duplex_mode::TDD},
    {nr_band::n50, duplex_mode::TDD},
    {nr_band::n51, duplex_mode::TDD},
    {nr_band::n53, duplex_mode::TDD},
    {nr_band::n65, duplex_mode::FDD},
    {nr_band::n66, duplex_mode::FDD},
    {nr_band::n67, duplex_mode::SDL},
    {nr_band::n70, duplex_mode::FDD},
    {nr_band::n71, duplex_mode::FDD},
    {nr_band::n74, duplex_mode::FDD},
    {nr_band::n75, duplex_mode::SDL},
    {nr_band::n76, duplex_mode::SDL},
    {nr_band::n77, duplex_mode::TDD},
    {nr_band::n78, duplex_mode::TDD},
    {nr_band::n79, duplex_mode::TDD},
    {nr_band::n80, duplex_mode::SUL},
    {nr_band::n81, duplex_mode::SUL},
    {nr_band::n82, duplex_mode::SUL},
    {nr_band::n83, duplex_mode::SUL},
    {nr_band::n84, duplex_mode::SUL},
    {nr_band::n85, duplex_mode::FDD},
    {nr_band::n86, duplex_mode::SUL},
    {nr_band::n89, duplex_mode::SUL},
    {nr_band::n90, duplex_mode::TDD},
    {nr_band::n91, duplex_mode::FDD},
    {nr_band::n92, duplex_mode::FDD},
    {nr_band::n93, duplex_mode::FDD},
    {nr_band::n94, duplex_mode::FDD},
    {nr_band::n95, duplex_mode::SUL},
    {nr_band::n96, duplex_mode::TDD},
    {nr_band::n97, duplex_mode::SUL},
    {nr_band::n98, duplex_mode::SUL},
    {nr_band::n99, duplex_mode::SUL},
    {nr_band::n100, duplex_mode::FDD},
    {nr_band::n101, duplex_mode::TDD},
    {nr_band::n102, duplex_mode::TDD},
    {nr_band::n104, duplex_mode::TDD},
    {nr_band::n255, duplex_mode::FDD},
    {nr_band::n256, duplex_mode::FDD},
    {nr_band::n257, duplex_mode::TDD},
    {nr_band::n258, duplex_mode::TDD},
    {nr_band::n259, duplex_mode::TDD},
    {nr_band::n260, duplex_mode::TDD},
    {nr_band::n261, duplex_mode::TDD},
    {nr_band::n262, duplex_mode::TDD}
    // clang-format on
}};

namespace {

/// NR operating band in FR1 with SSB Subcarrier Spacing and SSB pattern case, as per Table 5.4.3.3-1,
/// TS 38.104, Rel. 17, version 17.8.0.
struct nr_band_ssb_scs_case {
  nr_band            band;
  subcarrier_spacing scs;
  ssb_pattern_case   pattern;
};

} // namespace

/// NR operating bands with corresponding SSB Subcarrier Spacing and SSB pattern case, as per Table 5.4.3.3-1 for FR1
/// and Table 5.4.3.3-1 for FR2, TS 38.104, Rel. 17, version 17.8.0.
static constexpr unsigned                                           nof_nr_ssb_bands           = 72;
static constexpr std::array<nr_band_ssb_scs_case, nof_nr_ssb_bands> nr_ssb_band_scs_case_table = {{
    // clang-format off
    {nr_band::n1,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n2,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n3,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n5,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n5,  subcarrier_spacing::kHz30, ssb_pattern_case::B},
    {nr_band::n7,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n8,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n12, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n13, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n14, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n18, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n20, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n24, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n24, subcarrier_spacing::kHz30, ssb_pattern_case::B},
    {nr_band::n25, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n26, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n28, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n29, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n30, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n34, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n34, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n38, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n38, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n39, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n39, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n40, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n41, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n41, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n46, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n48, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n50, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n51, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n53, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n65, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n66, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n66, subcarrier_spacing::kHz30, ssb_pattern_case::B},
    {nr_band::n67, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n70, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n71, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n74, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n75, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n76, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n77, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n78, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n79, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n85, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n90, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n90, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n91, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n92, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n93, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n94, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n96, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n100, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n101, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n101, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n102, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n104, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n255, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n256, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n257,  subcarrier_spacing::kHz120, ssb_pattern_case::D},
    {nr_band::n257,  subcarrier_spacing::kHz240, ssb_pattern_case::E},
    {nr_band::n258,  subcarrier_spacing::kHz120, ssb_pattern_case::D},
    {nr_band::n258,  subcarrier_spacing::kHz240, ssb_pattern_case::E},
    {nr_band::n259,  subcarrier_spacing::kHz120, ssb_pattern_case::D},
    {nr_band::n259,  subcarrier_spacing::kHz240, ssb_pattern_case::E},
    {nr_band::n260,  subcarrier_spacing::kHz120, ssb_pattern_case::D},
    {nr_band::n260,  subcarrier_spacing::kHz240, ssb_pattern_case::E},
    {nr_band::n261,  subcarrier_spacing::kHz120, ssb_pattern_case::D},
    {nr_band::n261,  subcarrier_spacing::kHz240, ssb_pattern_case::E},
    {nr_band::n262,  subcarrier_spacing::kHz120, ssb_pattern_case::D},
    {nr_band::n262,  subcarrier_spacing::kHz240, ssb_pattern_case::E},
    // clang-format on
}};

namespace {

struct nr_raster_params {
  double   freq_range_start;
  double   freq_range_end;
  double   delta_F_global_kHz;
  double   F_REF_Offs_MHz;
  uint32_t N_REF_Offs;
  uint32_t N_REF_min;
  uint32_t N_REF_max;

  bool operator==(const nr_raster_params& rhs) const
  {
    return freq_range_start == rhs.freq_range_start && freq_range_end == rhs.freq_range_end &&
           delta_F_global_kHz == rhs.delta_F_global_kHz && F_REF_Offs_MHz == rhs.F_REF_Offs_MHz &&
           N_REF_Offs == rhs.N_REF_Offs && N_REF_min == rhs.N_REF_min && N_REF_max == rhs.N_REF_max;
  }
};

} // namespace

/// NR-ARFCN parameters for the global frequency raster, as per Table 5.4.2.1-1, TS 38.104, Rel. 17, version 17.8.0.
static constexpr unsigned                        max_nr_arfcn = 3279165;
static constexpr std::array<nr_raster_params, 3> nr_fr_params = {{
    // clang-format off
    // Frequency range 0 - 3000 MHz
    {0, 3000, 5, 0.0, 0, 0, 599999},
    // Frequency range 3000 - 24250 MHz
    {3000, 24250, 15, 3000.0, 600000, 600000, 2016666},
    // Frequency range 24250 - 100000 MHz
    {24250, 100000, 60, 24250.08, 2016667, 2016667, max_nr_arfcn}
    // clang-format on
}};

namespace {

struct n_rb_per_scs_fr1 {
  bs_channel_bandwidth bw;
  unsigned             n_rb_15kHz;
  unsigned             n_rb_30kHz;
  unsigned             n_rb_60kHz;
};

} // namespace

/// This implements Table 5.3.2-1 in TS 38.104. Value N_RB = 0 represent N/A.
static constexpr std::array<n_rb_per_scs_fr1, 15> tx_bw_config_fr1 = {{
    // clang-format off
    // BW = 5MHz.
    {bs_channel_bandwidth::MHz5, 25, 11, 0},
    // BW = 10MHz.
    {bs_channel_bandwidth::MHz10, 52, 24, 11},
    // BW = 15MHz.
    {bs_channel_bandwidth::MHz15, 79, 38, 18},
    // BW = 20MHz.
    {bs_channel_bandwidth::MHz20, 106, 51, 24},
    // BW = 25MHz.
    {bs_channel_bandwidth::MHz25, 133, 65, 31},
    // BW = 30MHz.
    {bs_channel_bandwidth::MHz30, 160, 78, 38},
    // BW = 35MHz.
    {bs_channel_bandwidth::MHz35, 188, 92, 44},
    // BW = 40MHz.
    {bs_channel_bandwidth::MHz40, 216, 106, 51},
    // BW = 45MHz.
    {bs_channel_bandwidth::MHz45, 242, 119, 58},
    // BW = 50MHz.
    {bs_channel_bandwidth::MHz50, 270, 133, 65},
    // BW = 60MHz.
    {bs_channel_bandwidth::MHz60, 0, 162, 79},
    // BW = 70MHz.
    {bs_channel_bandwidth::MHz70, 0, 189, 93},
    // BW = 80MHz.
    {bs_channel_bandwidth::MHz80, 0, 217, 107},
    // BW = 90MHz.
    {bs_channel_bandwidth::MHz90, 0, 245, 121},
    // BW = 100MHz.
    {bs_channel_bandwidth::MHz100, 0, 273, 135}
    // clang-format on
}};

namespace {

struct n_rb_per_scs_fr2 {
  bs_channel_bandwidth bw;
  unsigned             n_rb_60kHz;
  unsigned             n_rb_120kHz;
};

} // namespace

/// This implements Table 5.3.2-2 in TS 38.104. Value N_RB = 0 represent N/A.
static constexpr std::array<n_rb_per_scs_fr2, 15> tx_bw_config_fr2 = {{
    // clang-format off
    // BW = 50MHz.
    {bs_channel_bandwidth::MHz50, 66, 32},
    // BW = 100MHz.
    {bs_channel_bandwidth::MHz100, 132, 66},
    // BW = 200MHz.
    {bs_channel_bandwidth::MHz200, 264, 132},
    // BW = 400MHz.
    {bs_channel_bandwidth::MHz400, 0, 264},
    // clang-format on
}};

static nr_band_raster fetch_band_raster(nr_band band, std::optional<delta_freq_raster> delta_freq_raster)
{
  if (band == nr_band::n41 or band == nr_band::n48 or band == nr_band::n77 or band == nr_band::n78 or
      band == nr_band::n79 or band == nr_band::n90 or band == nr_band::n104) {
    srsran_assert(
        delta_freq_raster.has_value(),
        "For band n41, n48, n77, n78, n79, n90 and n104, the band freq. raster require Delta Freq. Raster as an input");
  }

  const auto* it = std::find_if(
      nr_band_table.begin(), nr_band_table.end(), [band, delta_freq_raster](const nr_band_raster& raster_band) {
        return delta_freq_raster.has_value()
                   ? raster_band.band == band and raster_band.delta_f_rast == delta_freq_raster
                   : raster_band.band == band;
      });

  return it != nr_band_table.end() ? *it : nr_band_raster{.band = nr_band ::invalid};
}

/// Helper to calculate F_REF according to Table 5.4.2.1-1.
static nr_raster_params get_raster_params(uint32_t nr_arfcn)
{
  for (const nr_raster_params& fr : nr_fr_params) {
    if (nr_arfcn >= fr.N_REF_min && nr_arfcn <= fr.N_REF_max) {
      return fr;
    }
  }
  // Return empty params.
  return {};
}

static nr_raster_params get_raster_params(double freq)
{
  for (const nr_raster_params& fr : nr_fr_params) {
    if (freq >= fr.freq_range_start * 1e6 && freq <= fr.freq_range_end * 1e6) {
      return fr;
    }
  }
  // Return empty params.
  return {};
}

static bool is_valid_raster_param(const nr_raster_params& raster)
{
  return std::any_of(
      nr_fr_params.begin(), nr_fr_params.end(), [raster](const nr_raster_params& fr) { return fr == raster; });
}

// Validates band n28, which has an additional ARFCN value to the given interval, as per Table 5.4.2.3-1, TS 38.104,
// version 17.8.0.
static error_type<std::string> validate_band_n28(uint32_t arfcn, bs_channel_bandwidth bw, bool is_dl = true)
{
  const nr_band_raster band_raster = fetch_band_raster(nr_band::n28, {});
  if (band_raster.band == srsran::nr_band::invalid) {
    return make_unexpected(fmt::format("Band n28 channel raster not found"));
  }

  // Try first if the ARFCN matches any value of the interval for 100kHz channel raster.
  uint32_t nref_first = is_dl ? band_raster.dl_nref_first : band_raster.ul_nref_first;
  uint32_t nref_last  = is_dl ? band_raster.dl_nref_last : band_raster.ul_nref_last;
  uint32_t nref_step  = is_dl ? band_raster.dl_nref_step : band_raster.ul_nref_step;
  if (arfcn >= nref_first and arfcn <= nref_last and ((arfcn - nref_first) % nref_step) == 0) {
    return error_type<std::string>{};
  }

  // Extra ARFCN value as per Table 5.4.2.3-1, TS 38.104, version 17.8.0 (see NOTE 4 in the table).
  const uint32_t arfnc_40MHz = is_dl ? 155608U : 144608U;
  if (bw == srsran::bs_channel_bandwidth::MHz40 and arfcn == arfnc_40MHz) {
    return error_type<std::string>{};
  }

  return make_unexpected(
      fmt::format("{} ARFCN must be within the interval [{},{}], in steps of {}, for the chosen band",
                  is_dl ? "DL" : "UL",
                  nref_first,
                  nref_last,
                  nref_step));
}

// Validates band n46, whose valid ARFCN values depend on the channel BW, as per Table 5.4.2.3-1, TS 38.104,
// version 17.8.0.
static error_type<std::string> validate_band_n46(uint32_t arfcn, bs_channel_bandwidth bw)
{
  static constexpr std::array<unsigned, 2>  n46_b_10_dlarfnc = {782000, 788668};
  static constexpr std::array<unsigned, 32> n46_b_20_dlarfnc = {
      // clang-format off
      744000, 745332, 746668, 748000, 749332, 750668, 752000, 753332, 754668, 756000, 765332, 766668, 768000, 769332,
      770668, 772000, 773332, 774668, 776000, 777332, 778668, 780000, 781332, 783000, 784332, 785668, 787000, 788332,
      789668, 791000, 792332, 793668
      // clang-format on
  };
  static constexpr std::array<unsigned, 18> n46_b_40_dlarfnc = {
      // clang-format off
      744668, 746000, 748668, 751332, 754000, 755332, 766000, 767332, 770000, 772668, 775332, 778000, 780668, 783668,
      786332, 787668, 790332, 793000
      // clang-format on
  };
  static constexpr std::array<unsigned, 17> n46_b_60_dlarfnc = {
      // clang-format off
      745332, 746668, 748000,  752000, 753332, 754668, 766668, 768000, 769332, 773332, 774668, 778668, 780000, 784332,
      785668, 791000, 792332
      // clang-format on
  };
  static constexpr std::array<unsigned, 10> n46_b_80_dlarfnc = {
      746000, 747332, 752668, 754000, 767332, 768668, 774000, 779332, 785000, 791668};
  static constexpr std::array<unsigned, 4> n46_b_100_dlarfnc = {746668, 753332, 768000, 791000};
  static const char* error_msg = "Only a restricted set of DL-ARFCN values are allowed in band n46";

  auto dl_arfcn_exist = [](span<const unsigned> band_list, unsigned dl_arfcn) {
    return std::find(band_list.begin(), band_list.end(), dl_arfcn) != band_list.end();
  };

  switch (bw) {
    case bs_channel_bandwidth::MHz10: {
      return dl_arfcn_exist(span<const unsigned>(n46_b_10_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                           : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz20: {
      return dl_arfcn_exist(span<const unsigned>(n46_b_20_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                           : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz40: {
      return dl_arfcn_exist(span<const unsigned>(n46_b_40_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                           : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz60: {
      return dl_arfcn_exist(span<const unsigned>(n46_b_60_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                           : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz80: {
      return dl_arfcn_exist(span<const unsigned>(n46_b_80_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                           : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz100: {
      return dl_arfcn_exist(span<const unsigned>(n46_b_100_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                            : make_unexpected(fmt::format(error_msg));
    }
    default:
      return make_unexpected(fmt::format("DL-ARFCN not valid for band n46."));
  }
}

/// Validates band n66, whose valid ARFCN values depend on the channel BW, as per Table 5.4.2.3-1, TS 38.104,
/// version 17.8.0.
static error_type<std::string> validate_band_n96(uint32_t arfcn, bs_channel_bandwidth bw)
{
  static constexpr std::array<unsigned, 59> b_20_dlarfnc = {
      // clang-format off
      797000, 798332, 799668, 801000, 802332, 803668, 805000, 806332, 807668, 809000, 810332, 811668, 813000, 814332,
      815668, 817000, 818332, 819668, 821000, 822332, 823668, 825000, 826332, 827668, 829000, 830332, 831668, 833000,
      834332, 835668, 837000, 838332, 839668, 841000, 842332, 843668, 845000, 846332, 847668, 849000, 850332, 851668,
      853000, 854332, 855668, 857000, 858332, 859668, 861000, 862332, 863668, 865000, 866332, 867668, 869000, 870332,
      871668, 873000, 874332
      // clang-format on
  };
  static constexpr std::array<unsigned, 29> b_40_dlarfnc = {
      // clang-format off
      797668, 800332, 803000, 805668, 808332, 811000, 813668, 816332, 819000, 821668, 824332, 827000, 829668, 832332,
      835000, 837668, 840332, 843000, 845668, 848332, 851000, 853668, 856332, 859000, 861668, 864332, 867000, 869668,
      872332
      // clang-format on
  };
  static constexpr std::array<unsigned, 29> b_60_dlarfnc = {
      // clang-format off
      798332, 799668, 803668, 805000, 809000, 810332, 814332, 815668, 819668, 821000, 825000, 826332, 830332, 831668,
      835668, 837000, 841000, 842332, 846332, 847668, 851668, 853000, 857000, 858332, 862332, 863668, 867668, 869000,
      873000
      // clang-format on
  };
  static constexpr std::array<unsigned, 14> b_80_dlarfnc = {
      // clang-format off
      799000, 804332, 809668, 815000, 820332, 825668, 831000, 836332, 841668, 847000, 852332, 857668, 863000, 868332
      // clang-format on
  };
  static constexpr std::array<unsigned, 17> b_100_dlarfnc = {
      // clang-format off
      799668, 803668, 810332, 814332, 821000, 825000, 831668, 835668, 842332, 846332, 853000, 857000, 863668, 867668,
      869000, 870332, 871668
      // clang-format on
  };

  auto dl_arfcn_exist = [](span<const unsigned> band_list, unsigned dl_arfcn) {
    return std::find(band_list.begin(), band_list.end(), dl_arfcn) != band_list.end();
  };

  static const char* error_msg = "Only a restricted set of DL-ARFCN values are allowed in band n96";
  switch (bw) {
    case bs_channel_bandwidth::MHz20: {
      return dl_arfcn_exist(span<const unsigned>(b_20_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz40: {
      return dl_arfcn_exist(span<const unsigned>(b_40_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz60: {
      return dl_arfcn_exist(span<const unsigned>(b_60_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz80: {
      return dl_arfcn_exist(span<const unsigned>(b_80_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz100: {
      return dl_arfcn_exist(span<const unsigned>(b_100_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                        : make_unexpected(fmt::format(error_msg));
    }
    default:
      return make_unexpected(fmt::format("DL-ARFCN not valid for band n96."));
  }
}

/// Validates band n102, whose valid ARFCN values depend on the channel BW, as per Table 5.4.2.3-1, TS 38.104,
/// version 17.8.0.
static error_type<std::string> validate_band_n102(uint32_t arfcn, bs_channel_bandwidth bw)
{
  static constexpr std::array<unsigned, 24> b_20_dlarfnc = {
      // clang-format off
      797000, 798332, 799668, 801000, 802332, 803668, 805000, 806332, 807668, 809000, 810332, 811668, 813000, 814332,
      815668, 817000, 818332, 819668, 821000, 822332, 823668, 825000, 826332, 827668
      // clang-format on
  };
  static constexpr std::array<unsigned, 12> b_40_dlarfnc = {
      // clang-format off
      797668, 800332, 803000, 805668, 808332, 811000, 813668, 816332, 819000, 821668, 824332, 827000
      // clang-format on
  };
  static constexpr std::array<unsigned, 12> b_60_dlarfnc = {
      // clang-format off
      798332, 799668, 803668, 805000, 809000, 810332, 814332, 815668, 819668, 821000, 825000, 826332
      // clang-format on
  };
  static constexpr std::array<unsigned, 6> b_80_dlarfnc  = {799000, 804332, 809668, 815000, 820332, 825668};
  static constexpr std::array<unsigned, 6> b_100_dlarfnc = {799668, 803668, 810332, 814332, 821000, 825000};

  const nr_band_raster band_raster = fetch_band_raster(nr_band::n102, {});
  if (band_raster.band == srsran::nr_band::invalid or arfcn < band_raster.dl_nref_first or
      arfcn > band_raster.dl_nref_last) {
    return make_unexpected(fmt::format("Band n102 channel raster not found"));
  }

  auto dl_arfcn_exist = [](span<const unsigned> band_list, unsigned dl_arfcn) {
    return std::find(band_list.begin(), band_list.end(), dl_arfcn) != band_list.end();
  };

  static const char* error_msg = "Only a restricted set of DL-ARFCN values are allowed in band n102";
  switch (bw) {
    case bs_channel_bandwidth::MHz20: {
      return dl_arfcn_exist(span<const unsigned>(b_20_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz40: {
      return dl_arfcn_exist(span<const unsigned>(b_40_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz60: {
      return dl_arfcn_exist(span<const unsigned>(b_60_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz80: {
      return dl_arfcn_exist(span<const unsigned>(b_80_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                       : make_unexpected(fmt::format(error_msg));
    }
    case bs_channel_bandwidth::MHz100: {
      return dl_arfcn_exist(span<const unsigned>(b_100_dlarfnc), arfcn) ? error_type<std::string>{}
                                                                        : make_unexpected(fmt::format(error_msg));
    }
    default:
      return make_unexpected(fmt::format("DL-ARFCN not valid for band n102."));
  }
}

static error_type<std::string> validate_band_n90(uint32_t arfcn, subcarrier_spacing scs)
{
  // Band n90 needs to be handled separately. Since it can take a Delta freq raster value among three possible ones
  // {15kHz, 30kHz, 100kHz}, we need to first check if the DL ARFCN is compatible with 100kHz; if not, we assume Delta
  // freq raster equal to the SCS common and check whether the DL ARFCN is compatible with this value.
  // Try first 100kHz channel raster.
  nr_band_raster band_raster = fetch_band_raster(nr_band::n90, delta_freq_raster::kHz100);
  if (band_raster.band == srsran::nr_band::invalid) {
    return make_unexpected(fmt::format("Band n90 channel raster for SCS {} not found", to_string(scs)));
  }

  if (arfcn >= band_raster.dl_nref_first and arfcn <= band_raster.dl_nref_last and
      ((arfcn - band_raster.dl_nref_first) % band_raster.dl_nref_step) == 0) {
    return error_type<std::string>{};
  }

  // The previous check failed, try now with freq raster equal to the SCS common.
  band_raster = fetch_band_raster(
      nr_band::n90, scs == subcarrier_spacing::kHz15 ? delta_freq_raster::kHz15 : delta_freq_raster::kHz30);
  if (band_raster.band != srsran::nr_band::invalid) {
    if (arfcn >= band_raster.dl_nref_first and arfcn <= band_raster.dl_nref_last and
        ((arfcn - band_raster.dl_nref_first) % band_raster.dl_nref_step) == 0) {
      return error_type<std::string>{};
    }
  }
  return make_unexpected(
      fmt::format("DL ARFCN for band n90 is either outside the allowed interval or not compatible with the step size"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

nr_band srsran::band_helper::get_band_from_dl_arfcn(uint32_t arfcn_f_ref)
{
  // As per Table 5.4.2.3-1, TS 38.104, v17.8.0, band n28 has an additional ARFCN value outside the interval of step 20.
  const uint32_t arfcn_n28 = 155608U;
  if (arfcn_f_ref == arfcn_n28) {
    return nr_band::n28;
  }

  for (const nr_band_raster& band : nr_band_table) {
    // Check given ARFCN is between the first and last possible ARFCN.
    if (arfcn_f_ref >= band.dl_nref_first and arfcn_f_ref <= band.dl_nref_last and
        ((arfcn_f_ref - band.dl_nref_first) % band.dl_nref_step) == 0) {
      return band.band;
    }
  }
  return nr_band::invalid;
}

error_type<std::string> srsran::band_helper::is_dl_arfcn_valid_given_band(nr_band              band,
                                                                          uint32_t             arfcn_f_ref,
                                                                          subcarrier_spacing   scs,
                                                                          bs_channel_bandwidth bw)
{
  // Validates first the bands with non-standard ARFCN values.
  if (band == nr_band::n28) {
    return validate_band_n28(arfcn_f_ref, bw, true);
  }

  if (band == nr_band::n46) {
    return validate_band_n46(arfcn_f_ref, bw);
  }

  if (band == nr_band::n90) {
    return validate_band_n90(arfcn_f_ref, scs);
  }

  if (band == nr_band::n96) {
    return validate_band_n96(arfcn_f_ref, bw);
  }

  if (band == nr_band::n102) {
    return validate_band_n102(arfcn_f_ref, bw);
  }

  // NOTE: This function restricts the choice of ARFCN for bands n41, n77, n78, and n79. As per Section 5.4.2.3,
  // TS 38.104, Delta freq raster of 30kHz cannot be used for SSB SCS and SCS common of 15kHz. In this function we
  // make a stronger requirement, i.e., we set Delta freq raster equal to the SSB SCS and SCS common.

  // Assume standard Delta freq raster of 100kHz.
  delta_freq_raster band_delta_freq_raster = delta_freq_raster::kHz100;

  // Update Delta freq raster based on SCS for bands n41, n48, n77, n78, n79 and n104.
  if (band == nr_band::n41 or band == nr_band::n48 or band == nr_band::n77 or band == nr_band::n78 or
      band == nr_band::n79 or band == nr_band::n104) {
    band_delta_freq_raster = scs == subcarrier_spacing::kHz15 ? delta_freq_raster::kHz15 : delta_freq_raster::kHz30;
  }

  // Update Delta freq raster based on SCS for FR2 bands.
  if (get_freq_range(band) == frequency_range::FR2) {
    band_delta_freq_raster = (scs == subcarrier_spacing::kHz60) ? delta_freq_raster::kHz60 : delta_freq_raster::kHz120;
  }

  for (const nr_band_raster& raster_band : nr_band_table) {
    if (raster_band.band == band and raster_band.delta_f_rast == band_delta_freq_raster) {
      if (arfcn_f_ref >= raster_band.dl_nref_first and arfcn_f_ref <= raster_band.dl_nref_last and
          ((arfcn_f_ref - raster_band.dl_nref_first) % raster_band.dl_nref_step) == 0) {
        return {};
      }
      return make_unexpected(
          fmt::format("DL ARFCN must be within the interval [{},{}], in steps of {}, for the chosen band",
                      raster_band.dl_nref_first,
                      raster_band.dl_nref_last,
                      raster_band.dl_nref_step));
    }
  }
  return make_unexpected(fmt::format("Band {} is not valid", fmt::underlying(band)));
}

error_type<std::string>
srsran::band_helper::is_ul_arfcn_valid_given_band(nr_band band, uint32_t arfcn_f_ref, bs_channel_bandwidth bw)
{
  if (get_duplex_mode(band) != duplex_mode::FDD) {
    return {};
  }

  // Validates first the bands with non-standard ARFCN values.
  if (band == nr_band::n28) {
    return validate_band_n28(arfcn_f_ref, bw, false);
  }

  // Assume standard Delta freq raster of 100kHz.
  const delta_freq_raster band_delta_freq_raster = delta_freq_raster::kHz100;

  for (const nr_band_raster& raster_band : nr_band_table) {
    if (raster_band.band == band and raster_band.delta_f_rast == band_delta_freq_raster) {
      // Check if the ARFCN doesn't exceed the band upper-bound for bands that support asymmetrical UL and DL channel
      // BWs.
      if ((band == nr_band::n66 or band == nr_band::n70 or band == nr_band::n92 or band == nr_band::n94) and
          (arfcn_f_ref < raster_band.ul_nref_first or arfcn_f_ref > raster_band.ul_nref_last)) {
        return make_unexpected(
            fmt::format("Asymmetrical UL and DL channel BWs are not supported. The UL ARFCN resulting from the DL "
                        "ARFCN for band n{} must not exceed the band upper-bound={}",
                        fmt::underlying(band),
                        raster_band.ul_nref_last));
      }
      if (arfcn_f_ref >= raster_band.ul_nref_first and arfcn_f_ref <= raster_band.ul_nref_last and
          ((arfcn_f_ref - raster_band.ul_nref_first) % raster_band.ul_nref_step) == 0) {
        return {};
      }
      return make_unexpected(
          fmt::format("UL ARFCN must be within the interval [{},{}], in steps of {}, for the chosen band",
                      raster_band.ul_nref_first,
                      raster_band.ul_nref_last,
                      raster_band.ul_nref_step));
    }
  }
  return make_unexpected(fmt::format("Band {} is not valid", fmt::underlying(band)));
}

uint32_t srsran::band_helper::get_ul_arfcn_from_dl_arfcn(uint32_t dl_arfcn, std::optional<nr_band> band)
{
  // NOTE: The procedure implemented in this function is implementation-defined.
  const nr_band operating_band = band.value_or(get_band_from_dl_arfcn(dl_arfcn));

  // Return same ARFCN for TDD bands.
  if (get_duplex_mode(operating_band) == duplex_mode::TDD) {
    return dl_arfcn;
  }

  // Extra ARFCN value as per Table 5.4.2.3-1, TS 38.104, version 17.8.0 (see NOTE 4 in the table).
  const uint32_t n28_b40_dl_arfcn = 155608U;
  const uint32_t n28_b40_ul_arfcn = 144608U;
  if (band == nr_band::n28 and dl_arfcn == n28_b40_dl_arfcn) {
    return n28_b40_ul_arfcn;
  }

  // Derive UL ARFCN for FDD bands.
  for (const nr_band_raster& b_it : nr_band_table) {
    if (b_it.band == operating_band) {
      const uint32_t offset             = (dl_arfcn - b_it.dl_nref_first) / b_it.dl_nref_step;
      const uint32_t candidate_ul_arfcn = b_it.ul_nref_first + offset * b_it.ul_nref_step;
      // For band n66, n70, n92, n94, the UL spectrum is smaller than the corresponding DL spectrum, as these bands
      // supports asymmetrical UL anc DL channel operations. However, the current GNB doesn't support this feature.
      // If the resulting UL ARFCN is outside the valid range, return 0.
      return (candidate_ul_arfcn >= b_it.ul_nref_first and candidate_ul_arfcn <= b_it.ul_nref_last) ? candidate_ul_arfcn
                                                                                                    : 0U;
    }
  }

  return 0;
}

double srsran::band_helper::nr_arfcn_to_freq(uint32_t nr_arfcn)
{
  const nr_raster_params params = get_raster_params(nr_arfcn);
  if (not is_valid_raster_param(params)) {
    return 0.0;
  }
  return (params.F_REF_Offs_MHz * 1e6 + params.delta_F_global_kHz * (nr_arfcn - params.N_REF_Offs) * 1e3);
}

uint32_t srsran::band_helper::freq_to_nr_arfcn(double freq)
{
  const nr_raster_params params = get_raster_params(freq);
  if (not is_valid_raster_param(params)) {
    return 0;
  }
  return static_cast<uint32_t>(((freq - params.F_REF_Offs_MHz * 1e6) / 1e3 / params.delta_F_global_kHz) +
                               params.N_REF_Offs);
}

bool srsran::band_helper::is_band_for_shared_spectrum(nr_band band)
{
  // As per TS 38.104, Table 5.2-1, only bands where Note 3 or Note 4 apply.
  return band == nr_band::n46 or band == nr_band::n96 or band == nr_band::n102;
}

bool srsran::band_helper::is_band_40mhz_min_ch_bw_equivalent(nr_band band)
{
  // As per TS 38.101, Table 5.2-1, only bands where Note 17 applies.
  return band == nr_band::n79 or band == nr_band::n104;
}

bool srsran::band_helper::is_ntn_band(nr_band band)
{
  return band == nr_band::n254 or band == nr_band::n255 or band == nr_band::n256;
}

ssb_pattern_case srsran::band_helper::get_ssb_pattern(nr_band band, subcarrier_spacing scs)
{
  // Look for the given band and SCS.
  for (const nr_band_ssb_scs_case& ssb_scs_case : nr_ssb_band_scs_case_table) {
    // As bands are in ascending order, do not waste more time if the current band is bigger.
    if (ssb_scs_case.band > band) {
      return ssb_pattern_case::invalid;
    }

    if (ssb_scs_case.band == band and ssb_scs_case.scs == scs) {
      return ssb_scs_case.pattern;
    }
  }

  // Band is out of range, so consider invalid.
  return ssb_pattern_case::invalid;
}

uint8_t srsran::band_helper::get_ssb_l_max(nr_band band, subcarrier_spacing scs, uint32_t nr_arfcn)
{
  // As per TS 38.213, Section 4.1.
  switch (get_ssb_pattern(band, scs)) {
    case ssb_pattern_case::A:
    case ssb_pattern_case::C: {
      const uint32_t ssb_cut_off_freq =
          is_paired_spectrum(band) ? CUTOFF_FREQ_ARFCN_CASE_A_B_C : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
      return (nr_arfcn <= ssb_cut_off_freq) ? 4U : 8U;
    }
    case ssb_pattern_case::B: {
      return 8U;
    }
    case ssb_pattern_case::D:
    case ssb_pattern_case::E: {
      return 64U;
    }
    default:
      report_fatal_error("SSB pattern case is invalid");
      return 0U;
  }
}

subcarrier_spacing srsran::band_helper::get_most_suitable_ssb_scs(nr_band band, subcarrier_spacing scs_common)
{
  subcarrier_spacing lowest_scs = subcarrier_spacing::invalid;

  // Look for the given band and SCS
  for (const nr_band_ssb_scs_case& ssb_case : nr_ssb_band_scs_case_table) {
    // Check if band and SCS match.
    if (ssb_case.band == band) {
      // If the SCS match with common, then that's the most suitable.
      if (scs_common == ssb_case.scs) {
        return ssb_case.scs;
      }

      // If it is the first match of the band, then store as the lowest SCS.
      if (lowest_scs == subcarrier_spacing::invalid) {
        lowest_scs = ssb_case.scs;
      }
    }

    // As bands are in ascending order, do not waste more time if the current band is bigger.
    if (ssb_case.band > band) {
      return lowest_scs;
    }
  }

  // Band is out of range, so consider invalid.
  return subcarrier_spacing::invalid;
}

duplex_mode srsran::band_helper::get_duplex_mode(nr_band band)
{
  // Look for the given band.
  for (const nr_operating_band& b : nr_operating_bands) {
    // Check if band and SCS match!
    if (b.band == band) {
      return b.duplex;
    }

    // As bands are in ascending order, do not waste more time if the current band is bigger.
    if (b.band > band) {
      return duplex_mode::INVALID;
    }
  }

  // Band is out of range, so consider invalid.
  return duplex_mode::INVALID;
}

bool srsran::band_helper::is_paired_spectrum(nr_band band)
{
  const duplex_mode mode = get_duplex_mode(band);
  srsran_assert(mode < duplex_mode::INVALID, "Returned invalid duplex MODE");
  return mode == duplex_mode::FDD;
}

bool srsran::band_helper::is_unlicensed_band(nr_band band)
{
  srsran_assert(band != nr_band::invalid, "Band must be a valid NR band.");
  switch (band) {
    case nr_band::n46:
    case nr_band::n48:
    case nr_band::n96:
    case nr_band::n102:
    case nr_band::n104:
      return true;
    default:
      return false;
  }
}

frequency_range srsran::band_helper::get_freq_range(nr_band band)
{
  srsran_assert(band != nr_band::invalid, "Band must be a valid NR band.");
  return (band <= nr_band::n104 || band == nr_band::n255 || band == nr_band::n256) ? frequency_range::FR1
                                                                                   : frequency_range::FR2;
}

double srsran::band_helper::get_abs_freq_point_a_from_center_freq(uint32_t nof_prb, double center_freq)
{
  static constexpr unsigned NRE = 12;

  // For FR1 unit of resources blocks for freq calc is always 180kHz regardless for actual SCS of carrier.
  // TODO: add offset_to_carrier.
  return center_freq - static_cast<double>(nof_prb / 2 * scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ * NRE);
}

uint32_t srsran::band_helper::get_abs_freq_point_a_arfcn(uint32_t nof_prb, uint32_t arfcn_f_ref)
{
  return freq_to_nr_arfcn(get_abs_freq_point_a_from_center_freq(nof_prb, nr_arfcn_to_freq(arfcn_f_ref)));
}

double srsran::band_helper::get_center_freq_from_abs_freq_point_a(uint32_t nof_prb, uint32_t freq_point_a_arfcn)
{
  static constexpr unsigned NRE = 12;
  // for FR1 unit of resources blocks for freq calc is always 180kHz regardless for actual SCS of carrier.
  // TODO: add offset_to_carrier
  const double abs_freq_point_a_freq = nr_arfcn_to_freq(freq_point_a_arfcn);
  return abs_freq_point_a_freq +
         static_cast<double>(nof_prb / 2 * scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ * NRE);
}

double srsran::band_helper::get_abs_freq_point_a_from_f_ref(double f_ref, uint32_t nof_rbs, subcarrier_spacing scs)
{
  // NOTE (i): It is unclear whether the SCS should always be 15kHz for FR1 (\ref get_abs_freq_point_a_from_center_freq
  // and see note).
  // NOTE (ii): TS 38.104, Section 5.4.2.2, reports <em>[...] The mapping must apply to at least one numerology
  // supported by the BS.<\em>. Therefore, the correct SCS to be used in this procedure still needs to determined.

  // Half of the number of subcarriers in a RE.
  static constexpr unsigned NRE_half = 6;

  // The procedure, which is explained in TS 38.104, Section 5.4.2.2, gives the position of f_ref in terms of subcarrier
  // and CRB index, depending on the size of N_RB. Below we compute the value in unit of subcarriers, meaning we don't
  // need to separate the cases of even and odd N_RB.
  const unsigned delta_point_a_f_ref = nof_rbs * NRE_half;
  return f_ref - static_cast<double>(delta_point_a_f_ref * scs_to_khz(scs) * KHZ_TO_HZ);
}

double
srsran::band_helper::get_f_ref_from_abs_freq_point_a(double abs_freq_point_a, uint32_t nof_rbs, subcarrier_spacing scs)
{
  // See notes in \ref get_abs_freq_point_a_from_f_ref.

  // Half of the number of subcarriers in a RE.
  static constexpr unsigned NRE_half = 6;
  // The procedure used in this function is the inverse of what explained in TS 38.104, Section 5.4.2.2.
  const unsigned delta_point_a_f_ref = nof_rbs * NRE_half;
  return abs_freq_point_a + static_cast<double>(delta_point_a_f_ref * scs_to_khz(scs) * KHZ_TO_HZ);
}

unsigned srsran::band_helper::get_n_rbs_from_bw(bs_channel_bandwidth bw, subcarrier_spacing scs, frequency_range fr)
{
  // Search on the table \ref tx_bw_config_fr2 for the BS channel bandwidth and return the N_RB corresponding to SCS.
  if (fr == frequency_range::FR2) {
    // Return an invalid value in case the input parameters are not valid.
    if ((scs < subcarrier_spacing::kHz60) || (scs > subcarrier_spacing::kHz120)) {
      return 0;
    }

    for (const auto& bw_idx : tx_bw_config_fr2) {
      if (bw_idx.bw == bw) {
        if (scs == subcarrier_spacing::kHz60) {
          return bw_idx.n_rb_60kHz;
        }
        return bw_idx.n_rb_120kHz;
      }
    }
  }

  // Return an invalid value in case the input parameters are not valid.
  if (scs > subcarrier_spacing::kHz60) {
    return 0;
  }

  // Search on the table \ref tx_bw_config_fr1 for the BS channel bandwidth and return the N_RB corresponding to SCS.
  for (const auto& bw_idx : tx_bw_config_fr1) {
    if (bw_idx.bw == bw) {
      if (scs == subcarrier_spacing::kHz15) {
        return bw_idx.n_rb_15kHz;
      }
      if (scs == subcarrier_spacing::kHz30) {
        return bw_idx.n_rb_30kHz;
      }
      return bw_idx.n_rb_60kHz;
    }
  }

  return 0;
}

min_channel_bandwidth srsran::band_helper::get_min_channel_bw(nr_band nr_band, subcarrier_spacing scs)
{
  switch (nr_band) {
    case nr_band::n1:
    case nr_band::n2:
    case nr_band::n3:
    case nr_band::n7:
    case nr_band::n24:
    case nr_band::n25:
    case nr_band::n34:
    case nr_band::n38:
    case nr_band::n39:
    case nr_band::n40:
    case nr_band::n41:
    case nr_band::n48:
    case nr_band::n50:
    case nr_band::n53:
    case nr_band::n65:
    case nr_band::n66:
    case nr_band::n70:
    case nr_band::n74:
    case nr_band::n75:
    case nr_band::n80:
    case nr_band::n84:
    case nr_band::n86:
    case nr_band::n90:
    case nr_band::n95:
    case nr_band::n97:
    case nr_band::n98:
    case nr_band::n99: {
      if (scs == subcarrier_spacing::kHz15) {
        return min_channel_bandwidth::MHz5;
      }
      if (scs == subcarrier_spacing::kHz30 or scs == subcarrier_spacing::kHz60) {
        return min_channel_bandwidth::MHz10;
      }
      return min_channel_bandwidth::invalid;
    }
    case nr_band::n5:
    case nr_band::n8:
    case nr_band::n12:
    case nr_band::n13:
    case nr_band::n14:
    case nr_band::n18:
    case nr_band::n20:
    case nr_band::n26:
    case nr_band::n28:
    case nr_band::n29:
    case nr_band::n30:
    case nr_band::n67:
    case nr_band::n71:
    case nr_band::n81:
    case nr_band::n82:
    case nr_band::n83:
    case nr_band::n85:
    case nr_band::n89:
    case nr_band::n92:
    case nr_band::n94:
    case nr_band::n101: {
      if (scs == subcarrier_spacing::kHz15) {
        return min_channel_bandwidth::MHz5;
      }
      if (scs == subcarrier_spacing::kHz30) {
        return min_channel_bandwidth::MHz10;
      }
      return min_channel_bandwidth::invalid;
    }
    case nr_band::n46:
    case nr_band::n77:
    case nr_band::n78:
    case nr_band::n79: {
      if (scs <= subcarrier_spacing::kHz60) {
        return min_channel_bandwidth::MHz10;
      }
      return min_channel_bandwidth::invalid;
    }
    case nr_band::n51:
    case nr_band::n76:
    case nr_band::n91:
    case nr_band::n93:
    case nr_band::n100: {
      if (scs == subcarrier_spacing::kHz15) {
        return min_channel_bandwidth::MHz5;
      }
      return min_channel_bandwidth::invalid;
    }
    case nr_band::n96:
    case nr_band::n102:
    case nr_band::n104: {
      if (scs <= subcarrier_spacing::kHz60) {
        return min_channel_bandwidth::MHz20;
      }
      return min_channel_bandwidth::invalid;
    }
    case nr_band::n255:
    case nr_band::n256: {
      if (scs == subcarrier_spacing::kHz15) {
        return min_channel_bandwidth::MHz5;
      }
      if (scs == subcarrier_spacing::kHz30) {
        return min_channel_bandwidth::MHz10;
      }
      return min_channel_bandwidth::invalid;
    }
    case nr_band::n257:
    case nr_band::n258:
    case nr_band::n259:
    case nr_band::n260:
    case nr_band::n261:
      if ((scs == subcarrier_spacing::kHz60) || (scs == subcarrier_spacing::kHz120)) {
        return min_channel_bandwidth::MHz50;
      }
      return min_channel_bandwidth::invalid;
    default:
      return min_channel_bandwidth::invalid;
  }
  return min_channel_bandwidth::invalid;
}

/// Compute the maximum value of row index of Tables 13-[1-10], TS 38.213 that can be addressed for a specific
/// configuration.
static unsigned get_max_coreset0_index(nr_band band, subcarrier_spacing scs_common, subcarrier_spacing scs_ssb)
{
  // Row indexes for frequency range 2.
  if (get_freq_range(band) == frequency_range::FR2) {
    // TS 38.213 Table 13-7.
    if (scs_ssb == subcarrier_spacing::kHz120 and scs_common == subcarrier_spacing::kHz60) {
      return 11;
    }
    // TS 38.213 Table 13-8.
    if (scs_ssb == subcarrier_spacing::kHz120 and scs_common == subcarrier_spacing::kHz120) {
      return 7;
    }
    // TS 38.213 Table 13-9.
    if (scs_ssb == subcarrier_spacing::kHz240 and scs_common == subcarrier_spacing::kHz60) {
      return 3;
    }
    // TS 38.213 Table 13-10.
    if (scs_ssb == subcarrier_spacing::kHz240 and scs_common == subcarrier_spacing::kHz120) {
      return 7;
    }
    // Other combinations are not supported.
    return 0;
  }

  const min_channel_bandwidth min_channel_bw         = band_helper::get_min_channel_bw(band, scs_common);
  const bool                  is_for_shared_spectrum = band_helper::is_band_for_shared_spectrum(band);

  // As per TS 38.101, Table 5.2-1, band n79 and n104 use Table 13-5 and Table 13-6, regardless of the minimum
  // channel BW.
  const bool is_40mhz_min_ch_bw_equivalent = band_helper::is_band_40mhz_min_ch_bw_equivalent(band);

  // Check first if the band is for shared spectrum channel access, as per Table 5.2-1, TS 38.104, ver. 17.8.0 and
  // Section 13, TS 38.213, ver.17.5.0.
  if (is_for_shared_spectrum) {
    if ((scs_ssb == subcarrier_spacing::kHz15 and scs_common == subcarrier_spacing::kHz15) or
        (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz30)) {
      return 7;
    }
    return 0;
  }

  // Next, continue with the bands with is_40mhz_min_ch_bw_equivalent == true, as these use Table 13-5 and Table 13-6,
  // regardless of the minimum channel BW.
  if (min_channel_bw == min_channel_bandwidth::MHz40 or is_40mhz_min_ch_bw_equivalent) {
    if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz15) {
      return 8;
    }
    if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz30) {
      return 9;
    }
  }
  // Finally, process the bands with minimum channel BW 5MHz or 10MHz.
  else if (min_channel_bw == min_channel_bandwidth::MHz5 || min_channel_bw == min_channel_bandwidth::MHz10) {
    if (scs_ssb == subcarrier_spacing::kHz15 and scs_common == subcarrier_spacing::kHz15) {
      return 14;
    }
    if (scs_ssb == subcarrier_spacing::kHz15 and scs_common == subcarrier_spacing::kHz30) {
      return 13;
    }
    if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz15) {
      return 8;
    }
    if (scs_ssb == subcarrier_spacing::kHz30 and scs_common == subcarrier_spacing::kHz30) {
      return 15;
    }
  }
  return 0;
}

/// \brief Computes the CRB index where the first SSB's subcarrier is located.
static unsigned get_ssb_crb_0(subcarrier_spacing scs_common, ssb_offset_to_pointA offset_to_point_A)
{
  unsigned denominator = pow2(to_numerology_value(scs_common) - to_numerology_value(get_ssb_ref_scs(scs_common)));
  return offset_to_point_A.to_uint() / denominator;
}

/// \brief Computes the CRBs (based on SCS_common) that intersect with the SSB's PRBs.
static interval<unsigned> get_ssb_crbs(subcarrier_spacing    scs_common,
                                       subcarrier_spacing    scs_ssb,
                                       ssb_offset_to_pointA  offset_to_point_A,
                                       ssb_subcarrier_offset k_ssb)
{
  // Space occupied by SSB in RBs, using SCScommon as reference.
  const unsigned ssb_nof_crbs = NOF_SSB_PRBS * scs_to_khz(scs_ssb) / scs_to_khz(scs_common);

  // Add a CRB to the SSB occupancy if k_ssb > 0; this is because the SSB is not aligned with the first CRB's first
  // subcarrier and will terminate in an extra CRB.
  const unsigned additional_crb = k_ssb > 0 ? 1 : 0;
  const unsigned ssb_crb_0      = get_ssb_crb_0(scs_common, offset_to_point_A);
  return interval<unsigned>{ssb_crb_0, ssb_crb_0 + ssb_nof_crbs + additional_crb};
}

std::optional<ssb_coreset0_freq_location>
srsran::band_helper::get_ssb_coreset0_freq_location(unsigned           dl_arfcn,
                                                    nr_band            band,
                                                    unsigned           n_rbs,
                                                    subcarrier_spacing scs_common,
                                                    subcarrier_spacing scs_ssb,
                                                    uint8_t            ss0_idx,
                                                    uint8_t            max_coreset0_duration)
{
  std::optional<ssb_coreset0_freq_location> result;

  // Get f_ref, point_A from dl_f_ref_arfcn, band and bandwidth.
  ssb_freq_position_generator du_cfg{dl_arfcn, band, n_rbs, scs_common, scs_ssb};

  // Iterate over different SSB candidates and select the valid CORESET#0 index with the narrowest bandwidth.
  // NOTE: CORESET#0 with least bandwidth is chosen so that nof. CCEs monitored in UE dedicated CORESET (used for
  // scheduling C-RNTI DCIs) per slot is much higher than CORESET#0. Hence, allowing more UEs to be scheduled per slot.
  unsigned          min_rbs                        = MAX_NOF_PRBS;
  unsigned          max_non_intersecting_cset0_rbs = 0;
  unsigned          max_duration                   = 0;
  ssb_freq_location ssb                            = du_cfg.get_next_ssb_location();
  while (ssb.is_valid) {
    // Iterate over the searchSpace0_indices and corresponding configurations.
    std::optional<unsigned> cset0_idx = get_coreset0_index(band,
                                                           n_rbs,
                                                           scs_common,
                                                           scs_ssb,
                                                           ssb.offset_to_point_A,
                                                           ssb.k_ssb,
                                                           du_cfg.get_ssb_first_symbol(),
                                                           ss0_idx,
                                                           max_coreset0_duration);

    if (cset0_idx.has_value()) {
      const unsigned nof_avail_cset0_rbs = get_nof_coreset0_rbs_not_intersecting_ssb(
          cset0_idx.value(), band, scs_common, scs_ssb, ssb.offset_to_point_A, ssb.k_ssb);

      const pdcch_type0_css_coreset_description cset0_desc =
          pdcch_type0_css_coreset_get(band, scs_ssb, scs_common, *cset0_idx, ssb.k_ssb.to_uint());

      // If the number of non-intersecting CORESET#0 RBs is the highest so far for the least nof. CORESET#0 RBs and
      // largest CORESET#0 duration less than max_coreset0_duration, save result.
      if (nof_avail_cset0_rbs > max_non_intersecting_cset0_rbs and cset0_desc.nof_rb_coreset < min_rbs and
          cset0_desc.nof_symb_coreset > max_duration) {
        max_non_intersecting_cset0_rbs = nof_avail_cset0_rbs;
        min_rbs                        = cset0_desc.nof_rb_coreset;
        max_duration                   = cset0_desc.nof_symb_coreset;

        result.emplace();
        result->offset_to_point_A = ssb.offset_to_point_A;
        result->k_ssb             = ssb.k_ssb;
        result->coreset0_idx      = cset0_idx.value();
        result->searchspace0_idx  = ss0_idx;
        result->ssb_arfcn         = freq_to_nr_arfcn(ssb.ss_ref);
      }
    }

    ssb = du_cfg.get_next_ssb_location();
  }

  return result;
}

std::optional<ssb_coreset0_freq_location>
srsran::band_helper::get_ssb_coreset0_freq_location_for_cset0_idx(unsigned           dl_arfcn,
                                                                  nr_band            band,
                                                                  unsigned           n_rbs,
                                                                  subcarrier_spacing scs_common,
                                                                  subcarrier_spacing scs_ssb,
                                                                  uint8_t            ss0_idx,
                                                                  unsigned           cset0_idx)
{
  srsran_assert(scs_ssb < subcarrier_spacing::kHz60,
                "Only 15kHz and 30kHz currently supported for SSB subcarrier spacing");

  std::optional<ssb_coreset0_freq_location> result;

  // Get f_ref, point_A from dl_f_ref_arfcn, band and bandwidth.
  ssb_freq_position_generator du_cfg{dl_arfcn, band, n_rbs, scs_common, scs_ssb};

  // Get the maximum Coreset0 index that can be used for the Tables 13-[1-6], TS 38.213.
  const unsigned max_cset0_idx = get_max_coreset0_index(band, scs_common, scs_ssb);

  if (cset0_idx > max_cset0_idx) {
    return result;
  }

  // Iterate over different SSB candidates and verify whether the given CORESET#0 index results in CORESET#0 CRBs not
  // intersecting with the SSB.
  unsigned          max_cset0_rbs = 0;
  ssb_freq_location ssb           = du_cfg.get_next_ssb_location();
  while (ssb.is_valid) {
    // CRB index where the first SSB's subcarrier is located.
    const unsigned crbs_ssb =
        scs_common == subcarrier_spacing::kHz15 ? ssb.offset_to_point_A.to_uint() : ssb.offset_to_point_A.to_uint() / 2;

    auto coreset0_cfg = pdcch_type0_css_coreset_get(band, scs_ssb, scs_common, cset0_idx, ssb.k_ssb.to_uint());
    // CORESET#0 must be within cell bandwidth limits.
    if (coreset0_cfg.nof_rb_coreset > n_rbs) {
      break;
    }

    const pdcch_type0_css_occasion_pattern1_description ss0_config =
        pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
            .is_fr2 = false, .ss_zero_index = ss0_idx, .nof_symb_coreset = coreset0_cfg.nof_symb_coreset});

    // CORESET#0 offset must be between pointA and max CRB.
    const bool coreset0_not_below_pointA = crbs_ssb >= static_cast<unsigned>(coreset0_cfg.offset);
    const bool coreset0_not_above_bw_ub =
        crbs_ssb - static_cast<unsigned>(coreset0_cfg.offset) + coreset0_cfg.nof_rb_coreset <= n_rbs;

    // CORESET#0 number of symbols should not overlap with SSB symbols.
    const bool ss0_not_overlapping_with_ssb_symbols =
        ss0_config.offset == 0 ? coreset0_cfg.nof_symb_coreset <= du_cfg.get_ssb_first_symbol() : true;

    // If it passes all the checks.
    if (coreset0_not_below_pointA and coreset0_not_above_bw_ub and ss0_not_overlapping_with_ssb_symbols) {
      // Compute the number of non-intersecting RBs between CORESET#0 and SSB.
      const unsigned nof_avail_cset0_rbs = get_nof_coreset0_rbs_not_intersecting_ssb(
          cset0_idx, band, scs_common, scs_ssb, ssb.offset_to_point_A, ssb.k_ssb);

      // If the number of non-intersecting CORESET#0 RBs is the highest so far, save result.
      if (nof_avail_cset0_rbs > max_cset0_rbs) {
        max_cset0_rbs = nof_avail_cset0_rbs;

        result.emplace();
        result->offset_to_point_A = ssb.offset_to_point_A;
        result->k_ssb             = ssb.k_ssb;
        result->coreset0_idx      = cset0_idx;
        result->searchspace0_idx  = ss0_idx;
        result->ssb_arfcn         = freq_to_nr_arfcn(ssb.ss_ref);
      }
    }

    ssb = du_cfg.get_next_ssb_location();
  }

  return result;
}

std::optional<unsigned> srsran::band_helper::get_coreset0_index(nr_band                 band,
                                                                unsigned                n_rbs,
                                                                subcarrier_spacing      scs_common,
                                                                subcarrier_spacing      scs_ssb,
                                                                ssb_offset_to_pointA    offset_to_point_A,
                                                                ssb_subcarrier_offset   k_ssb,
                                                                uint8_t                 ssb_first_symbol,
                                                                uint8_t                 ss0_idx,
                                                                std::optional<unsigned> nof_coreset0_symb)
{
  // Determine the frequency range.
  bool is_fr2 = (get_freq_range(band) == frequency_range::FR2);

  // Get the reference subcarrier spacing used for SS/PBCH block frequency positon.
  subcarrier_spacing scs_ref = get_ssb_ref_scs(scs_ssb);

  // CRB index where the first SSB's subcarrier is located.
  const unsigned crbs_ssb =
      offset_to_point_A.to_uint() / pow2(to_numerology_value(scs_common) - to_numerology_value(scs_ref));

  // Get the maximum Coreset0 index that can be used for the Tables 13-[1-6], TS 38.213.
  const unsigned max_cset0_idx = get_max_coreset0_index(band, scs_common, scs_ssb);

  // Iterate over the coreset0_indices and corresponding configurations.
  unsigned                max_nof_avail_rbs = 0;
  std::optional<unsigned> chosen_cset0_idx;
  for (int cset0_idx = static_cast<int>(max_cset0_idx); cset0_idx >= 0; --cset0_idx) {
    auto coreset0_cfg = pdcch_type0_css_coreset_get(band, scs_ssb, scs_common, cset0_idx, k_ssb.to_uint());
    if (max_nof_avail_rbs > coreset0_cfg.nof_rb_coreset) {
      // No point in continuing search for this and lower CORESET#0s.
      break;
    }

    const pdcch_type0_css_occasion_pattern1_description ss0_config =
        pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
            .is_fr2 = is_fr2, .ss_zero_index = ss0_idx, .nof_symb_coreset = coreset0_cfg.nof_symb_coreset});

    // Check if the number of symbols of the CORESET#0 is the one we are searching for.
    const bool coreset0_nof_symb_valid =
        not nof_coreset0_symb.has_value() or coreset0_cfg.nof_symb_coreset == nof_coreset0_symb.value();

    // CORESET#0 offset must be between pointA and max CRB.
    const bool coreset0_not_below_pointA = crbs_ssb >= static_cast<unsigned>(coreset0_cfg.offset);
    const bool coreset0_not_above_bw_ub =
        crbs_ssb - static_cast<unsigned>(coreset0_cfg.offset) + coreset0_cfg.nof_rb_coreset <= n_rbs;

    // CORESET#0 number of symbols should not overlap with SSB symbols.
    const bool ss0_not_overlapping_with_ssb_symbols =
        ss0_config.offset == 0 ? coreset0_cfg.nof_symb_coreset <= ssb_first_symbol : true;

    // If it passes all the checks.
    if (coreset0_nof_symb_valid and coreset0_not_below_pointA and coreset0_not_above_bw_ub and
        ss0_not_overlapping_with_ssb_symbols) {
      // Compute the number of non-intersecting RBs between CORESET#0 and SSB.
      const unsigned nof_avail_rbs =
          get_nof_coreset0_rbs_not_intersecting_ssb(cset0_idx, band, scs_common, scs_ssb, offset_to_point_A, k_ssb);

      // If it is the best candidate so far, store it and try the next CORESET#0 index.
      if (nof_avail_rbs > max_nof_avail_rbs) {
        max_nof_avail_rbs = nof_avail_rbs;
        chosen_cset0_idx  = cset0_idx;
      }
    }
  }
  return chosen_cset0_idx;
}

unsigned srsran::band_helper::get_nof_coreset0_rbs_not_intersecting_ssb(unsigned              cset0_idx,
                                                                        nr_band               band,
                                                                        subcarrier_spacing    scs_common,
                                                                        subcarrier_spacing    scs_ssb,
                                                                        ssb_offset_to_pointA  offset_to_point_A,
                                                                        ssb_subcarrier_offset k_ssb)
{
  // Coreset0 configuration for the provided CORESET#0 index.
  auto cset0_cfg = pdcch_type0_css_coreset_get(band, scs_ssb, scs_common, cset0_idx, k_ssb.to_uint());

  const interval<unsigned> ssb_prbs   = get_ssb_crbs(scs_common, scs_ssb, offset_to_point_A, k_ssb);
  const unsigned           crb_ssb_0  = get_ssb_crb_0(scs_common, offset_to_point_A.to_uint());
  interval<unsigned>       cset0_prbs = {crb_ssb_0 - cset0_cfg.offset,
                                         crb_ssb_0 - cset0_cfg.offset + cset0_cfg.nof_rb_coreset};
  return cset0_cfg.nof_rb_coreset - cset0_prbs.intersect(ssb_prbs).length();
}

n_ta_offset srsran::band_helper::get_ta_offset(nr_band band)
{
  return get_ta_offset(get_freq_range(band));
}

n_ta_offset srsran::band_helper::get_ta_offset(frequency_range freq_range)
{
  if (freq_range == frequency_range::FR1) {
    // Assume no LTE-NR coexistence.
    return n_ta_offset::n25600;
  }
  return n_ta_offset::n13792;
}

std::optional<unsigned> srsran::band_helper::get_ssb_arfcn(unsigned              dl_arfcn,
                                                           nr_band               band,
                                                           unsigned              n_rbs,
                                                           subcarrier_spacing    scs_common,
                                                           subcarrier_spacing    scs_ssb,
                                                           ssb_offset_to_pointA  offset_to_point_A,
                                                           ssb_subcarrier_offset k_ssb)
{
  // Get f_ref, point_A from dl_f_ref_arfcn, band and bandwidth.
  ssb_freq_position_generator du_cfg{dl_arfcn, band, n_rbs, scs_common, scs_ssb};
  ssb_freq_location           ssb = du_cfg.get_next_ssb_location();
  while (ssb.is_valid) {
    if (ssb.offset_to_point_A == offset_to_point_A and ssb.k_ssb == k_ssb) {
      return freq_to_nr_arfcn(ssb.ss_ref);
    }
    ssb = du_cfg.get_next_ssb_location();
  }
  return {};
}

error_type<std::string> srsran::band_helper::is_ssb_arfcn_valid_given_band(uint32_t             ssb_arfcn,
                                                                           nr_band              band,
                                                                           subcarrier_spacing   ssb_scs,
                                                                           bs_channel_bandwidth bw)
{
  // Convert the ARFCN to GSCN.
  std::optional<unsigned> gscn = band_helper::get_gscn_from_ss_ref(nr_arfcn_to_freq(ssb_arfcn));
  if (not gscn.has_value()) {
    return make_unexpected(fmt::format(
        "GSCN {} is not valid for band {} with SSB SCS {}", gscn, fmt::underlying(band), fmt::underlying(ssb_scs)));
  }
  // If the GCSN exists, check if it is a valid one.
  return band_helper::is_gscn_valid_given_band(gscn.value(), band, ssb_scs, bw);
}
