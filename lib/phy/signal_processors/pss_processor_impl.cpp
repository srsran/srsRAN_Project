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

#include "pss_processor_impl.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

pss_processor_impl::pregen_signal_s::pregen_signal_s()
{
  // Initialise M sequence x
  std::array<uint32_t, SEQUENCE_LEN + 7> x;
  x[6] = 1;
  x[5] = 1;
  x[4] = 1;
  x[3] = 0;
  x[2] = 1;
  x[1] = 1;
  x[0] = 0;

  // Generate M sequence x
  for (uint32_t i = 0; i != SEQUENCE_LEN; ++i) {
    x[i + 7] = (x[i + 4] + x[i]) % 2;
  }

  // Modulate M sequence d
  for (uint32_t i = 0; i != SEQUENCE_LEN; ++i) {
    (*this)[i] = 1.0F - 2.0F * (float)x[i];
  }
}

const pss_processor_impl::pregen_signal_s pss_processor_impl::signal = pss_processor_impl::pregen_signal_s();

void srsgnb::pss_processor_impl::generation(std::array<float, SEQUENCE_LEN>& sequence, const args_t& args) const
{
  // Calculate generation parameters
  unsigned m = M(phys_cell_id::NID_2(args.phys_cell_id));

  // Temporal sequence
  span<float>       tmp    = sequence;
  span<const float> pregen = signal;

  // Copy sequence from offset to the end
  srsvec::sc_prod(pregen.subspan(m, SEQUENCE_LEN - m), args.amplitude, tmp.subspan(0, SEQUENCE_LEN - m));

  // Copy sequence from 0 to offset
  srsvec::sc_prod(pregen.subspan(0, m), args.amplitude, tmp.subspan(SEQUENCE_LEN - m, m));
}

void srsgnb::pss_processor_impl::mapping(const std::array<float, SEQUENCE_LEN>& sequence,
                                         resource_grid_writer&                         grid,
                                         const args_t&                          args) const
{
  // Calculate symbol and first subcarrier for PSS
  unsigned l = args.ssb_first_symbol + SSB_L;
  unsigned k = args.ssb_first_subcarrier + SSB_K_BEGIN;

  // Write in grid
  for (unsigned i = 0; i != SEQUENCE_LEN; ++i) {
    grid.put(l, k, sequence[i]);
    ++k;
  }
}

void srsgnb::pss_processor_impl::map(resource_grid_writer& grid, const args_t& args)
{
  // Generate sequence
  std::array<float, SEQUENCE_LEN> sequence;
  generation(sequence, args);

  // Mapping to physical resources
  mapping(sequence, grid, args);
}

std::unique_ptr<pss_processor> srsgnb::create_pss_processor()
{
  return std::make_unique<pss_processor_impl>();
}
