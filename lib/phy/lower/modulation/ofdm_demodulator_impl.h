/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef LIB_PHY_LOWER_MODULATION_OFDM_DEMODULATOR_IMPL_H
#define LIB_PHY_LOWER_MODULATION_OFDM_DEMODULATOR_IMPL_H

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/lower/modulation/ofdm_demodulator.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/zero.h"

namespace srsgnb {

/// OFDM factory configuration. Provides the necessary parameters
struct ofdm_demodulator_factory_config {
  dft_processor_factory& dft_factory;
};

/// Describes a generic OFDM symbol demodulator.
class ofdm_symbol_demodulator_impl : public ofdm_symbol_demodulator
{
private:
  /// Indicates the DFT size.
  unsigned dft_size;
  /// Indicates the resource grid bandwidth in resource elements.
  unsigned rg_size;
  /// Indicates the cyclic prefix length.
  cyclic_prefix cp;
  /// Indicates the numerology.
  unsigned numerology;
  /// Indicates the scaling factor at the DFT output.
  float scale;
  /// Indicates the center frequency of the carrier in Hz.
  double center_freq_hz;
  /// DFT processor.
  std::unique_ptr<dft_processor> dft;
  /// Internal buffer aimed at storing the phase compensated DFT outputs.
  std::vector<cf_t> compensated_output;

  /// \brief Gets the offset to a symbol including the cyclic prefixes.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The number of samples to the start of the given symbol.
  unsigned get_symbol_offset(unsigned symbol_index) const;

  /// \brief Computes the phase compensation (TS 138.211, Section 5.4) for a given symbol.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The phase compensation to be applied to the given symbol.
  cf_t get_phase_compensation(unsigned symbol_index) const;

public:
  /// \brief Constructs an OFDM symbol demodulator.
  /// \param[in] factory_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_symbol_demodulator_impl(const ofdm_demodulator_factory_config& factory_config,
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
    return cp.get_length(symbol_index, numerology, dft_size) + dft_size;
  }

  // See interface for documentation.
  void
  demodulate(resource_grid_writer& grid, span<const cf_t> input, unsigned port_index, unsigned symbol_index) override;
};

/// Describes a generic OFDM slot demodulator.
class ofdm_slot_demodulator_impl : public ofdm_slot_demodulator
{
private:
  /// Indicates the cyclic prefix length.
  cyclic_prefix cp;
  /// Instance of symbol demodulator.
  ofdm_symbol_demodulator_impl symbol_demodulator;

public:
  /// \brief Constructs an OFDM slot demodulator.
  /// \param[in] factory_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_slot_demodulator_impl(const ofdm_demodulator_factory_config& factory_config,
                             const ofdm_demodulator_configuration&  ofdm_config) :
    cp(ofdm_config.cp), symbol_demodulator(factory_config, ofdm_config)
  {
    // Do nothing.
  }

  // See interface for documentation;
  unsigned get_slot_size(unsigned slot_index) const override;

  // See interface for documentation;
  void
  demodulate(resource_grid_writer& grid, const span<cf_t> input, unsigned port_index, unsigned slot_index) override;
};

/// Describes a generic OFDM demodulator factory.
class ofdm_demodulator_factory_impl : public ofdm_demodulator_factory
{
private:
  /// Keep a copy of the DFT factory reference.
  dft_processor_factory& dft_factory;

public:
  /// Constructs the OFDM demodulator factory.
  ofdm_demodulator_factory_impl(const ofdm_demodulator_factory_config& config) : dft_factory(config.dft_factory)
  {
    // Do nothing.
  }

  // See interface for documentation.
  std::unique_ptr<ofdm_symbol_demodulator>
  create_ofdm_symbol_demodulator(const ofdm_demodulator_configuration& dft_config) override
  {
    ofdm_demodulator_factory_config factory_config = {dft_factory};
    return std::make_unique<ofdm_symbol_demodulator_impl>(factory_config, dft_config);
  }

  // See interface for documentation.
  std::unique_ptr<ofdm_slot_demodulator>
  create_ofdm_slot_demodulator(const ofdm_demodulator_configuration& dft_config) override
  {
    ofdm_demodulator_factory_config factory_config = {dft_factory};
    return std::make_unique<ofdm_slot_demodulator_impl>(factory_config, dft_config);
  }
};

std::unique_ptr<ofdm_demodulator_factory> create_ofdm_demodulator_factory(ofdm_demodulator_factory_config& config);

} // namespace srsgnb

#endif // LIB_PHY_LOWER_MODULATION_OFDM_DEMODULATOR_IMPL_H
