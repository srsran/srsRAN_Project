
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
#include "srsran/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/support/error_handling.h"

namespace srsran {

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
    srsran_assert(dft_1_25kHz, "Invalid DFT for 1.25kHz SCS");
    srsran_assert(dft_1_25kHz->get_direction() == dft_processor::direction::DIRECT,
                  "Invalid DFT direction for 1.25kHz SCS");
    srsran_assert(dft_5kHz, "Invalid DFT for 1.25kHz SCS");
    srsran_assert(dft_5kHz->get_direction() == dft_processor::direction::DIRECT, "Invalid DFT direction for 5kHz SCS");
  }

  // See interface for documentation.
  void demodulate(prach_buffer& buffer, span<const cf_t> input, const configuration& config) override;
};

} // namespace srsran