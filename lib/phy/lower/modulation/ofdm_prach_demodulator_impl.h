
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
#include "srsgnb/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

/// Describes a generic OFDM PRACH demodulator.
class ofdm_prach_demodulator_impl : public ofdm_prach_demodulator
{
private:
  /// DFT processor for 1.25kHz subcarrier spacing.
  std::unique_ptr<dft_processor> dft_1_25kHz;
  /// DFT processor for 5kHz subcarrier spacing.
  std::unique_ptr<dft_processor> dft_5kHz;

public:
  ofdm_prach_demodulator_impl(std::unique_ptr<dft_processor> dft_1_25kHz_, std::unique_ptr<dft_processor> dft_5kHz_) :
    dft_1_25kHz(std::move(dft_1_25kHz_)), dft_5kHz(std::move(dft_5kHz_))
  {
    srsgnb_assert(dft_1_25kHz, "Invalid DFT for 1.25kHz SCS");
    srsgnb_assert(dft_1_25kHz->get_direction() == dft_processor::direction::DIRECT,
                  "Invalid DFT direction for 1.25kHz SCS");
    srsgnb_assert(dft_5kHz, "Invalid DFT for 1.25kHz SCS");
    srsgnb_assert(dft_5kHz->get_direction() == dft_processor::direction::DIRECT, "Invalid DFT direction for 5kHz SCS");
  }

  // See interface for documentation.
  void demodulate(prach_buffer& buffer, span<const cf_t> input, const configuration& config) override;
};

} // namespace srsgnb