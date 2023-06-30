/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  /// Correlation DFT size.
  static constexpr unsigned DFT_SIZE = 4096;

  /// \brief Constructor - Acquires ownership of the internal components.
  /// \param[in] idft_      Inverse DFT processor.
  /// \param[in] generator_ PRACH frequency-domain sequence generator.
  prach_detector_generic_impl(std::unique_ptr<dft_processor> idft_, std::unique_ptr<prach_generator> generator_) :
    idft(std::move(idft_)), generator(std::move(generator_))
  {
    srsran_assert(idft, "Invalid IDFT processor.");
    srsran_assert(idft->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
    srsran_assert(idft->get_size() == DFT_SIZE,
                  "IDFT size {} must be equal to {}.",
                  idft->get_size(),
                  static_cast<unsigned>(DFT_SIZE));
  };

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override;

private:
  /// \brief Estimates and accumulates the noise estimation.
  ///
  /// The noise is estimated by subtracting \c input from \c reference.
  ///
  /// \param[in,out] accumulator Noise estimation accumulator.
  /// \param[in]     reference   Reference power measurement.
  /// \param[in]     input       Input samples.
  /// \remark The input size must be equal to the accumulator size.
  static void vector_noise_estimation(span<float> accumulator, float reference, span<const float> input)
  {
    std::transform(
        input.begin(), input.end(), accumulator.begin(), accumulator.begin(), [&reference](float value, float acc) {
          float diff = reference - value;
          if (!std::isnormal(diff)) {
            diff = 1e9F;
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
  static_tensor<static_cast<unsigned>(metric_global_dims::all), float, DFT_SIZE, metric_global_dims> metric_global_num;
  /// Metric global denominator.
  static_tensor<static_cast<unsigned>(metric_global_dims::all), float, DFT_SIZE, metric_global_dims> metric_global_den;
  /// Temporal storage.
  std::array<float, DFT_SIZE> temp;
  std::array<float, DFT_SIZE> temp2;

  std::unique_ptr<dft_processor>   idft;
  std::unique_ptr<prach_generator> generator;
};

} // namespace srsran
