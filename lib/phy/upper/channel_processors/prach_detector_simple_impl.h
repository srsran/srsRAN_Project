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

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/upper/channel_processors/prach_detector.h"
#include "srsgnb/phy/upper/channel_processors/prach_generator.h"

namespace srsgnb {

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
    srsgnb_assert(idft, "Invalid IDFT processor.");
    srsgnb_assert(idft->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
    srsgnb_assert(idft->get_size() % ASSERT_DFT_SIZE_MULTIPLE == 0,
                  "IDFT size {} must be multiple of {}.",
                  idft->get_size(),
                  static_cast<unsigned>(ASSERT_DFT_SIZE_MULTIPLE));
  };

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override;
};

} // namespace srsgnb
