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

#pragma once

#include "srsran/adt/tensor.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/channel_processors/prach_generator.h"

namespace srsran {

/// Implements a parameter validator for \ref prach_detector_simple_impl.
class prach_detector_validator_impl : public prach_detector_validator
{
public:
  // See interface for documentation.
  bool is_valid(const prach_detector::configuration& config) const override;
};

/// \brief Implements a simple PRACH detector.
///
/// Detects PRACH sequences from the frequency-domain channel samples. The detector is inspired by the generalized
/// log-likelihood ratio detection test.
class prach_detector_generic_impl : public prach_detector
{
public:
  /// \brief Constructor - Acquires ownership of the internal components.
  /// \param[in] idft_long_       Inverse DFT processor for long preambles.
  /// \param[in] idft_short_      Inverse DFT processor for short preambles.
  /// \param[in] generator_       PRACH frequency-domain sequence generator.
  /// \param[in] combine_symbols_ Set to true for combining PRACH symbols for each port.
  /// \remark Assertions are triggered if the IDFT sizes are smaller than their sequences or greater than \ref
  /// MAX_DFT_SIZE.
  prach_detector_generic_impl(std::unique_ptr<dft_processor>   idft_long_,
                              std::unique_ptr<dft_processor>   idft_short_,
                              std::unique_ptr<prach_generator> generator_,
                              bool                             combine_symbols_);

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override;

private:
  /// Maximum IDFT size allowed.
  static constexpr unsigned MAX_IDFT_SIZE = 4096;

  /// \brief Estimates and accumulates the noise estimation.
  ///
  /// For each possible timing offset in the detection window, the noise is estimated by subtracting \c input (roughly
  /// speaking, an estimation of the PRACH power) from \c reference (roughly speaking, the total measured energy).
  ///
  /// \param[in,out] accumulator Noise estimation accumulator.
  /// \param[in]     reference   Reference power measurement, including noise.
  /// \param[in]     input       Input samples.
  /// \remark The input size must be equal to the accumulator size.
  static void vector_noise_estimation(span<float> accumulator, float reference, span<const float> input)
  {
    std::transform(
        input.begin(), input.end(), accumulator.begin(), accumulator.begin(), [&reference](float value, float acc) {
          float diff = reference - value;
          if (!std::isnormal(diff)) {
            diff = 1e-9F;
          }
          return diff + acc;
        });
  }

  /// Correlation tensor dimensions.
  enum class metric_global_dims : unsigned {
    /// Sample within a window.
    sample = 0,
    /// Correlation window for a sample.
    window,
    /// Total number of dimensions.
    all
  };

  /// Metric global numerator.
  static_tensor<static_cast<unsigned>(metric_global_dims::all), float, MAX_IDFT_SIZE, metric_global_dims>
      metric_global_num;
  /// Metric global denominator.
  static_tensor<static_cast<unsigned>(metric_global_dims::all), float, MAX_IDFT_SIZE, metric_global_dims>
      metric_global_den;
  /// Temporal storage.
  std::array<cf_t, MAX_IDFT_SIZE>  cf_temp;
  std::array<float, MAX_IDFT_SIZE> temp;
  std::array<float, MAX_IDFT_SIZE> temp2;

  std::unique_ptr<dft_processor>   idft_long;
  std::unique_ptr<dft_processor>   idft_short;
  std::unique_ptr<prach_generator> generator;

  /// Set to true for combining PRACH symbols for each port.
  bool combine_symbols;
};

} // namespace srsran
