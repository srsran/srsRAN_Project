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

#include "time_alignment_estimator_dft_impl.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_measurement.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/zero.h"
#include <utility>

using namespace srsran;

static double to_seconds(unsigned index, unsigned dft_size, subcarrier_spacing scs)
{
  // Calculate DFT sampling rate.
  unsigned sampling_rate = dft_size * scs_to_khz(scs) * 1000;

  // Calculate time.
  return static_cast<double>(index) / static_cast<double>(sampling_rate);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(span<const cf_t>                pilots_lse,
                                                                       bounded_bitset<max_nof_symbols> re_mask,
                                                                       subcarrier_spacing              scs)
{
  span<cf_t> channel_observed_freq = idft->get_input();
  srsvec::zero(channel_observed_freq);
  re_mask.for_each(0, re_mask.size(), [&channel_observed_freq, &pilots_lse, i_lse = 0U](unsigned i_re) mutable {
    channel_observed_freq[i_re] = pilots_lse[i_lse++];
  });

  span<const cf_t> channel_observed_time = idft->run();

  static constexpr unsigned  HALF_CP_LENGTH     = ((144 / 2) * dft_size) / 2048;
  std::pair<unsigned, float> observed_max_delay = srsvec::max_abs_element(channel_observed_time.first(HALF_CP_LENGTH));
  std::pair<unsigned, float> observed_max_advance = srsvec::max_abs_element(channel_observed_time.last(HALF_CP_LENGTH));

  double t_align_seconds;
  if (observed_max_delay.second >= observed_max_advance.second) {
    t_align_seconds = to_seconds(observed_max_delay.first, dft_size, scs);
  } else {
    t_align_seconds = -to_seconds(HALF_CP_LENGTH - observed_max_advance.first, dft_size, scs);
  }

  time_alignment_measurement result;
  result.time_alignment = t_align_seconds;
  result.min            = -to_seconds(HALF_CP_LENGTH, dft_size, scs);
  result.max            = to_seconds(HALF_CP_LENGTH, dft_size, scs);
  result.resolution     = to_seconds(1, dft_size, scs);
  return result;
}
