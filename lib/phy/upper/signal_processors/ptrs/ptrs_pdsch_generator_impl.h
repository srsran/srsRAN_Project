/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"

namespace srsran {

/// Generic implementation of PT-RS for PDSCH generator.
class ptrs_pdsch_generator_generic_impl : public ptrs_pdsch_generator
{
public:
  /// Constructs a PT-RS for PDSCH generator with the required dependencies.
  ptrs_pdsch_generator_generic_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_gen_,
                                    std::unique_ptr<resource_grid_mapper>    mapper_) :
    pseudo_random_gen(std::move(pseudo_random_gen_)), mapper(std::move(mapper_))
  {
    srsran_assert(pseudo_random_gen, "Invalid sequence generator.");
  }

  // See interface for documentation.
  void generate(resource_grid_writer& grid, const configuration& config) override;

private:
  /// Pseudo-random sequence generator.
  std::unique_ptr<pseudo_random_generator> pseudo_random_gen;
  /// Resource grid mapper.
  std::unique_ptr<resource_grid_mapper> mapper;
  /// Temporary sequence.
  static_vector<cf_t, MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB>    dmrs_sequence;
  static_re_buffer<ptrs_constants::max_nof_ports, MAX_NOF_PRBS> sequence;
};

} // namespace srsran