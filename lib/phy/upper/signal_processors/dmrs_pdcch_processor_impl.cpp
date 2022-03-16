
#include "dmrs_pdcch_processor_impl.h"
#include "dmrs_helper.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

unsigned dmrs_pdcch_processor_impl::c_init(unsigned symbol, const dmrs_pdcch_processor::config_t& config)
{
  unsigned n_slot = config.slot.slot_index();
  unsigned n_id   = config.n_id;

  return ((MAX_NOF_DMRS_PER_SYMBOL * n_slot + symbol + 1) * (2 * n_id + 1) * pow2(17) + 2 * n_id) % pow2(31);
}

void dmrs_pdcch_processor_impl::sequence_generation(span<cf_t>                            sequence,
                                                    unsigned int                          symbol,
                                                    const dmrs_pdcch_processor::config_t& config) const
{
  // Initialise pseudo-random generator.
  prg->init(c_init(symbol, config));

  // Generate sequence.
  dmrs_sequence_generate(
      sequence, *prg, M_SQRT1_2 * config.amplitude, config.reference_point_k_rb, NOF_DMRS_PER_RB, config.rb_mask);
}

void dmrs_pdcch_processor_impl::mapping(resource_grid_writer&                 grid,
                                        span<const cf_t>                      sequence,
                                        span<const bool>                      mask,
                                        unsigned int                          symbol,
                                        const dmrs_pdcch_processor::config_t& config)
{
  // Put signal for each port.
  for (unsigned port_idx : config.ports) {
    grid.put(port_idx, symbol, 0, mask, sequence);
  }
}

void dmrs_pdcch_processor_impl::map(srsgnb::resource_grid_writer&                 grid,
                                    const srsgnb::dmrs_pdcch_processor::config_t& config)
{
  // Count number of DMRS.
  unsigned count_dmrs = 0;

  // Generate allocation mask, common for all symbol.
  std::array<bool, MAX_RB* NRE> mask = {};
  for (unsigned prb_idx = 0; prb_idx != MAX_RB; ++prb_idx) {
    // Skip if the RB is not used.
    if (!config.rb_mask[prb_idx]) {
      continue;
    }

    // Set the DMRS positions to true.
    for (unsigned re_idx = 1; re_idx < NRE; re_idx += STRIDE) {
      mask[NRE * prb_idx + re_idx] = true;
    }
    count_dmrs += NOF_DMRS_PER_RB;
  }

  // Generate and map for each symbol of the PDCCH transmission.
  for (unsigned symbol = 0; symbol != config.start_symbol_index + config.duration; ++symbol) {
    // Temporal allocation of the sequence.
    static_vector<cf_t, MAX_NOF_DMRS_PER_SYMBOL> sequence(count_dmrs);

    // Generate sequence.
    sequence_generation(sequence, symbol, config);

    // Map sequence in symbols.
    mapping(grid, sequence, mask, symbol, config);
  }
}

std::unique_ptr<dmrs_pdcch_processor> create_dmrs_pdcch_processor()
{
  return std::make_unique<dmrs_pdcch_processor_impl>();
}
