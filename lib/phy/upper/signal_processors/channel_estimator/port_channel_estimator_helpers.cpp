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

#include "port_channel_estimator_helpers.h"
#include "port_channel_estimator_average_impl.h"
#include "srsran/srsvec/conversion.h"
#include "srsran/srsvec/convolution.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/mean.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/unwrap.h"

#if defined(__AVX2__)
#include <immintrin.h>
#elif defined(__ARM_NEON)
#include <arm_neon.h>
#endif

using namespace srsran;

static const bounded_bitset<NRE> re_pattern_pucch_f2 =
    {false, true, false, false, true, false, false, true, false, false, true, false};
static const bounded_bitset<NRE> re_pattern_pusch_0 =
    {true, false, true, false, true, false, true, false, true, false, true, false};
static const bounded_bitset<NRE> re_pattern_pusch_1 =
    {false, true, false, true, false, true, false, true, false, true, false, true};

/// Coefficients of a raised cosine FIR filter with roll-off 0.2, 3-symbol span, 10 samples per symbol (total 31
/// samples). The filter will be applied to a frequency-domain signal to attenuate time components for t > 1/10 Ts, with
/// Ts the symbol time (recall that the CP is ~1/14 Ts).
static constexpr unsigned                             MAX_FILTER_LENGTH = 31;
static constexpr std::array<float, MAX_FILTER_LENGTH> RC_FILTER         = {
    -0.0641253, -0.0660711, -0.0611526, -0.0485918, -0.0281126, 0.0000000,  0.0348830, 0.0751249,
    0.1188406,  0.1637874,  0.2075139,  0.2475302,  0.2814857,  0.3073415,  0.3235207, 0.3290274,
    0.3235207,  0.3073415,  0.2814857,  0.2475302,  0.2075139,  0.1637874,  0.1188406, 0.0751249,
    0.0348830,  0.0000000,  -0.0281126, -0.0485918, -0.0611526, -0.0660711, -0.0641253};

/// Create an array indexing the virtual pilots.
template <size_t N>
static constexpr std::array<int, N> create_index_array()
{
  std::array<int, port_channel_estimator_average_impl::MAX_V_PILOTS> out;
  int                                                                count = 0;
  for (int& v : out) {
    v = count++;
  }
  return out;
}

static const std::array<int, port_channel_estimator_average_impl::MAX_V_PILOTS> indices =
    create_index_array<port_channel_estimator_average_impl::MAX_V_PILOTS>();

namespace {

/// \brief Provides access to customized raised-cosine filters.
///
/// Besides the FIR coefficients, the class also provides access to a correction term to be applied to the tails of the
/// output signal to compensate for the truncation.
class filter_type
{
public:
  /// \brief Creates a customized raised-cosine filter by resampling and renormalizing the coefficients in RC_FILTER.
  ///
  /// The filter is tuned to work with DM-RS placed on nof_rbs resource blocks every other stride-th resource element.
  filter_type(unsigned nof_rbs, unsigned stride)
  {
    // Span at most 3 RBs.
    nof_rbs = std::min(nof_rbs, 3U);
    // Number of coefficients at full rate.
    unsigned nof_coefs     = nof_rbs * 10 + 1;
    unsigned nof_coefs_out = nof_coefs / 2 / stride;
    // Index of the first coefficient copied from the general filter to the customized one.
    unsigned n_first = MAX_FILTER_LENGTH / 2 - nof_coefs_out * stride;
    // Number of coefficients after downsampling.
    nof_coefs_out = 2 * nof_coefs_out + 1;

    rc_filter       = rc_filter.first(nof_coefs_out);
    tail_correction = tail_correction.first(nof_coefs_out);
    unsigned n      = n_first;
    float    total  = 0;
    for (unsigned i_coef = 0; i_coef != nof_coefs_out; ++i_coef) {
      rc_filter[i_coef] = RC_FILTER[n];
      total += rc_filter[i_coef];
      tail_correction[i_coef] = 1.0F / total;
      n += stride;
    }
    // Normalize the filter so that the sum of its coefficients is 1 and the tail correction coefficients accordingly.
    srsvec::sc_prod(rc_filter, rc_filter, 1 / total);
    srsvec::sc_prod(tail_correction, tail_correction, total);

    tail_correction = tail_correction.subspan(nof_coefs_out / 2, nof_coefs_out / 2);
  }

