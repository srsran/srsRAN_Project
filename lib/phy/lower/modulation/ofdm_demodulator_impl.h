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

#include "phase_compensation_lut.h"
#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/lower/modulation/ofdm_demodulator.h"
#include <memory>

namespace srsgnb {

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
  std::vector<cf_t> compensated_output;

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

} // namespace srsgnb
