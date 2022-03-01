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

#include "sss_processor_impl.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

sss_processor_impl::pregen_d0_s::pregen_d0_s()
{
  // Initialise M sequence x0
  std::array<uint32_t, SEQUENCE_LEN + 7> x0 = {};
  x0[6]                                     = 0;
  x0[5]                                     = 0;
  x0[4]                                     = 0;
  x0[3]                                     = 0;
  x0[2]                                     = 0;
  x0[1]                                     = 0;
  x0[0]                                     = 1;

  // Generate M sequence x0
  for (uint32_t i = 0; i < SEQUENCE_LEN; i++) {
    x0[i + 7] = (x0[i + 4] + x0[i]) % 2;
  }

  // Modulate M sequence d
  for (uint32_t i = 0; i != SEQUENCE_LEN; ++i) {
    (*this)[i] = (1.0f - 2.0f * x0[i]);
  }
}

sss_processor_impl::pregen_d1_s::pregen_d1_s()
{
  // Initialise M sequence x1
  std::array<uint32_t, SEQUENCE_LEN + 7> x1 = {};
  x1[6]                                     = 0;
  x1[5]                                     = 0;
  x1[4]                                     = 0;
  x1[3]                                     = 0;
  x1[2]                                     = 0;
  x1[1]                                     = 0;
  x1[0]                                     = 1;

  // Generate M sequence x0
  for (uint32_t i = 0; i < SEQUENCE_LEN; i++) {
    x1[i + 7] = (x1[i + 1] + x1[i]) % 2;
  }

  // Modulate M sequence d
  for (uint32_t i = 0; i != SEQUENCE_LEN; ++i) {
    (*this)[i] = (1.0f - 2.0f * x1[i]);
  }
}

const sss_processor_impl::pregen_d0_s sss_processor_impl::d0 = sss_processor_impl::pregen_d0_s();
const sss_processor_impl::pregen_d1_s sss_processor_impl::d1 = sss_processor_impl::pregen_d1_s();

void srsgnb::sss_processor_impl::generation(std::array<cf_t, SEQUENCE_LEN>& sequence, const config_t& config) const
{
  // Calculate generation parameters
  unsigned m0 = 15 * (phys_cell_id::NID_1(config.phys_cell_id) / 112) + 5 * phys_cell_id::NID_2(config.phys_cell_id);
  unsigned m1 = phys_cell_id::NID_1(config.phys_cell_id) % 112;

  // Convert sequence to span
  span<cf_t> seq_span = {sequence};

  // Apply d0 sequence with scaling
  srsvec::sc_prod({d0.begin() + m0, d0.size() - m0}, config.amplitude, seq_span.first(SEQUENCE_LEN - m0));
  srsvec::sc_prod({d0.begin(), m0}, config.amplitude, seq_span.last(m0));

  // Apply d1 sequence
  srsvec::prod({d1.begin() + m1, d1.size() - m1}, seq_span.first(SEQUENCE_LEN - m1), seq_span.first(SEQUENCE_LEN - m1));
  srsvec::prod({d1.begin(), m1}, seq_span.last(m1), seq_span.last(m1));
}

void srsgnb::sss_processor_impl::mapping(const std::array<cf_t, SEQUENCE_LEN>& sequence,
                                         resource_grid_writer&                 grid,
                                         const config_t&                       args) const
{
  // Calculate symbol and first subcarrier for SSS
  unsigned l = args.ssb_first_symbol + SSB_L;
  unsigned k = args.ssb_first_subcarrier + SSB_K_BEGIN;

  // Write in grid
  grid.put(l, k, sequence);
}

void srsgnb::sss_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Generate sequence
  std::array<cf_t, SEQUENCE_LEN> sequence;
  generation(sequence, config);

  // Mapping to physical resources
  mapping(sequence, grid, config);
}

std::unique_ptr<sss_processor> srsgnb::create_sss_processor()
{
  return std::make_unique<sss_processor_impl>();
}