  // Customized raised-cosine FIR coefficients.
  span<float> rc_filter = {filter_coefs};
  // Tail correction coefficients.
  span<float> tail_correction = {correction_coefs};

private:
  // Auxiliary buffers.
  std::array<float, MAX_FILTER_LENGTH> filter_coefs;
  std::array<float, MAX_FILTER_LENGTH> correction_coefs;
};

} // namespace

/// \brief Computes some virtual pilots to improve estimation at the edges.
///
/// \param[out] out       Computed virtual pilots.
/// \param[in]  in_abs    Modulus of the true pilots used for extrapolation.
/// \param[in]  in_arg    Phase angle of the true pilots used for extrapolation - it is assumed unwrapped.
/// \param[in]  is_start  Direction flag: If true, virtual pilots are computed to precede the true ones. If false,
///                       virtual pilots will follow the true ones.
static void compute_v_pilots(span<cf_t> out, span<const float> in_abs, span<const float> in_arg, bool is_start);

/// Adds virtual pilots at both ends of \c old_pilots (\c old_pilots is assumed a centered subspan of new_pilots).
static void add_v_pilots(span<cf_t> new_pilots, span<const cf_t> old_pilots);

/// \brief Extracts resource elements from an input span.
/// \param[out] out     Extracted resource elements.
/// \param[in]  re_mask Selected resource elements within each PRB.
/// \param[in]  in      Frequency domain resource grid view.
/// \return The view of the remaining unwritten output.
static span<cf_t> extract_re_prb(span<cf_t> out, const bounded_bitset<NRE>& re_mask, span<const cbf16_t> in);

unsigned srsran::extract_layer_hop_rx_pilots(dmrs_symbol_list&                            rx_symbols,
                                             const resource_grid_reader&                  grid,
                                             unsigned                                     port,
                                             const port_channel_estimator::configuration& cfg,
                                             unsigned                                     hop,
                                             unsigned                                     i_layer)
{
  // CDM group the layer belongs to. Note that layers of the same CDM group (i.e., {0, 1} and {2, 3}) share DM-RS
  // resources.
  unsigned i_cdm = i_layer / 2;

  // Select DM-RS pattern.
  const port_channel_estimator::layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

  const crb_bitmap& hop_rb_mask = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;

  // Extract hop PRB positions.
  unsigned i_prb_begin   = hop_rb_mask.find_lowest();
  unsigned i_prb_end     = hop_rb_mask.find_highest() + 1;
  unsigned nof_prb       = hop_rb_mask.count();
  bool     is_contiguous = (i_prb_end - i_prb_begin == nof_prb);

  unsigned symbol_index =
      ((hop == 1) && pattern.hopping_symbol_index.has_value()) ? *pattern.hopping_symbol_index : cfg.first_symbol;
  unsigned symbol_index_end  = ((hop == 0) && pattern.hopping_symbol_index.has_value())
                                   ? *pattern.hopping_symbol_index
                                   : cfg.first_symbol + cfg.nof_symbols;
  unsigned dmrs_symbol_index = 0;
  // For each OFDM symbol in the transmission...
  for (; symbol_index != symbol_index_end; ++symbol_index) {
    // Skip if the symbol does not carry DM-RS.
    if (!pattern.symbols.test(symbol_index)) {
      continue;
    }

    // Select symbol buffer for the selected layer and symbol.
    span<cf_t> layer_dmrs_symbols = rx_symbols.get_symbol(dmrs_symbol_index++, i_cdm);

    // Get view of the OFDM symbol carrying the DM-RS to extract.
    span<const cbf16_t> ofdm_symbol_view = grid.get_view(cfg.rx_ports[port], symbol_index);

    if (is_contiguous) {
      layer_dmrs_symbols = extract_re_prb(
          layer_dmrs_symbols, pattern.re_pattern, ofdm_symbol_view.subspan(i_prb_begin * NRE, nof_prb * NRE));
    } else {
      hop_rb_mask.for_each(0, hop_rb_mask.size(), [&](unsigned i_rb) {
        layer_dmrs_symbols =
            extract_re_prb(layer_dmrs_symbols, pattern.re_pattern, ofdm_symbol_view.subspan(i_rb * NRE, NRE));
      });
    }

    // The DM-RS symbol buffer must be complete.
    srsran_assert(layer_dmrs_symbols.empty(),
                  "The DM-RS buffer is not completed. {} samples have not been read.",
                  layer_dmrs_symbols.size());
  }

  return dmrs_symbol_index;
}

