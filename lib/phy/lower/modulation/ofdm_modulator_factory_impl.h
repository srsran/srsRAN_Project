
#ifndef SRSGNB_OFDM_MODULATOR_FACTORY_IMPL_H
#define SRSGNB_OFDM_MODULATOR_FACTORY_IMPL_H

#include "ofdm_modulator_impl.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator_factory.h"

namespace srsgnb {

/// Describes a generic OFDM modulator factory.
class ofdm_modulator_factory_impl : public ofdm_modulator_factory
{
private:
  /// Keep a copy of common configuration.
  ofdm_modulator_common_configuration common_config;

public:
  /// Constructs the OFDM modulator factory.
  ofdm_modulator_factory_impl(const ofdm_modulator_common_configuration& config) : common_config(config)
  {
    // Do nothing.
  }

  // See interface for documentation.
  std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& dft_config) override
  {
    return std::make_unique<ofdm_symbol_modulator_impl>(common_config, dft_config);
  }

  // See interface for documentation.
  std::unique_ptr<ofdm_slot_modulator>
  create_ofdm_slot_modulator(const ofdm_modulator_configuration& dft_config) override
  {
    return std::make_unique<ofdm_slot_modulator_impl>(common_config, dft_config);
  }
};

} // namespace srsgnb

#endif // SRSGNB_OFDM_MODULATOR_FACTORY_IMPL_H
