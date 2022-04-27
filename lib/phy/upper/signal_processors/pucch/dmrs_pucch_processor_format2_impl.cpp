
#include "dmrs_pucch_processor_format2_impl.h"

using namespace srsgnb;

unsigned dmrs_pucch_processor_format2_impl::c_init(unsigned symbol, const dmrs_pucch_processor::config_t& config)
{
  return 0;
}

void dmrs_pucch_processor_format2_impl::sequence_generation(span<const cf_t>                      sequence,
                                                            unsigned                              symbol,
                                                            const dmrs_pucch_processor::config_t& config) const
{}

void dmrs_pucch_processor_format2_impl::mapping(resource_grid_writer&                 grid,
                                                span<const srsgnb::cf_t>              sequence,
                                                unsigned                              symbol,
                                                const dmrs_pucch_processor::config_t& config)
{}

void dmrs_pucch_processor_format2_impl::map(resource_grid_writer& grid, const dmrs_pucch_processor::config_t& config) {}