void srsran::apply_fd_smoothing(span<cf_t>                                   enlarged_filtered_pilots_out,
                                span<cf_t>                                   enlarged_pilots_in,
                                unsigned                                     nof_rb,
                                unsigned                                     stride,
                                port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy)
{
  static constexpr unsigned MAX_V_PILOTS = port_channel_estimator_average_impl::MAX_V_PILOTS;

  switch (fd_smoothing_strategy) {
    case port_channel_estimator_fd_smoothing_strategy::mean:
      std::fill(
          enlarged_filtered_pilots_out.begin(), enlarged_filtered_pilots_out.end(), srsvec::mean(enlarged_pilots_in));
      break;
    case port_channel_estimator_fd_smoothing_strategy::filter: {
      // Generate a low pass filter.
      filter_type rc(nof_rb, stride);

      // Recover data part of extended_pilots_lse
      span<const cf_t> pilots_lse =
          enlarged_pilots_in.subspan(MAX_V_PILOTS, enlarged_pilots_in.size() - 2 * MAX_V_PILOTS);
      // Create virtual pilots.
      unsigned nof_v_pilots = std::min<unsigned>(MAX_V_PILOTS, rc.rc_filter.size() / 2);
      if (nof_rb == 1) {
        nof_v_pilots = pilots_lse.size() / nof_rb;
      }
      span<cf_t> enlarged_pilots =
          enlarged_pilots_in.subspan(MAX_V_PILOTS - nof_v_pilots, pilots_lse.size() + 2 * nof_v_pilots);
      add_v_pilots(enlarged_pilots, pilots_lse);

      span<cf_t> enlarged_filtered_pilots =
          enlarged_filtered_pilots_out.subspan(MAX_V_PILOTS - nof_v_pilots, pilots_lse.size() + 2 * nof_v_pilots);
      // Apply filter.
      srsvec::convolution_same(enlarged_filtered_pilots, enlarged_pilots, rc.rc_filter);
    } break;

    case port_channel_estimator_fd_smoothing_strategy::none:
    default:
      // No strategy.
      srsvec::copy(enlarged_filtered_pilots_out, enlarged_pilots_in);
      break;
  }
}

float srsran::estimate_time_alignment(const re_measurement<cf_t>&                       pilots_lse,
                                      const port_channel_estimator::layer_dmrs_pattern& pattern,
                                      unsigned                                          hop,
                                      subcarrier_spacing                                scs,
                                      time_alignment_estimator&                         ta_estimator)
{
  const crb_bitmap& hop_rb_mask = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;

  unsigned nof_symbols = pilots_lse.size().nof_symbols;
  unsigned nof_slices  = pilots_lse.size().nof_slices;
  unsigned nof_re      = pilots_lse.size().nof_subc;

  // Convert RE measurement to a RE buffer.
  modular_re_buffer_reader<cf_t, MAX_NSYMB_PER_SLOT * port_channel_estimator_average_impl::MAX_LAYERS>
      pilots_lse_buffer(nof_slices * nof_symbols, nof_re);
  for (unsigned i_symbol = 0, count = 0; i_symbol != nof_symbols; ++i_symbol) {
    for (unsigned i_slice = 0; i_slice != nof_slices; ++i_slice) {
      pilots_lse_buffer.set_slice(count++, pilots_lse.get_symbol(i_symbol, i_slice));
    }
  }

  // Handle contiguous RB mask cases.
  if (hop_rb_mask.is_contiguous()) {
    // RE pattern for PUCCH Format 1, 3 and 4.
    if (pattern.re_pattern.all()) {
      return ta_estimator.estimate(pilots_lse_buffer, 1, scs).time_alignment;
    }

    // RE pattern for PUSCH.
    if ((pattern.re_pattern == re_pattern_pusch_0) || (pattern.re_pattern == re_pattern_pusch_1)) {
      return ta_estimator.estimate(pilots_lse_buffer, 2, scs).time_alignment;
    }

    // RE pattern for PUCCH Format 2.
    if (pattern.re_pattern == re_pattern_pucch_f2) {
      return ta_estimator.estimate(pilots_lse_buffer, 3, scs).time_alignment;
    }
  }

  // Prepare RE mask, common for all symbols carrying DM-RS.
  bounded_bitset<MAX_RB * NRE> re_mask = hop_rb_mask.kronecker_product<NRE>(pattern.re_pattern);

  srsran_assert(pilots_lse_buffer.get_slice(0).size() == re_mask.count(),
                "Expected {} channel estimates, provided {}.",
                re_mask.size(),
                pilots_lse_buffer.get_slice(0).size());

  return ta_estimator.estimate(pilots_lse_buffer, re_mask, scs).time_alignment;
}

