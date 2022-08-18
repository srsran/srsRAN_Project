/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include "srsgnb/ran/prach/prach_constants.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

/// \brief Implements a simple PRACH detector.
///
/// Detects PRACH sequences in frequency-domain buffers using a matched filter correlation. It determines the detection
/// from the ratio of the frequency-domain correlation maximum value and the input buffer average power.
class prach_detector_simple_impl : public prach_detector
{
  static constexpr float DETECTION_THRESHOLD = 0.1F;

  std::unique_ptr<dft_processor>   idft;
  std::unique_ptr<prach_generator> generator;

public:
  /// \brief Constructor - Acquires ownership of the internal components.
  /// \param[in] idft_      Inverse DFT processor.
  /// \param[in] generator_ PRACH frequency-domain sequence generator.
  prach_detector_simple_impl(std::unique_ptr<dft_processor> idft_, std::unique_ptr<prach_generator> generator_) :
    idft(std::move(idft_)), generator(std::move(generator_))
  {
    report_fatal_error_if_not(idft, "Invalid IDFT processor.");
    report_fatal_error_if_not(idft->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
    report_fatal_error_if_not(idft->get_size() >= prach_constants::LONG_SEQUENCE_LENGTH,
                              "IDFT size {} must be at least {}.",
                              idft->get_size(),
                              prach_constants::LONG_SEQUENCE_LENGTH);
  };

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override;
};

} // namespace srsgnb
