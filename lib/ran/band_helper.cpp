/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/band_helper.h"
#include "ssb_freq_position_generator.h"
#include "srsgnb/adt/interval.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/duplex_mode.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsgnb/ran/subcarrier_spacing.h"

using namespace srsran;

using namespace band_helper;

// Possible values of delta f_raster in Table 5.4.2.3-1 and Table 5.4.2.3-2
enum class delta_freq_raster {
  DEFAULT = 0, // for bands with 2 possible values for delta_f_raster (e.g. 15 and 30 kHz), the lower is chosen
  kHz15,
  kHz30,
  kHz60,
  kHz100,
  kHz120
};

// NR operating band and DL ARFCN lower-bound and upper-bound. See Table 5.4.2.3-1 in TS 38.104.
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

// From Table 5.4.2.3-1 in TS 38.104, this is the number of NR FR1 bands that has a DL allocated band (FDD, TDD or SDL).
// NOTE: Band 41 has two different Freq raster, we only consider raster 15kHz.
const uint32_t nof_nr_DL_bands_fr1 = 36;

// Table with NR operating FR1 band and related ARFCN lower-bound and upper-bound. See Table 5.4.2.3-1 in TS 38.104.
// NOTE: It only includes FDD, TDD, and SDL bands.
// NOTE: Band 2 is a subset of band 25
static constexpr std::array<nr_band_raster, nof_nr_DL_bands_fr1> nr_band_table_fr1 = {{
    // clang-format off
    {nr_band::n1,  delta_freq_raster::kHz100, 384000, 20, 396000, 422000, 20, 434000},
    {nr_band::n2,  delta_freq_raster::kHz100, 370000, 20, 382000, 386000, 20, 398000},
    {nr_band::n3,  delta_freq_raster::kHz100, 342000, 20, 357000, 361000, 20, 376000},
    {nr_band::n5,  delta_freq_raster::kHz100, 164800, 20, 169800, 173800, 20, 178800},
    {nr_band::n7,  delta_freq_raster::kHz100, 500000, 20, 514000, 524000, 20, 538000},
    {nr_band::n8,  delta_freq_raster::kHz100, 176000, 20, 183000, 185000, 20, 192000},
    {nr_band::n12, delta_freq_raster::kHz100, 139800, 20, 143200, 145800, 20, 149200},
    {nr_band::n20, delta_freq_raster::kHz100, 166400, 20, 172400, 158200, 20, 164200},
    {nr_band::n25, delta_freq_raster::kHz100, 370000, 20, 383000, 386000, 20, 399000},
    {nr_band::n28, delta_freq_raster::kHz100, 140600, 20, 149600, 151600, 20, 160600},
    {nr_band::n34, delta_freq_raster::kHz100, 402000, 20, 405000, 402000, 20, 405000},
    {nr_band::n38, delta_freq_raster::kHz100, 514000, 20, 524000, 514000, 20, 524000},
    {nr_band::n39, delta_freq_raster::kHz100, 376000, 20, 384000, 376000, 20, 384000},
    {nr_band::n40, delta_freq_raster::kHz100, 460000, 20, 480000, 460000, 20, 480000},
    {nr_band::n41, delta_freq_raster::kHz15,  499200,  3, 537999, 499200,  3, 537999},
    {nr_band::n41, delta_freq_raster::kHz30,  499200,  6, 537996, 499200,  6, 537996},
    {nr_band::n50, delta_freq_raster::kHz100, 286400, 20, 303400, 286400, 20, 303400},
    {nr_band::n51, delta_freq_raster::kHz100, 285400, 20, 286400, 285400, 20, 286400},
    {nr_band::n66, delta_freq_raster::kHz100, 342000, 20, 356000, 422000, 20, 440000},
    {nr_band::n70, delta_freq_raster::kHz100, 339000, 20, 342000, 399000, 20, 404000},
    {nr_band::n71, delta_freq_raster::kHz100, 132600, 20, 139600, 123400, 20, 130400},
    {nr_band::n74, delta_freq_raster::kHz100, 285400, 20, 294000, 295000, 20, 303600},
    {nr_band::n75, delta_freq_raster::kHz100,      0,  0,      0, 286400, 20, 303400},
    {nr_band::n76, delta_freq_raster::kHz100,      0,  0,      0, 285400, 20, 286400},
    {nr_band::n77, delta_freq_raster::kHz15,  620000,  1, 680000, 620000,  1, 680000},
    {nr_band::n77, delta_freq_raster::kHz30,  620000,  2, 680000, 620000,  2, 680000},
    {nr_band::n78, delta_freq_raster::kHz15,  620000,  1, 653333, 620000,  1, 653333},
    {nr_band::n78, delta_freq_raster::kHz30,  620000,  2, 653332, 620000,  2, 653332},
    {nr_band::n79, delta_freq_raster::kHz15,  693334,  2, 733333, 693334,  2, 733333},
    {nr_band::n79, delta_freq_raster::kHz30,  693334,  2, 733332, 693334,  2, 733332},
    {nr_band::n80, delta_freq_raster::kHz100, 342000,  20, 357000,     0,  0,      0},
    {nr_band::n81, delta_freq_raster::kHz100, 176000,  20, 183000,     0,  0,      0},
    {nr_band::n82, delta_freq_raster::kHz100, 166400,  20, 172400,     0,  0,      0},
    {nr_band::n83, delta_freq_raster::kHz100, 140600,  20, 149600,     0,  0,      0},
    {nr_band::n84, delta_freq_raster::kHz100, 384000,  20, 396000,     0,  0,      0},
    {nr_band::n86, delta_freq_raster::kHz100, 342000,  20, 356000,     0,  0,      0}
    // clang-format on
}};