interpolator::configuration srsran::configure_interpolator(const bounded_bitset<NRE>& re_mask)
{
  int offset = re_mask.find_lowest();
  srsran_assert(offset != -1, "re_mask seems to have no active entries.");

  int stride = re_mask.find_lowest(offset + 1, re_mask.size());
  srsran_assert(stride != -1, "re_mask seems to have only one active entry.");

  return {static_cast<unsigned>(offset), static_cast<unsigned>(stride - offset)};
}

static void compute_v_pilots(span<cf_t> out, span<const float> in_abs, span<const float> in_arg, bool is_start)
{
  unsigned nof_v_pilots = out.size();
  srsran_assert(nof_v_pilots < 13, "The maximum number of virtual pilots is 12, requested {}.", nof_v_pilots);
  srsran_assert(in_abs.size() == nof_v_pilots,
                "Input and output should have the same size, provided {} and {}.",
                in_abs.size(),
                nof_v_pilots);
  srsran_assert(in_arg.size() == nof_v_pilots,
                "Input and output should have the same size, provided {} and {}.",
                in_arg.size(),
                nof_v_pilots);
  span<const int> used_indices = span<const int>(indices).first(nof_v_pilots);

  float mean_x    = static_cast<float>(nof_v_pilots * (nof_v_pilots - 1)) / 2.0F / nof_v_pilots;
  float norm_x_sq = static_cast<float>((nof_v_pilots - 1) * nof_v_pilots * (2 * nof_v_pilots - 1)) / 6.0F;

  float mean_abs  = srsvec::mean(in_abs);
  float slope_abs = srsvec::dot_prod(in_abs, used_indices, 0.0F);
  slope_abs -= mean_x * mean_abs * nof_v_pilots;
  slope_abs /= (norm_x_sq - nof_v_pilots * mean_x * mean_x);

  float intercept_abs = mean_abs - slope_abs * mean_x;

  float mean_arg  = srsvec::mean(in_arg);
  float slope_arg = srsvec::dot_prod(in_arg, used_indices, 0.0F);
  slope_arg -= mean_x * mean_arg * nof_v_pilots;
  slope_arg /= (norm_x_sq - nof_v_pilots * mean_x * mean_x);

  float intercept_arg = mean_arg - slope_arg * mean_x;

  int v_offset = -nof_v_pilots;
  if (!is_start) {
    v_offset = nof_v_pilots;
  }

  for (unsigned i_pilot = 0; i_pilot != nof_v_pilots; ++i_pilot) {
    int   i_virtual = static_cast<int>(i_pilot) + v_offset;
    float rho       = slope_abs * i_virtual + intercept_abs;
    out[i_pilot]    = std::polar(std::abs(rho),
                              slope_arg * i_virtual + intercept_arg + ((rho > 0) ? 0.0F : static_cast<float>(M_PI)));
  }
}

