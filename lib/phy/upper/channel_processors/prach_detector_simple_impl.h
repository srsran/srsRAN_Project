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
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

/// \brief Implements a simple PRACH detector.
///
/// Detects PRACH sequences in time-domain buffers using a matched filter correlation. It determines the detection from
/// the ratio of the time-domain correlation maximum value and the input buffer average power.
///
/// It can only detect PRACH preambles within a window of the PRACH OFDM symbol length.
class prach_detector_simple_impl : public prach_detector
{
  static constexpr float DETECTION_THRESHOLD = 0.1F;

  std::unique_ptr<dft_processor>   dft_1_25_kHz;
  std::unique_ptr<dft_processor>   idft_1_25_kHz;
  std::unique_ptr<dft_processor>   dft_5_kHz;
  std::unique_ptr<dft_processor>   idft_5_kHz;
  std::unique_ptr<prach_generator> generator;
  srsvec::aligned_vec<cf_t>        signal_freq_temp;
  unsigned                         sampling_rate_Hz;

public:
  prach_detector_simple_impl(std::unique_ptr<dft_processor>   dft_1_25_kHz_,
                             std::unique_ptr<dft_processor>   idft_1_25_kHz_,
                             std::unique_ptr<dft_processor>   dft_5_kHz_,
                             std::unique_ptr<dft_processor>   idft_5_kHz_,
                             std::unique_ptr<prach_generator> generator_,
                             unsigned                         dft_size_15kHz_) :
    dft_1_25_kHz(std::move(dft_1_25_kHz_)),
    idft_1_25_kHz(std::move(idft_1_25_kHz_)),
    dft_5_kHz(std::move(dft_5_kHz_)),
    idft_5_kHz(std::move(idft_5_kHz_)),
    generator(std::move(generator_)),
    signal_freq_temp((dft_size_15kHz_ * 15000) / 1250),
    sampling_rate_Hz(dft_size_15kHz_ * 15000)
  {
    srsgnb_assert(dft_1_25_kHz, "Invalid DFT processor.");
    srsgnb_assert(dft_1_25_kHz->get_direction() == dft_processor::direction::DIRECT, "Expected DFT.");
    srsgnb_assert(dft_1_25_kHz->get_size() == (sampling_rate_Hz / 1250), "Invalid DFT size.");
    srsgnb_assert(idft_1_25_kHz, "Invalid IDFT processor.");
    srsgnb_assert(idft_1_25_kHz->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
    srsgnb_assert(idft_1_25_kHz->get_size() == (sampling_rate_Hz / 1250), "Invalid DFT size.");
    srsgnb_assert(dft_5_kHz, "Invalid DFT processor.");
    srsgnb_assert(dft_5_kHz->get_direction() == dft_processor::direction::DIRECT, "Expected DFT.");
    srsgnb_assert(dft_5_kHz->get_size() == (sampling_rate_Hz / 5000), "Invalid DFT size.");
    srsgnb_assert(idft_5_kHz, "Invalid IDFT processor.");
    srsgnb_assert(idft_5_kHz->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
    srsgnb_assert(idft_5_kHz->get_size() == (sampling_rate_Hz / 5000), "Invalid DFT size.");
  };

  // See interface for documentation.
  detection_result detect(span<const cf_t> signal, const slot_configuration& config) override;
};

} // namespace srsgnb
