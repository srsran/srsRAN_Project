/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "time_alignment_estimator_dft_impl.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_measurement.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/zero.h"
#include <algorithm>
#include <utility>

using namespace srsran;

template <typename IndexType>
static double to_seconds(IndexType index, unsigned dft_size, subcarrier_spacing scs)
{
  // Calculate DFT sampling rate.
  unsigned sampling_rate = dft_size * scs_to_khz(scs) * 1000;

  // Calculate time.
  return static_cast<double>(index) / static_cast<double>(sampling_rate);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(const re_buffer_reader<cf_t>&   symbols,
                                                                       bounded_bitset<max_nof_symbols> mask,
                                                                       subcarrier_spacing              scs,
                                                                       double                          max_ta)
{
  srsran_assert(mask.size() <= idft->get_size(),
                "The mask size {} is larger than the maximum allowed number of subcarriers {}.",
                mask.size(),
                idft->get_size());

  srsran_assert(mask.count() == symbols.get_slice(0).size(),
                "The number of complex symbols per port {} does not match the mask size {}.",
                symbols.get_slice(0).size(),
                mask.count());

  span<cf_t> channel_observed_freq = idft->get_input();
  srsvec::zero(channel_observed_freq);
  srsvec::zero(idft_abs2);

  for (unsigned i_in = 0, max_in = symbols.get_nof_slices(); i_in != max_in; ++i_in) {
    mask.for_each(0, mask.size(), [&channel_observed_freq, &symbols, i_in, i_lse = 0U](unsigned i_re) mutable {
      channel_observed_freq[i_re] = symbols.get_slice(i_in)[i_lse++];
    });

    span<const cf_t> channel_observed_time = idft->run();
    std::transform(
        idft_abs2.cbegin(), idft_abs2.cend(), channel_observed_time.begin(), idft_abs2.begin(), [](float a, cf_t b) {
          return a + std::norm(b);
        });
  }

  return estimate(scs, max_ta);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(span<const cf_t>                symbols,
                                                                       bounded_bitset<max_nof_symbols> mask,
                                                                       subcarrier_spacing              scs,
                                                                       double                          max_ta)
{
  modular_re_buffer_reader<cf_t, 1> symbols_view(1, symbols.size());
  symbols_view.set_slice(0, symbols);

  return estimate(symbols_view, mask, scs, max_ta);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(const re_buffer_reader<cf_t>& symbols,
                                                                       unsigned                      stride,
                                                                       srsran::subcarrier_spacing    scs,
                                                                       double                        max_ta)
{
  srsran_assert(
      symbols.get_slice(0).size() * stride <= idft->get_size(),
      "The number of complex symbols (i.e., {}) times the stride (i.e., {}) exceeds the IDFT size (i.e., {}).",
      symbols.get_slice(0).size(),
      stride,
      idft->get_size());
  span<cf_t> channel_observed_freq = idft->get_input();
  srsvec::zero(channel_observed_freq);
  srsvec::zero(idft_abs2);

  for (unsigned i_in = 0, max_in = symbols.get_nof_slices(); i_in != max_in; ++i_in) {
    for (unsigned i_symbol = 0, i_re = 0, i_end = stride * symbols.get_slice(i_in).size(); i_re != i_end;
         i_re += stride) {
      channel_observed_freq[i_re] = symbols.get_slice(i_in)[i_symbol++];
    }

    span<const cf_t> channel_observed_time = idft->run();
    std::transform(
        idft_abs2.cbegin(), idft_abs2.cend(), channel_observed_time.begin(), idft_abs2.begin(), [](float a, cf_t b) {
          return a + std::norm(b);
        });
  }

  return estimate(scs, max_ta);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(span<const cf_t>           symbols,
                                                                       unsigned                   stride,
                                                                       srsran::subcarrier_spacing scs,
                                                                       double                     max_ta)
{
  modular_re_buffer_reader<cf_t, 1> symbols_view(1, symbols.size());
  symbols_view.set_slice(0, symbols);

  return estimate(symbols_view, stride, scs, max_ta);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(srsran::subcarrier_spacing scs, double max_ta)
{
  unsigned max_ta_samples = ((144 / 2) * dft_size) / 2048;

  if (std::isnormal(max_ta)) {
    max_ta_samples = static_cast<unsigned>(std::floor(max_ta * static_cast<double>(scs_to_khz(scs) * 1000 * dft_size)));
  }

  span<float>                channel_observed_time(idft_abs2);
  std::pair<unsigned, float> observed_max_delay   = srsvec::max_element(channel_observed_time.first(max_ta_samples));
  std::pair<unsigned, float> observed_max_advance = srsvec::max_element(channel_observed_time.last(max_ta_samples));

  double t_align_seconds;
  if (observed_max_delay.second >= observed_max_advance.second) {
    t_align_seconds = to_seconds(observed_max_delay.first, dft_size, scs);
  } else {
    t_align_seconds = -to_seconds(max_ta_samples - observed_max_advance.first, dft_size, scs);
  }

  // Fill results.
  time_alignment_measurement result;
  result.time_alignment = t_align_seconds;
  result.min            = -to_seconds(max_ta_samples, dft_size, scs);
  result.max            = to_seconds(max_ta_samples, dft_size, scs);
  result.resolution     = to_seconds(1, dft_size, scs);
  return result;
}
