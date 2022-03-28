
#include "ofdm_modulator_impl.h"

using namespace srsgnb;

unsigned int ofdm_symbol_modulator_impl::get_symbol_size(unsigned int symbol_index)
{
  return 0;
}

void ofdm_symbol_modulator_impl::modulate(srsgnb::span<srsgnb::cf_t>          ouput,
                                          unsigned int                        port_index,
                                          unsigned int                        symbol_index,
                                          const srsgnb::resource_grid_reader& grid)
{}
