/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/duplex_mode.h"
#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/ran/subcarrier_spacing.h"

using namespace srsgnb;

using namespace band_helper;

// NR operating band and DL ARFCN lower-bound and upper-bound. See Table 5.4.2.3-1 in TS 38.104.
struct nr_band {
  uint16_t band;
  // DL ARFCN LB.
  uint32_t dl_nref_first;
  // DL ARFCN UB.
  uint32_t dl_nref_last;
};

// From Table 5.4.2.3-1 in TS 38.104, this is the number of NR FR1 bands that has a DL allocated band (FDD, TDD or SDL).
// NOTE: Band 41 has two different Freq raster, we only consider raster 15kHz.
const uint32_t nof_nr_DL_bands_fr1 = 26;

// Table with NR operating FR1 band and related DL ARFCN lower-bound and upper-bound. See Table 5.4.2.3-1 in TS 38.104.
// NOTE: It only includes FDD, TDD, and SDL bands.
// NOTE: Band 2 is a subset of band 25
static constexpr std::array<nr_band, nof_nr_DL_bands_fr1> nr_band_table_fr1 = {{
    // clang-format off
    {1,  422000, 434000},
    // Band 2 is a subset of band 25.
    {2,  386000, 398000},
    {3,  361000, 376000},
    {5,  173800, 178800},
    {7,  524000, 538000},
    {8,  185000, 192000},
    {12, 145800, 149200},
    {20, 158200, 164200},
    {25, 386000, 399000},
    {28, 151600, 160600},
    {34, 402000, 405000},
    {38, 514000, 524000},
    {39, 376000, 384000},
    {40, 460000, 480000},
    {41, 499200, 537999},
    {50, 286400, 303400},
    {51, 285400, 286400},
    {66, 422000, 440000},
    {70, 399000, 404000},
    {71, 123400, 130400},
    {74, 295000, 303600},
    {75, 286400, 303400},
    {76, 285400, 286400},
    {77, 620000, 680000},
    {78, 620000, 653333},
    {79, 693334, 733333},
    // clang-format on
}};

// NR operating band in FR1 with related Duplex Mode. See TS 38.101-1 Table 5.2-1.
struct nr_operating_band {
  uint16_t    band;
  duplex_mode duplex;
};
static const uint32_t nof_nr_operating_band_fr1                                                  = 32;
static constexpr std::array<nr_operating_band, nof_nr_operating_band_fr1> nr_operating_bands_fr1 = {{
    // clang-format off
    {1,  duplex_mode::FDD},
    {2,  duplex_mode::FDD},
    {3,  duplex_mode::FDD},
    {5,  duplex_mode::FDD},
    {7,  duplex_mode::FDD},
    {8,  duplex_mode::FDD},
    {12, duplex_mode::FDD},
    {20, duplex_mode::FDD},
    {25, duplex_mode::FDD},
    {28, duplex_mode::FDD},
    {34, duplex_mode::TDD},
    {38, duplex_mode::TDD},
    {39, duplex_mode::TDD},
    {40, duplex_mode::TDD},
    {41, duplex_mode::TDD},
    {50, duplex_mode::TDD},
    {51, duplex_mode::TDD},
    {66, duplex_mode::FDD},
    {70, duplex_mode::FDD},
    {71, duplex_mode::FDD},
    {74, duplex_mode::FDD},
    {75, duplex_mode::TDD},
    {76, duplex_mode::TDD},
    {77, duplex_mode::TDD},
    {78, duplex_mode::TDD},
    {79, duplex_mode::TDD},
    {80, duplex_mode::SUL},
    {81, duplex_mode::SUL},
    {82, duplex_mode::SUL},
    {83, duplex_mode::SUL},
    {84, duplex_mode::SUL},
    {86, duplex_mode::SUL}
    // clang-format on
}};

// NR operating band in FR1 with SSB Subcarrier Spacing and SSB pattern case. See TS 38.101-1 Table 5.2-1.
struct nr_band_ssb_scs_case {
  uint16_t           band;
  subcarrier_spacing scs;
  ssb_pattern_case   pattern;
};
static const uint32_t nof_nr_ssb_bands_fr1                                                             = 32;
static constexpr std::array<nr_band_ssb_scs_case, nof_nr_ssb_bands_fr1> nr_ssb_band_scs_case_table_fr1 = {{
    // clang-format off
    {1,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {2,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {3,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {5,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {5,  subcarrier_spacing::kHz30, ssb_pattern_case::B},
    {7,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {8,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {12, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {20, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {25, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {28, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {34, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {34, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {38, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {38, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {39, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {39, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {40, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {41, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {41, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {50, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {51, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {66, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {66, subcarrier_spacing::kHz30, ssb_pattern_case::B},
    {70, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {71, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {74, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {75, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {76, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {77, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {78, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {79, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    // clang-format on
}};

uint16_t srsgnb::band_helper::get_band_from_dl_arfcn(uint32_t arfcn)
{
  for (const nr_band& band : nr_band_table_fr1) {
    // Check given ARFCN is between the first and last possible ARFCN.
    if (arfcn >= band.dl_nref_first and arfcn <= band.dl_nref_last) {
      return band.band;
    }
  }
  return UINT16_MAX;
}

ssb_pattern_case srsgnb::band_helper::get_ssb_pattern(uint16_t band, subcarrier_spacing scs)
{
  // Look for the given band and SCS.
  for (const nr_band_ssb_scs_case& ssb_scs_case : nr_ssb_band_scs_case_table_fr1) {
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

duplex_mode srsgnb::band_helper::get_duplex_mode(uint16_t band)
{
  // Look for the given band.
  for (const nr_operating_band& b : nr_operating_bands_fr1) {
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

bool srsgnb::band_helper::is_paired_spectrum(uint16_t band)
{
  duplex_mode mode = get_duplex_mode(band);
  srsran_assert(mode < duplex_mode::INVALID, "Returned invalid duplex MODE");
  return mode == duplex_mode::FDD ? true : false;
}