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

#include "phase_compensation_lut.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/lower/modulation/ofdm_demodulator.h"
#include "srsran/srsvec/aligned_vec.h"
#include <memory>

namespace srsran {

/// OFDM demodulator factory configuration. Provides the necessary dependency instances.
struct ofdm_demodulator_common_configuration {
  /// DFT instance. The ownership is transferred to the demodulator.
  std::unique_ptr<dft_processor> dft;
};

/// Describes a generic OFDM symbol demodulator.
class ofdm_symbol_demodulator_impl : public ofdm_symbol_demodulator
{
private:
  /// Indicates the DFT size.
  unsigned dft_size;
  /// Indicates the resource grid bandwidth in resource elements.
  unsigned rg_size;
  /// Cyclic prefix type.
  cyclic_prefix cp;
  /// DFT window offset.
  unsigned nof_samples_window_offset;
  /// Numerology.
  subcarrier_spacing scs;
  /// Sampling rate in Hz.
  unsigned sampling_rate_Hz;
  /// Scaling factor at the DFT output.
  float scale;
  /// DFT processor.
  std::unique_ptr<dft_processor> dft;
  /// Phase compensation table.
  phase_compensation_lut phase_compensation_table;
  /// Internal buffer aimed at storing the phase compensated DFT outputs.
  srsvec::aligned_vec<cf_t> compensated_output;
  /// DFT window offset phase compensation.
  srsvec::aligned_vec<cf_t> window_phase_compensation;

public:
  /// \brief Constructs an OFDM symbol demodulator.
  /// \param[in] common_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_symbol_demodulator_impl(ofdm_demodulator_common_configuration& common_config,
                               const ofdm_demodulator_configuration&  ofdm_config);

  /// \brief Gets the resource grid bandwidth in resource elements.
  /// \return The number of resource elements in the grid.
  unsigned get_rg_size() const { return rg_size; };

  /// \brief Gets the offset in samples to the start of (the cyclic prefix of) a given symbol.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \param[in] slot_index Slot index within the subframe containing the symbol to demodulate.
  /// \return The number of samples preceding the given symbol.
  unsigned get_cp_offset(unsigned symbol_index, unsigned slot_index) const;

  // See interface for documentation.
  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    return cp.get_length(symbol_index, scs).to_samples(sampling_rate_Hz) + dft_size;
  }

  // See interface for documentation.
  void
  demodulate(resource_grid_writer& grid, span<const cf_t> input, unsigned port_index, unsigned symbol_index) override;
};

/// Describes a generic OFDM slot demodulator.
class ofdm_slot_demodulator_impl : public ofdm_slot_demodulator
{
private:
  /// Cyclic prefix type.
  cyclic_prefix cp;
  /// Instance of symbol demodulator.
  ofdm_symbol_demodulator_impl symbol_demodulator;

public:
  /// \brief Constructs an OFDM slot demodulator.
  /// \param[in] common_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_slot_demodulator_impl(ofdm_demodulator_common_configuration& common_config,
                             const ofdm_demodulator_configuration&  ofdm_config) :
    cp(ofdm_config.cp), symbol_demodulator(common_config, ofdm_config)
  {
    // Do nothing.
  }

  // See interface for documentation;
  unsigned get_slot_size(unsigned slot_index) const override;

  // See interface for documentation;
  void
  demodulate(resource_grid_writer& grid, span<const cf_t> input, unsigned port_index, unsigned slot_index) override;
};

} // namespace srsran
