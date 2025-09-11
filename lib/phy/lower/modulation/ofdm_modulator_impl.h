/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "phase_compensation_lut.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {

/// OFDM modulator factory configuration. Provides the necessary dependency instances.
struct ofdm_modulator_common_configuration {
  /// DFT instance. The ownership is transferred to the modulator.
  std::unique_ptr<dft_processor> dft;
};

/// Describes a generic OFDM symbol modulator.
class ofdm_symbol_modulator_impl : public ofdm_symbol_modulator
{
private:
  /// Indicates the DFT size.
  unsigned dft_size;
  /// Indicates the resource grid bandwidth in resource elements.
  unsigned rg_size;
  /// Cyclic prefix type.
  cyclic_prefix cp;
  /// Numerology.
  subcarrier_spacing scs;
  /// Sampling rate in Hz.
  unsigned sampling_rate_Hz;
  /// Scaling factor at the DFT output.
  float scale;
  /// DFT processor.
  std::unique_ptr<dft_processor> dft;
  /// Phase compensation look-up table.
  phase_compensation_lut phase_compensation_table;
  /// Current center frequency in Hertz.
  double current_center_freq_Hz;
  /// Next center frequency in Hertz.
  std::atomic<double> next_center_freq_Hz;

public:
  /// \brief Constructs an OFDM symbol modulator.
  /// \param[in] common_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_symbol_modulator_impl(ofdm_modulator_common_configuration& common_config,
                             const ofdm_modulator_configuration&  ofdm_config);

  // See the interface for documentation.
  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    return cp.get_length(symbol_index, scs).to_samples(sampling_rate_Hz) + dft_size;
  }

  // See the interface for documentation.
  void set_center_frequency(double center_frequency_Hz) override { next_center_freq_Hz = center_frequency_Hz; }

  // See the interface for documentation.
  void
  modulate(span<cf_t> ouput, const resource_grid_reader& grid, unsigned port_index, unsigned symbol_index) override;
};

/// Describes a generic OFDM slot modulator.
class ofdm_slot_modulator_impl : public ofdm_slot_modulator
{
private:
  /// Cyclic prefix type.
  cyclic_prefix cp;
  /// Resource grid numerology.
  unsigned numerology;
  /// Instance of symbol modulator.
  std::unique_ptr<ofdm_symbol_modulator> symbol_modulator;

public:
  /// \brief Constructs an OFDM slot modulator.
  /// \param[in] ofdm_config       OFDM factory parameters.
  /// \param[in] symbol_modulator_ OFDM symbol modulator instance.
  ofdm_slot_modulator_impl(const ofdm_modulator_configuration&    ofdm_config,
                           std::unique_ptr<ofdm_symbol_modulator> symbol_modulator_) :
    cp(ofdm_config.cp), numerology(ofdm_config.numerology), symbol_modulator(std::move(symbol_modulator_))
  {
    srsran_assert(symbol_modulator, "Invalid OFDM symbol modulator.");
  }

  // See interface for documentation;
  unsigned get_slot_size(unsigned slot_index) const override;

  // See interface for documentation;
  void modulate(span<cf_t> output, const resource_grid_reader& grid, unsigned port_index, unsigned slot_index) override;
};

} // namespace srsran
