
#ifndef LIB_PHY_LOWER_MODULATION_OFDM_MODULATOR_IMPL_H
#define LIB_PHY_LOWER_MODULATION_OFDM_MODULATOR_IMPL_H

#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"

namespace srsgnb {

/// Describes a generic OFDM symbol modulator.
class ofdm_symbol_modulator_impl : public ofdm_symbol_modulator
{
public:
  // See interface for documentation.
  unsigned int get_symbol_size(unsigned int symbol_index) override;

  // See interface for documentation.
  void modulate(span<cf_t>                  ouput,
                unsigned int                port_index,
                unsigned int                symbol_index,
                const resource_grid_reader& grid) override;
};

} // namespace srsgnb

#endif // LIB_PHY_LOWER_MODULATION_OFDM_MODULATOR_IMPL_H