static void add_v_pilots(span<cf_t> new_pilots, span<const cf_t> old_pilots)
{
  std::array<float, port_channel_estimator_average_impl::MAX_V_PILOTS> aux_base_abs;
  std::array<float, port_channel_estimator_average_impl::MAX_V_PILOTS> aux_base_arg;

  unsigned nof_pilots   = old_pilots.size();
  unsigned nof_v_pilots = new_pilots.size() - nof_pilots;
  srsran_assert((nof_v_pilots >= 2) && ((nof_v_pilots & 1U) == 0U),
                "The total number of virtual pilots should be an even number not lower than 2, provided {}.",
                nof_v_pilots);
  nof_v_pilots /= 2;

  // Compute virtual pilots at the beginning of the band.
  span<const cf_t> base_pilots = old_pilots.first(nof_v_pilots);
  span<float>      base_abs    = span<float>(aux_base_abs).first(nof_v_pilots);
  std::transform(base_pilots.begin(), base_pilots.end(), base_abs.begin(), [](cf_t a) { return std::abs(a); });

  span<float> base_arg = span<float>(aux_base_arg).first(nof_v_pilots);
  srsvec::unwrap_arguments(base_arg, base_pilots);
  compute_v_pilots(new_pilots.first(nof_v_pilots), base_abs, base_arg, /* is_start = */ true);

  // Compute virtual pilots at the end of the band.
  base_pilots = old_pilots.last(nof_v_pilots);
  std::transform(base_pilots.begin(), base_pilots.end(), base_abs.begin(), [](cf_t a) { return std::abs(a); });

  srsvec::unwrap_arguments(base_arg, base_pilots);
  compute_v_pilots(new_pilots.last(nof_v_pilots), base_abs, base_arg, /* is start = */ false);
}

static span<cf_t> extract_re_prb(span<cf_t> out, const bounded_bitset<NRE>& re_mask, span<const cbf16_t> in)
{
  srsran_assert(in.size() % NRE == 0, "Invalid input size.");
  srsran_assert(re_mask.size() == NRE, "Invalid RE mask.");
  unsigned nof_prb = in.size() / NRE;
  srsran_assert(out.size() >= nof_prb * re_mask.count(), "Invalid output size.");

  if (re_mask.all()) {
    srsvec::convert(out.first(nof_prb * NRE), in);
    return out.last(out.size() - nof_prb * NRE);
  }

#ifdef __AVX2__
  if (re_mask == re_pattern_pucch_f2) {
    for (unsigned i_prb = 0; i_prb != nof_prb; ++i_prb) {
      __m128i data_idx = _mm_setr_epi32(1, 4, 7, 10);

      // Gather data RE as 32-bit unsigned integers.
      __m128i data_si128 = _mm_i32gather_epi32(reinterpret_cast<const int*>(in.data() + NRE * i_prb), data_idx, 4);

      // Convert to single precision.
      __m128i data_lo    = _mm_unpacklo_epi16(_mm_setzero_si128(), data_si128);
      __m128i data_hi    = _mm_unpackhi_epi16(_mm_setzero_si128(), data_si128);
      __m256i data_si256 = _mm256_set_m128i(data_hi, data_lo);

      // Store DM-RS REs.
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(out.data() + 4 * i_prb), data_si256);
    }

    return out.last(out.size() - 4 * nof_prb);
  }

  if ((re_mask == re_pattern_pusch_0) || (re_mask == re_pattern_pusch_1)) {
    // Shuffle indices.
    static constexpr std::array<uint8_t, 32> idx = {
        0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x02, 0x03, // 0 - 7
        0xff, 0xff, 0x08, 0x09, 0xff, 0xff, 0x0a, 0x0b, // 8 - 15
        0xff, 0xff, 0x10, 0x01, 0xff, 0xff, 0x12, 0x13, // 16 - 23
        0xff, 0xff, 0x18, 0x19, 0xff, 0xff, 0x1a, 0x1b, // 24 - 31
    };

    unsigned nof_dmrs = nof_prb * 6;
    unsigned i_dmrs   = 0;
    unsigned offset   = (re_mask == re_pattern_pusch_1) ? 1 : 0;
    for (unsigned end = (nof_dmrs / 4) * 4; i_dmrs != end; i_dmrs += 4) {
      // Load 8 contiguous RE in Complex BF16.
      __m256i data_cbf16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in.data() + 2 * i_dmrs + offset));

      // Shuffle the contiguous RE from Complex BF16 to single-precision FP, discarding the unwanted REs.
      __m256i data_cf =
          _mm256_shuffle_epi8(data_cbf16, _mm256_loadu_si256(reinterpret_cast<const __m256i*>(idx.data())));

      // Store DM-RS REs.
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(out.data() + i_dmrs), data_cf);
    }

    if (i_dmrs != nof_dmrs) {
      __m128i data_cbf16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in.data() + 2 * i_dmrs + offset));
      __m128i data_cf    = _mm_shuffle_epi8(data_cbf16, _mm_loadu_si128(reinterpret_cast<const __m128i*>(idx.data())));
      _mm_storeu_si128(reinterpret_cast<__m128i*>(out.data() + i_dmrs), data_cf);
    }

    return out.last(out.size() - nof_dmrs);
  }
