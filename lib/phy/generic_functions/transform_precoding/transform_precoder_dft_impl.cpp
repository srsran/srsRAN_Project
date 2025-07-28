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

#include "transform_precoder_dft_impl.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"
#include <numeric>

using namespace srsran;

void transform_precoder_dft_impl::deprecode_ofdm_symbol(span<cf_t> x, span<const cf_t> y)
{
  // Extract number of subcarriers.
  unsigned M_sc = x.size();
  srsran_assert(x.size() == y.size(), "Input and output sizes must be equal.");
  srsran_assert(M_sc % NRE == 0, "The number of subcarriers (i.e., {}) must be muliple of {}.", M_sc, NRE);

  // Calculate number of resource blocks.
  unsigned M_rb = M_sc / NRE;
  srsran_assert(transform_precoding::is_nof_prbs_valid(M_rb), "The number of PRB (i.e., {}) is not valid.", M_rb);
  srsran_assert(dft_processors.count(M_rb), "No DFT processor available for the number of PRB (i.e., {}).", M_rb);

  // Calculate scaling factor.
  float scaling_factor = 1.0F / std::sqrt(static_cast<float>(M_sc));

  // Select DFT processor.
  dft_processor& dft = *dft_processors.at(M_rb);

  // Convert input data in the DFT input.
  srsvec::copy(dft.get_input(), y);

  // Run DFT.
  span<const cf_t> out = dft.run();

  // Convert DFT output to DFT output data.
  srsvec::sc_prod(x, out, scaling_factor);
}

void transform_precoder_dft_impl::deprecode_ofdm_symbol_noise(span<float> out, span<const float> in)
{
  // Lambda to check if a noise variance value is valid.
  auto is_valid_noise_variance = [](float value) { return (value > 0.0F) && !std::isnan(value) && !std::isinf(value); };

  // Accumulate all the valid noise variances.
  unsigned count = 0;
  float    eq_noise_vars_mean =
      std::accumulate(in.begin(), in.end(), 0.0F, [&count, &is_valid_noise_variance](float acc, float value) {
        // Skip value if is negative, zero, nan or infinite.
        if (is_valid_noise_variance(value)) {
          ++count;
          return acc + value;
        }
        return acc;
      });
  if (count != 0) {
    eq_noise_vars_mean /= count;
  }

  // Write the mean channel estimates.
  std::transform(in.begin(), in.end(), out.begin(), [&eq_noise_vars_mean, &is_valid_noise_variance](float value) {
    return is_valid_noise_variance(value) ? eq_noise_vars_mean : value;
  });
}
