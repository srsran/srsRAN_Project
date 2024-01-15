
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

#include "srsran/adt/slotted_array.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// Describes a generic OFDM PRACH demodulator.
class ofdm_prach_demodulator_impl : public ofdm_prach_demodulator
{
public:
  using dft_processors_table = slotted_id_table<prach_subcarrier_spacing,
                                                std::unique_ptr<dft_processor>,
                                                static_cast<unsigned>(prach_subcarrier_spacing::invalid)>;

  /// \brief Builds a PRACH OFDM demodulator.
  ///
  /// \param[in] srate_          Sampling rate.
  /// \param[in] dft_processors_ Table with one DFT processor for each of the possible RA subcarrier spacings.
  /// \remark An assertion is triggered if \c dft_processors_ do not contain a valid DFT processor for each of the
  /// possible random access subcarrier spacings.
  ofdm_prach_demodulator_impl(sampling_rate srate_, dft_processors_table dft_processors_) :
    srate(srate_), dft_processors(std::move(dft_processors_))
  {
    using int_type = std::underlying_type_t<prach_subcarrier_spacing>;
    for (int_type i_scs     = static_cast<int_type>(prach_subcarrier_spacing::kHz15),
                  i_scs_end = static_cast<int_type>(prach_subcarrier_spacing::invalid);
         i_scs != i_scs_end;
         ++i_scs) {
      prach_subcarrier_spacing ra_scs = static_cast<prach_subcarrier_spacing>(i_scs);
      srsran_assert(dft_processors.contains(ra_scs),
                    "Table does not contain a DFT processor for {} subcarrier spacing.",
                    to_string(ra_scs));
      srsran_assert(dft_processors[ra_scs], "Invalid DFT processor for {} subcarrier spacing.", to_string(ra_scs));
      srsran_assert(dft_processors[ra_scs]->get_direction() == dft_processor::direction::DIRECT,
                    "Invalid DFT processor direction for {} subcarrier spacing.",
                    to_string(ra_scs));
      srsran_assert(dft_processors[ra_scs]->get_size() == srate.get_dft_size(ra_scs_to_Hz(ra_scs)),
                    "Invalid DFT processor size (i.e., {}) for {} subcarrier spacing.",
                    dft_processors[ra_scs]->get_size(),
                    to_string(ra_scs));
    }
  }

  // See interface for documentation.
  void demodulate(prach_buffer& buffer, span<const cf_t> input, const configuration& config) override;

private:
  /// Sampling rate.
  sampling_rate srate;
  /// DFT processor table.
  dft_processors_table dft_processors;
};

} // namespace srsran