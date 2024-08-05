
/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    for (const auto& dft_proc : dft_processors) {
      srsran_assert(dft_proc, "Invalid DFT processor.");
      srsran_assert(dft_proc->get_direction() == dft_processor::direction::DIRECT, "Invalid DFT processor direction.");
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