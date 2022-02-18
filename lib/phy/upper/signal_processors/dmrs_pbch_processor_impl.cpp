/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pbch_processor_impl.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

unsigned dmrs_pbch_processor_impl::c_init(const args_t& args)
{
  // Default values for L_max == 4
  uint64_t i_ssb = (args.ssb_idx & 0b11U) + 4UL * args.n_hf; // Least 2 significant bits

  if (args.L_max == 8 || args.L_max == 64) {
    i_ssb = args.ssb_idx & 0b111U; // Least 3 significant bits
  }

  return ((i_ssb + 1UL) * ((args.phys_cell_id / 4UL) + 1UL) << 11UL) + ((i_ssb + 1UL) << 6UL) + (args.phys_cell_id % 4);
}

void srsgnb::dmrs_pbch_processor_impl::generation(std::array<cf_t, NOF_RE>& sequence, const args_t& args) const
{
  // Calculate initial state
  prg->init(c_init(args));

  // Generate sequence
  prg->generate(M_SQRT1_2, {(float*)sequence.data(), 2 * NOF_RE});
}

void srsgnb::dmrs_pbch_processor_impl::mapping(const std::array<cf_t, NOF_RE>& r,
                                               resource_grid_writer&           grid,
                                               const args_t&                   args) const
{
  unsigned l0 = args.ssb_first_symbol;
  unsigned k0 = args.ssb_first_subcarrier;

  // Calculate index shift
  uint32_t v = args.phys_cell_id % 4;

  // r sequence read index
  uint32_t r_idx = 0;

  // Put sequence in symbol 1 (0 + v , 4 + v , 8 + v ,..., 236 + v)
  for (uint32_t k = v; k < 240; k += 4) {
    grid.put(l0 + 1, k0 + k, r[r_idx++]);
  }

  // Put sequence in symbol 2, lower section (0 + v , 4 + v , 8 + v ,..., 44 + v)
  for (uint32_t k = v; k < 48; k += 4) {
    grid.put(l0 + 2, k0 + k, r[r_idx++]);
  }

  // Put sequence in symbol 2, upper section (192 + v , 196 + v ,..., 236 + v)
  for (uint32_t k = 192 + v; k < 240; k += 4) {
    grid.put(l0 + 2, k0 + k, r[r_idx++]);
  }

  // Put sequence in symbol 3 (0 + v , 4 + v , 8 + v ,..., 236 + v)
  for (uint32_t k = v; k < 240; k += 4) {
    grid.put(l0 + 3, k0 + k, r[r_idx++]);
  }
}

void srsgnb::dmrs_pbch_processor_impl::map(resource_grid_writer& grid, const args_t& args)
{
  // Generate sequence
  std::array<cf_t, NOF_RE> sequence;
  generation(sequence, args);

  // Mapping to physical resources
  mapping(sequence, grid, args);
}

std::unique_ptr<dmrs_pbch_processor> srsgnb::create_dmrs_pbch_processor()
{
  return std::make_unique<dmrs_pbch_processor_impl>();
}
