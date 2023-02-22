/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/channel_processors/prach_generator.h"

namespace srsran {

/// Implements a parameter validator for \ref prach_detector_simple_impl.
class prach_detector_validator_impl : public prach_detector_validator
{
public:
  // See interface for documentation.
  bool is_valid(const prach_detector::configuration& config) override { return true; }
};

/// \brief Implements a simple PRACH detector.
///
/// Detects PRACH sequences in frequency-domain buffers using a matched filter correlation. It determines the detection
/// from the ratio of the frequency-domain correlation maximum value and the input buffer average power.
class prach_detector_simple_impl : public prach_detector
{
  /// Correlation detection threshold.
  static constexpr float DETECTION_THRESHOLD = 0.07F;
  /// Correlation DFT size assertion of multiples. It is the ratio of the minimum time alignment command granularity
  /// (1.92 MHz) over the minimum PRACH subcarrier spacing (1.25 kHz).
  static constexpr unsigned ASSERT_DFT_SIZE_MULTIPLE = 1536;

  std::unique_ptr<dft_processor>   idft;
  std::unique_ptr<prach_generator> generator;

public:
  /// \brief Constructor - Acquires ownership of the internal components.
  /// \param[in] idft_      Inverse DFT processor.
  /// \param[in] generator_ PRACH frequency-domain sequence generator.
  prach_detector_simple_impl(std::unique_ptr<dft_processor> idft_, std::unique_ptr<prach_generator> generator_) :
    idft(std::move(idft_)), generator(std::move(generator_))
  {
    srsran_assert(idft, "Invalid IDFT processor.");
    srsran_assert(idft->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
    srsran_assert(idft->get_size() % ASSERT_DFT_SIZE_MULTIPLE == 0,
                  "IDFT size {} must be multiple of {}.",
                  idft->get_size(),
                  static_cast<unsigned>(ASSERT_DFT_SIZE_MULTIPLE));
  };

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override;
};

} // namespace srsran
