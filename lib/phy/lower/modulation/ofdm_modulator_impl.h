
#ifndef LIB_PHY_LOWER_MODULATION_OFDM_MODULATOR_IMPL_H
#define LIB_PHY_LOWER_MODULATION_OFDM_MODULATOR_IMPL_H

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/zero.h"

namespace srsgnb {

/// OFDM factory configuration. Provides the necessary parameters
struct ofdm_modulator_factory_config {
  dft_processor_factory& dft_factory;
};

/// Describes a generic OFDM symbol modulator.
class ofdm_symbol_modulator_impl : public ofdm_symbol_modulator
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

  /// \brief Gets the offset to a symbol including the cyclic prefixes.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The number of samples to the start of the given symbol.
  const unsigned get_symbol_offset(unsigned symbol_index);

  /// \brief Computes the phase compensation (TS 138.211, Section 5.4) for a given symbol.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The phase compensation to be applied to the given symbol.
  const cf_t get_phase_compensation(unsigned symbol_index);

public:
  /// \brief Constructs an OFDM symbol modulator.
  /// \param[in] factory_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_symbol_modulator_impl(const ofdm_modulator_factory_config& factory_config,
                             const ofdm_modulator_configuration&  ofdm_config);

  // See interface for documentation.
  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    return cp.get_length(symbol_index, numerology, dft_size) + dft_size;
  }

  // See interface for documentation.
  void
  modulate(span<cf_t> ouput, const resource_grid_reader& grid, unsigned port_index, unsigned symbol_index) override;
};

/// Describes a generic OFDM slot modulator.
class ofdm_slot_modulator_impl : public ofdm_slot_modulator
{
private:
  /// Indicates the cyclic prefix length.
  cyclic_prefix cp;
  /// Instance of symbol modulator.
  ofdm_symbol_modulator_impl symbol_modulator;

public:
  /// \brief Constructs an OFDM slot modulator.
  /// \param[in] factory_config Provides specific configuration parameters from the factory.
  /// \param[in] ofdm_config Provides generic OFDM configuration parameters.
  ofdm_slot_modulator_impl(const ofdm_modulator_factory_config& factory_config,
                           const ofdm_modulator_configuration&  ofdm_config) :
    cp(ofdm_config.cp), symbol_modulator(factory_config, ofdm_config)
  {
    // Do nothing.
  }

  // See interface for documentation;
  unsigned get_slot_size(unsigned slot_index) const override;

  // See interface for documentation;
  void modulate(span<cf_t> output, const resource_grid_reader& grid, unsigned port_index, unsigned slot_index) override;
};

/// Describes a generic OFDM modulator factory.
class ofdm_modulator_factory_impl : public ofdm_modulator_factory
{
private:
  /// Keep a copy of the DFT factory reference.
  dft_processor_factory& dft_factory;

public:
  /// Constructs the OFDM modulator factory.
  ofdm_modulator_factory_impl(const ofdm_modulator_factory_config& config) : dft_factory(config.dft_factory)
  {
    // Do nothing.
  }

  // See interface for documentation.
  std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& dft_config) override
  {
    ofdm_modulator_factory_config factory_config = {dft_factory};
    return std::make_unique<ofdm_symbol_modulator_impl>(factory_config, dft_config);
  }

  // See interface for documentation.
  std::unique_ptr<ofdm_slot_modulator>
  create_ofdm_slot_modulator(const ofdm_modulator_configuration& dft_config) override
  {
    ofdm_modulator_factory_config factory_config = {dft_factory};
    return std::make_unique<ofdm_slot_modulator_impl>(factory_config, dft_config);
  }
};

std::unique_ptr<ofdm_modulator_factory> create_ofdm_modulator_factory(ofdm_modulator_factory_config& config);

} // namespace srsgnb

#endif // LIB_PHY_LOWER_MODULATION_OFDM_MODULATOR_IMPL_H
