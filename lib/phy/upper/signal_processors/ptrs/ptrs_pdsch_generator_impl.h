/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "srsran/ran/ptrs/ptrs_constants.h"

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
