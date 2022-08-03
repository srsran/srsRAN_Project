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
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

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

public:
  /// \brief Constructs an OFDM symbol modulator.
  /// \param[in] common_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_symbol_modulator_impl(ofdm_modulator_common_configuration& common_config,
                             const ofdm_modulator_configuration&  ofdm_config);

  // See interface for documentation.
  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    return cp.get_length(symbol_index, scs).to_samples(sampling_rate_Hz) + dft_size;
  }

  // See interface for documentation.
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
  ofdm_symbol_modulator_impl symbol_modulator;

public:
  /// \brief Constructs an OFDM slot modulator.
  /// \param[in] common_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_slot_modulator_impl(ofdm_modulator_common_configuration& common_config,
                           const ofdm_modulator_configuration&  ofdm_config) :
    cp(ofdm_config.cp), numerology(ofdm_config.numerology), symbol_modulator(common_config, ofdm_config)
  {
    // Do nothing.
  }

  // See interface for documentation;
  unsigned get_slot_size(unsigned slot_index) const override;

  // See interface for documentation;
  void modulate(span<cf_t> output, const resource_grid_reader& grid, unsigned port_index, unsigned slot_index) override;
};

} // namespace srsgnb
