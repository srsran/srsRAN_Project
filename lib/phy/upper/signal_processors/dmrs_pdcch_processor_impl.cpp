
#include "dmrs_pdcch_processor_impl.h"

using namespace srsgnb;

unsigned dmrs_pdcch_processor_impl::c_init(unsigned symbol, const dmrs_pdcch_processor::config_t& config)
{
  unsigned n_slot = config.slot.slot_index();
  unsigned n_id   = config.n_id;

  return ((((NSYMB_PER_SLOT_NORM * n_slot + symbol + 1) * (2 * n_id + 1)) << 17U) + 2 * n_id) & INT32_MAX;
}

void dmrs_pdcch_processor_impl::sequence_generation(span<cf_t>                            sequence,
                                                    unsigned int                          symbol,
                                                    const dmrs_pdcch_processor::config_t& config) const
{
  // Calculate signal amplitude.
  float amplitude = M_SQRT1_2 * config.amplitude;

  // Initialise pseudo-random generator.
  prg->init(c_init(symbol, config));

  // Check the mask for all RB.
  unsigned prb_count            = 0; // Counts consecutive used PRB
  unsigned prb_skip             = 0; // Number of PRB to skip
  unsigned reference_point_k_rb = config.reference_point_k_rb;

  // Iterate over all PRBs, starting at reference point for k.
  for (unsigned prb_idx = 0; prb_idx < MAX_RB; ++prb_idx) {
    // If the PRB is used for PDSCH transmission count
    if (config.rb_mask[prb_idx]) {
      // If it is the first PRB...
      if (prb_count == 0) {
        // ... discard unused pilots and reset counter unless the reference is not in 0.
        prb_skip = std::max(0, static_cast<int>(prb_skip) - static_cast<int>(reference_point_k_rb));
        prg->advance(prb_skip * NOF_DMRS_PER_RB * 2);
        prb_skip             = 0;
        reference_point_k_rb = 0;
      }
      ++prb_count;

      continue;
    }

    // Increase number of PRB to skip.
    ++prb_skip;

    // End of consecutive PRB, skip copying if no PRB was counted.
    if (prb_count == 0) {
      continue;
    }

    // Generate contiguous pilots.
    prg->generate(sequence.first(prb_count * NOF_DMRS_PER_RB), amplitude);

    // Advance sequence buffer.
    sequence = sequence.last(sequence.size() - prb_count * NOF_DMRS_PER_RB);

    // Reset counter.
    prb_count = 0;
  }

  // Generate the last group of contiguous RB.
  if (prb_count > 0) {
    prg->generate(sequence.first(prb_count * NOF_DMRS_PER_RB), amplitude);
  }
}

void dmrs_pdcch_processor_impl::mapping(resource_grid_writer&                 grid,
                                        span<const cf_t>                      sequence,
                                        span<const bool>                      mask,
                                        unsigned int                          symbol,
                                        const dmrs_pdcch_processor::config_t& config)
{
  // Put signal for each port.
  for (const uint8_t& port_idx : config.ports) {
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
  for (unsigned prb_idx = 0; prb_idx < MAX_RB; ++prb_idx) {
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