#endif // __AVX2__

#ifdef __ARM_NEON
  if (re_mask == re_pattern_pucch_f2) {
    for (unsigned i_prb = 0; i_prb != nof_prb; ++i_prb) {
      // Load an entire PRB, deinterleave the data in 3 registers of four 32-bit unsigned integers and select the second
      // NEON register.
      uint32x4_t data_u32 = vld3q_u32(reinterpret_cast<const unsigned int*>(in.data() + NRE * i_prb)).val[1];

      // Convert to single precision.
      float32x4_t data_lo = vreinterpretq_f32_u16(vzip1q_u16(vdupq_n_u16(0), vreinterpretq_u16_u32(data_u32)));
      float32x4_t data_hi = vreinterpretq_f32_u16(vzip2q_u16(vdupq_n_u16(0), vreinterpretq_u16_u32(data_u32)));

      // Store DM-RS REs.
      vst1q_f32(reinterpret_cast<float*>(out.data() + 4 * i_prb), data_lo);
      vst1q_f32(reinterpret_cast<float*>(out.data() + 4 * i_prb + 2), data_hi);
    }

    return out.last(out.size() - 4 * nof_prb);
  }

  if ((re_mask == re_pattern_pusch_0) || (re_mask == re_pattern_pusch_1)) {
    unsigned nof_dmrs = nof_prb * 6;
    unsigned i_dmrs   = 0;
    unsigned offset   = (re_mask == re_pattern_pusch_1) ? 1 : 0;
    for (unsigned end = (nof_dmrs / 4) * 4; i_dmrs != end; i_dmrs += 4) {
      // Load 8 consecutive subcarriers, deinterleave the data in 2 registers of four 32-bit unsigned integers and
      // select the first register.
      uint32x4_t data_u32 = vld2q_u32(reinterpret_cast<const unsigned int*>(in.data() + 2 * i_dmrs + offset)).val[0];

      // Convert to single precision.
      float32x4_t data_lo = vreinterpretq_f32_u16(vzip1q_u16(vdupq_n_u16(0), vreinterpretq_u16_u32(data_u32)));
      float32x4_t data_hi = vreinterpretq_f32_u16(vzip2q_u16(vdupq_n_u16(0), vreinterpretq_u16_u32(data_u32)));

      // Store DM-RS REs.
      vst1q_f32(reinterpret_cast<float*>(out.data() + i_dmrs), data_lo);
      vst1q_f32(reinterpret_cast<float*>(out.data() + i_dmrs + 2), data_hi);
    }

    if (i_dmrs != nof_dmrs) {
      // Load 4 consecutive subcarriers, deinterleave the data in 2 registers of two 32-bit unsigned integers and
      // select the second register.

      uint32x2_t data_u32 = vld2_u32(reinterpret_cast<const unsigned int*>(in.data() + 2 * i_dmrs + offset)).val[0];

      // Convert to single precision.
      float32x2_t data_lo = vreinterpret_f32_u16(vzip1_u16(vdup_n_u16(0), vreinterpret_u16_u32(data_u32)));
      float32x2_t data_hi = vreinterpret_f32_u16(vzip2_u16(vdup_n_u16(0), vreinterpret_u16_u32(data_u32)));

      // Store DM-RS REs.
      vst1_f32(reinterpret_cast<float*>(out.data() + i_dmrs), data_lo);
      vst1_f32(reinterpret_cast<float*>(out.data() + i_dmrs + 1), data_hi);
    }

    return out.last(out.size() - nof_dmrs);
  }
#endif // __ARM_NEON

  // Generic algorithm.
  bounded_bitset<MAX_NOF_PRBS>       prb  = ~bounded_bitset<MAX_NOF_PRBS>(nof_prb);
  bounded_bitset<NRE * MAX_NOF_PRBS> mask = prb.kronecker_product(re_mask);
  mask.for_each(0, mask.size(), [&out, &in](unsigned i_re) mutable {
    out.front() = to_cf(in[i_re]);
    out         = out.last(out.size() - 1);
  });

  return out;
}