// NR operating band in FR1 with related Duplex Mode. See TS 38.101-1 Table 5.2-1.
struct nr_operating_band {
  nr_band     band;
  duplex_mode duplex;
};
static const uint32_t                                                     nof_nr_operating_band_fr1 = 32;
static constexpr std::array<nr_operating_band, nof_nr_operating_band_fr1> nr_operating_bands_fr1    = {{
       // clang-format off
    {nr_band::n1,  duplex_mode::FDD},
    {nr_band::n2,  duplex_mode::FDD},
    {nr_band::n3,  duplex_mode::FDD},
    {nr_band::n5,  duplex_mode::FDD},
    {nr_band::n7,  duplex_mode::FDD},
    {nr_band::n8,  duplex_mode::FDD},
    {nr_band::n12, duplex_mode::FDD},
    {nr_band::n20, duplex_mode::FDD},
    {nr_band::n25, duplex_mode::FDD},
    {nr_band::n28, duplex_mode::FDD},
    {nr_band::n34, duplex_mode::TDD},
    {nr_band::n38, duplex_mode::TDD},
    {nr_band::n39, duplex_mode::TDD},
    {nr_band::n40, duplex_mode::TDD},
    {nr_band::n41, duplex_mode::TDD},
    {nr_band::n50, duplex_mode::TDD},
    {nr_band::n51, duplex_mode::TDD},
    {nr_band::n66, duplex_mode::FDD},
    {nr_band::n70, duplex_mode::FDD},
    {nr_band::n71, duplex_mode::FDD},
    {nr_band::n74, duplex_mode::FDD},
    {nr_band::n75, duplex_mode::TDD},
    {nr_band::n76, duplex_mode::TDD},
    {nr_band::n77, duplex_mode::TDD},
    {nr_band::n78, duplex_mode::TDD},
    {nr_band::n79, duplex_mode::TDD},
    {nr_band::n80, duplex_mode::SUL},
    {nr_band::n81, duplex_mode::SUL},
    {nr_band::n82, duplex_mode::SUL},
    {nr_band::n83, duplex_mode::SUL},
    {nr_band::n84, duplex_mode::SUL},
    {nr_band::n86, duplex_mode::SUL}
    // clang-format on
}};

// NR operating band in FR1 with SSB Subcarrier Spacing and SSB pattern case. See TS 38.101-1 Table 5.2-1.
struct nr_band_ssb_scs_case {
  nr_band            band;
  subcarrier_spacing scs;
  ssb_pattern_case   pattern;
};
static const uint32_t                                                   nof_nr_ssb_bands_fr1           = 32;
static constexpr std::array<nr_band_ssb_scs_case, nof_nr_ssb_bands_fr1> nr_ssb_band_scs_case_table_fr1 = {{
    // clang-format off
    {nr_band::n1,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n2,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n3,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n5,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n5,  subcarrier_spacing::kHz30, ssb_pattern_case::B},
    {nr_band::n7,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n8,  subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n12, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n20, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n25, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n28, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n34, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n34, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n38, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n38, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n39, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n39, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n40, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n41, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n41, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n50, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n51, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n66, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n66, subcarrier_spacing::kHz30, ssb_pattern_case::B},
    {nr_band::n70, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n71, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n74, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n75, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n76, subcarrier_spacing::kHz15, ssb_pattern_case::A},
    {nr_band::n77, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n78, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    {nr_band::n79, subcarrier_spacing::kHz30, ssb_pattern_case::C},
    // clang-format on
}};

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

static const uint32_t                            max_nr_arfcn = 3279165;
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

struct n_rb_per_scs {
  bs_channel_bandwidth_fr1 bw;
  unsigned                 n_rb_15kHz;
  unsigned                 n_rb_30kHz;
  unsigned                 n_rb_60kHz;
};

// This implements Table 5.3.2-1 in TS 38.104. Value N_RB = 0 represent N/A.
static std::array<n_rb_per_scs, 13> tx_bw_config_fr1 = {{
    // clang-format off
    // BW = 5MHz.
    {bs_channel_bandwidth_fr1::MHz5, 25, 11, 0},
    // BW = 10MHz.
    {bs_channel_bandwidth_fr1::MHz10, 52, 24, 11},
    // BW = 15MHz.
    {bs_channel_bandwidth_fr1::MHz15, 79, 38, 18},
    // BW = 20MHz.
    {bs_channel_bandwidth_fr1::MHz20, 106, 51, 24},
    // BW = 25MHz.
    {bs_channel_bandwidth_fr1::MHz25, 133, 65, 31},
    // BW = 30MHz.
    {bs_channel_bandwidth_fr1::MHz30, 160, 78, 38},
    // BW = 40MHz.
    {bs_channel_bandwidth_fr1::MHz40, 216, 106, 51},
    // BW = 50MHz.
    {bs_channel_bandwidth_fr1::MHz50, 270, 133, 65},
    // BW = 60MHz.
    {bs_channel_bandwidth_fr1::MHz60, 0, 162, 79},
    // BW = 70MHz.
    {bs_channel_bandwidth_fr1::MHz70, 0, 189, 93},
    // BW = 80MHz.
    {bs_channel_bandwidth_fr1::MHz80, 0, 217, 107},
    // BW = 90MHz.
    {bs_channel_bandwidth_fr1::MHz90, 0, 245, 121},
    // BW = 100MHz.
    {bs_channel_bandwidth_fr1::MHz100, 0, 273, 135}
    // clang-format on
}};

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
  for (const nr_raster_params& fr : nr_fr_params) {
    if (fr == raster) {
      return true;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

nr_band srsran::band_helper::get_band_from_dl_arfcn(uint32_t arfcn)
{
  for (const nr_band_raster& band : nr_band_table_fr1) {
    // Check given ARFCN is between the first and last possible ARFCN.
    if (arfcn >= band.dl_nref_first and arfcn <= band.dl_nref_last) {
      return band.band;
    }
  }
  return nr_band::invalid;
}

bool srsran::band_helper::is_dl_arfcn_valid_given_band(nr_band band, uint32_t arfcn)
{
  for (const nr_band_raster& raster_band : nr_band_table_fr1) {
    if (raster_band.band == band) {
      return arfcn >= raster_band.dl_nref_first and arfcn <= raster_band.dl_nref_last;
    }
  }
  return false;
}

uint32_t srsran::band_helper::get_ul_arfcn_from_dl_arfcn(uint32_t dl_arfcn)
{
  // NOTE: The procedure implemented in this function is implementation-defined.

  // Return same ARFCN for TDD bands.
  if (get_duplex_mode(get_band_from_dl_arfcn(dl_arfcn)) == duplex_mode::TDD) {
    return dl_arfcn;
  }

  // Derive UL ARFCN for FDD bands.
  for (const auto& band : nr_band_table_fr1) {
    if (band.band == get_band_from_dl_arfcn(dl_arfcn)) {
      uint32_t offset = (dl_arfcn - band.dl_nref_first) / band.dl_nref_step;
      return (band.ul_nref_first + offset * band.ul_nref_step);
    }
  }

  return 0;
}

double srsran::band_helper::nr_arfcn_to_freq(uint32_t nr_arfcn)
{
  nr_raster_params params = get_raster_params(nr_arfcn);
  if (not is_valid_raster_param(params)) {
    return 0.0;
  }
  return (params.F_REF_Offs_MHz * 1e6 + params.delta_F_global_kHz * (nr_arfcn - params.N_REF_Offs) * 1e3);
}

uint32_t srsran::band_helper::freq_to_nr_arfcn(double freq)
{
  nr_raster_params params = get_raster_params(freq);
  if (not is_valid_raster_param(params)) {
    return 0;
  }
  return static_cast<uint32_t>(((freq - params.F_REF_Offs_MHz * 1e6) / 1e3 / params.delta_F_global_kHz) +
                               params.N_REF_Offs);
}

ssb_pattern_case srsran::band_helper::get_ssb_pattern(nr_band band, subcarrier_spacing scs)
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

subcarrier_spacing srsran::band_helper::get_most_suitable_ssb_scs(nr_band band, subcarrier_spacing scs_common)
{
  subcarrier_spacing lowest_scs = subcarrier_spacing::invalid;

  // Look for the given band and SCS
  for (const nr_band_ssb_scs_case& ssb_case : nr_ssb_band_scs_case_table_fr1) {
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

bool srsran::band_helper::is_paired_spectrum(nr_band band)
{
  duplex_mode mode = get_duplex_mode(band);
  srsgnb_assert(mode < duplex_mode::INVALID, "Returned invalid duplex MODE");
  return mode == duplex_mode::FDD;
}

frequency_range srsran::band_helper::get_freq_range(nr_band band)
{
  srsgnb_assert(band != nr_band::invalid, "Band must be a valid NR band.");
  return band <= nr_band::n86 ? frequency_range::FR1 : frequency_range::FR2;
}

double srsran::band_helper::get_abs_freq_point_a_from_center_freq(uint32_t nof_prb, double center_freq)
{
  constexpr static unsigned NRE = 12;

  // For FR1 unit of resources blocks for freq calc is always 180kHz regardless for actual SCS of carrier.
  // TODO: add offset_to_carrier.
  return center_freq - (nof_prb / 2 * scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ * NRE);
}

uint32_t srsran::band_helper::get_abs_freq_point_a_arfcn(uint32_t nof_prb, uint32_t arfcn)
{
  return freq_to_nr_arfcn(get_abs_freq_point_a_from_center_freq(nof_prb, nr_arfcn_to_freq(arfcn)));
}

double srsran::band_helper::get_center_freq_from_abs_freq_point_a(uint32_t nof_prb, uint32_t freq_point_a_arfcn)
{
  constexpr static unsigned NRE = 12;
  // for FR1 unit of resources blocks for freq calc is always 180kHz regardless for actual SCS of carrier.
  // TODO: add offset_to_carrier
  double abs_freq_point_a_freq = nr_arfcn_to_freq(freq_point_a_arfcn);
  return abs_freq_point_a_freq + (nof_prb / 2 * scs_to_khz(subcarrier_spacing::kHz15) * KHZ_TO_HZ * NRE);
}

double srsran::band_helper::get_abs_freq_point_a_from_f_ref(double f_ref, uint32_t nof_rbs, subcarrier_spacing scs)
{
  // NOTE (i): It is unclear whether the SCS should always be 15kHz for FR1 (\ref get_abs_freq_point_a_from_center_freq
  // and see note).
  // NOTE (ii): TS 38.104, Section 5.4.2.2, reports <em>[...] The mapping must apply to at least one numerology
  // supported by the BS.<\em>. Therefore, the correct SCS to be used in this procedure still needs to determined.

  // Half of the number of subcarriers in a RE.
  constexpr static unsigned NRE_half = 6;

  // The procedure, which is explained in TS 38.104, Section 5.4.2.2, gives the position of f_ref in terms of subcarrier
  // and CRB index, depending on the size of N_RB. Below we compute the value in unit of subcarriers, meaning we don't
  // need to separate the cases of even and odd N_RB.
  unsigned delta_point_a_f_ref = nof_rbs * NRE_half;
  return f_ref - static_cast<double>(delta_point_a_f_ref * scs_to_khz(scs) * KHZ_TO_HZ);
}

double
srsran::band_helper::get_f_ref_from_abs_freq_point_a(double abs_freq_point_a, uint32_t nof_rbs, subcarrier_spacing scs)
{
  // See notes in \ref get_abs_freq_point_a_from_f_ref.

  // Half of the number of subcarriers in a RE.
  constexpr static unsigned NRE_half = 6;
  // The procedure used in this function is the inverse of what explained in TS 38.104, Section 5.4.2.2.
  unsigned delta_point_a_f_ref = nof_rbs * NRE_half;
  return abs_freq_point_a + static_cast<double>(delta_point_a_f_ref * scs_to_khz(scs) * KHZ_TO_HZ);
}

unsigned srsran::band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1 bw, subcarrier_spacing scs, frequency_range fr)
{
  // Return an invalid value in case the input parameters are not valid.
  if (fr != frequency_range::FR1 or scs > subcarrier_spacing::kHz60)
    return 0;

  // Search on the table \ref tx_bw_config_fr1 for the BS channel bandwidth and return the N_RB corresponding to SCS.
  for (unsigned bw_idx = 0; bw_idx < tx_bw_config_fr1.size(); ++bw_idx) {
    if (tx_bw_config_fr1[bw_idx].bw == bw) {
      if (scs == subcarrier_spacing::kHz15) {
        return tx_bw_config_fr1[bw_idx].n_rb_15kHz;
      } else if (scs == subcarrier_spacing::kHz30) {
        return tx_bw_config_fr1[bw_idx].n_rb_30kHz;
      } else {
        return tx_bw_config_fr1[bw_idx].n_rb_60kHz;
      }
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
    case nr_band::n25:
    case nr_band::n34:
    case nr_band::n38:
    case nr_band::n39:
    case nr_band::n40:
    case nr_band::n50:
    case nr_band::n66:
    case nr_band::n70:
    case nr_band::n74:
    case nr_band::n75:
    case nr_band::n80:
    case nr_band::n84:
    case nr_band::n86: {
      if (scs == subcarrier_spacing::kHz15) {
        return min_channel_bandwidth::MHz5;
      } else if (scs == subcarrier_spacing::kHz30 or scs == subcarrier_spacing::kHz60) {
        return min_channel_bandwidth::MHz10;
      } else {
        return min_channel_bandwidth::invalid;
      }
    }
    case nr_band::n5:
    case nr_band::n8:
    case nr_band::n12:
    case nr_band::n20:
    case nr_band::n28:
    case nr_band::n71:
    case nr_band::n81:
    case nr_band::n82:
    case nr_band::n83: {
      if (scs == subcarrier_spacing::kHz15) {
        return min_channel_bandwidth::MHz5;
      } else if (scs == subcarrier_spacing::kHz30) {
        return min_channel_bandwidth::MHz10;
      } else {
        return min_channel_bandwidth::invalid;
      }
    }
    case nr_band::n51:
    case nr_band::n76: {
      if (scs == subcarrier_spacing::kHz15) {
        return min_channel_bandwidth::MHz5;
      } else {
        return min_channel_bandwidth::invalid;
      }
    }
    case nr_band::n41:
    case nr_band::n77:
    case nr_band::n78: {
      if (scs <= subcarrier_spacing::kHz60) {
        return min_channel_bandwidth::MHz10;
      } else {
        return min_channel_bandwidth::invalid;
      }
    }
    case nr_band::n79: {
      if (scs <= subcarrier_spacing::kHz60) {
        return min_channel_bandwidth::MHz40;
      } else {
        return min_channel_bandwidth::invalid;
      }
    }
    default:
      return min_channel_bandwidth::invalid;
  }
  return min_channel_bandwidth::invalid;
}

// Compute the maximum value of row index of Table 13-11, TS 38.213 that can be addressed for a specific configuration.
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

// \brief Computes the CRB index where the first SSB's subcarrier is located.
static unsigned get_ssb_crb_0(subcarrier_spacing scs_common, ssb_offset_to_pointA offset_to_point_A)
{
  return scs_common == subcarrier_spacing::kHz15 ? offset_to_point_A.to_uint() : offset_to_point_A.to_uint() / 2;
}

// \brief Computes the CRBs (based on SCS_common) that intersect with the SSB's PRBs.
static interval<unsigned> get_ssb_crbs(subcarrier_spacing    scs_common,
                                       subcarrier_spacing    scs_ssb,
                                       ssb_offset_to_pointA  offset_to_point_A,
                                       ssb_subcarrier_offset k_ssb)
{
  // Space occupied by SSB in RBs, using SCScommon as reference.
  const unsigned ssb_nof_crbs = NOF_SSB_PRBS * scs_to_khz(scs_ssb) / scs_to_khz(scs_common);

  // Add a CRB to the SSB occupancy if k_ssb > 0; this is because the SSB is not aligned with the first CRB's first
  // subcarrier and will terminate in an extra CRB.
  unsigned additional_crb = k_ssb > 0 ? 1 : 0;
  unsigned ssb_crb_0      = get_ssb_crb_0(scs_common, offset_to_point_A);
  return interval<unsigned>{ssb_crb_0, ssb_crb_0 + ssb_nof_crbs + additional_crb};
}

optional<ssb_coreset0_freq_location> srsran::band_helper::get_ssb_coreset0_freq_location(unsigned           dl_arfcn,
                                                                                         nr_band            band,
                                                                                         unsigned           n_rbs,
                                                                                         subcarrier_spacing scs_common,
                                                                                         subcarrier_spacing scs_ssb,
                                                                                         uint8_t            ss0_idx)
{
  srsgnb_assert(band != nr_band::n34 && band != nr_band::n38 && band != nr_band::n39 && band != nr_band::n79,
                "Bands n34, n38, n39 and n79 not currently supported");
  optional<ssb_coreset0_freq_location> result;

  // Get f_ref, point_A from dl_arfcn, band and bandwidth.
  ssb_freq_position_generator du_cfg{dl_arfcn, band, n_rbs, scs_common, scs_ssb};

  // Iterate over different SSB candidates and select the valid CORESET#0 index with the widest bandwidth.
  unsigned          max_cset0_rbs = 0;
  ssb_freq_location ssb           = du_cfg.get_next_ssb_location();
  while (ssb.is_valid) {
    // Iterate over the searchSpace0_indices and corresponding configurations.
    optional<unsigned> cset0_idx = get_coreset0_index(
        band, n_rbs, scs_common, scs_ssb, ssb.offset_to_point_A, ssb.k_ssb, du_cfg.get_ssb_first_symbol(), ss0_idx);

    if (cset0_idx.has_value()) {
      unsigned nof_avail_cset0_rbs = get_nof_coreset0_rbs_not_intersecting_ssb(
          cset0_idx.value(), band, scs_common, scs_ssb, ssb.offset_to_point_A, ssb.k_ssb);

      // If the number of non-intersecting CORESET#0 RBs is the highest so far, save result.
      if (nof_avail_cset0_rbs > max_cset0_rbs) {
        max_cset0_rbs = nof_avail_cset0_rbs;

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

optional<unsigned> srsran::band_helper::get_coreset0_index(nr_band               band,
                                                           unsigned              n_rbs,
                                                           subcarrier_spacing    scs_common,
                                                           subcarrier_spacing    scs_ssb,
                                                           ssb_offset_to_pointA  offset_to_point_A,
                                                           ssb_subcarrier_offset k_ssb,
                                                           uint8_t               ssb_first_symbol,
                                                           uint8_t               ss0_idx,
                                                           optional<unsigned>    nof_coreset0_symb)
{
  min_channel_bandwidth min_ch_bw = band_helper::get_min_channel_bw(band, scs_common);
  // CRB index where the first SSB's subcarrier is located.
  unsigned crbs_ssb =
      scs_common == subcarrier_spacing::kHz15 ? offset_to_point_A.to_uint() : offset_to_point_A.to_uint() / 2;

  // Get the maximum Coreset0 index that can be used for the Tables 13-[1-6], TS 38.213.
  unsigned max_cset0_idx = get_max_coreset0_index(scs_common, scs_ssb, min_ch_bw);

  // Iterate over the coreset0_indices and corresponding configurations.
  unsigned           max_nof_avail_rbs = 0;
  optional<unsigned> chosen_cset0_idx;
  for (int cset0_idx = max_cset0_idx; cset0_idx >= 0; --cset0_idx) {
    auto coreset0_cfg = pdcch_type0_css_coreset_get(min_ch_bw, scs_ssb, scs_common, cset0_idx, k_ssb.to_uint());
    if (max_nof_avail_rbs > coreset0_cfg.nof_rb_coreset) {
      // No point in continuing search for this and lower CORESET#0s.
      break;
    }

    pdcch_type0_css_occasion_pattern1_description ss0_config =
        pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
            .is_fr2 = false, .ss_zero_index = ss0_idx, .nof_symb_coreset = coreset0_cfg.nof_symb_coreset});

    // Check if the number of symbols of the CORESET#0 is the one we are searching for.
    bool coreset0_nof_symb_valid =
        not nof_coreset0_symb.has_value() or coreset0_cfg.nof_symb_coreset == nof_coreset0_symb.value();

    // CORESET#0 offset must be between pointA and max CRB.
    bool coreset0_not_below_pointA = crbs_ssb >= static_cast<unsigned>(coreset0_cfg.offset);
    bool coreset0_not_above_bw_ub =
        crbs_ssb - static_cast<unsigned>(coreset0_cfg.offset) + coreset0_cfg.nof_rb_coreset <= n_rbs;

    // CORESET#0 number of symbols should not overlap with SSB symbols.
    bool ss0_not_overlapping_with_ssb_symbols =
        ss0_config.offset == 0 ? coreset0_cfg.nof_symb_coreset <= ssb_first_symbol : true;

    // If it passes all the checks.
    if (coreset0_nof_symb_valid and coreset0_not_below_pointA and coreset0_not_above_bw_ub and
        ss0_not_overlapping_with_ssb_symbols) {
      // Compute the number of non-intersecting RBs between CORESET#0 and SSB.
      unsigned nof_avail_rbs =
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
  auto cset0_cfg = pdcch_type0_css_coreset_get(
      band_helper::get_min_channel_bw(band, scs_common), scs_ssb, scs_common, cset0_idx, k_ssb.to_uint());

  interval<unsigned> ssb_prbs   = get_ssb_crbs(scs_common, scs_ssb, offset_to_point_A, k_ssb);
  unsigned           crb_ssb_0  = get_ssb_crb_0(scs_common, offset_to_point_A.to_uint());
  interval<unsigned> cset0_prbs = {crb_ssb_0 - cset0_cfg.offset,
                                   crb_ssb_0 - cset0_cfg.offset + cset0_cfg.nof_rb_coreset};
  return cset0_cfg.nof_rb_coreset - cset0_prbs.intersect(ssb_prbs).length();
}
