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

#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include <memory>

namespace srsran {

/// Phase Tracking Reference Signals (PT-RS) for PDSCH generator factory interface.
class ptrs_pdsch_generator_factory
{
public:
  /// Default destructor.
  virtual ~ptrs_pdsch_generator_factory() = default;

  /// Creates a PT-RS for PDSCH generator.
  virtual std::unique_ptr<ptrs_pdsch_generator> create() = 0;
};

/// \brief Creates a generic PT-RS for PDSCH generator factory.
/// \param[in] pseudo_random_gen_factory Pseudo-random sequence generator factory.
/// \return A PT-RS for PDSCH generator factory instance.
std::shared_ptr<ptrs_pdsch_generator_factory>
create_ptrs_pdsch_generator_generic_factory(std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory,
                                            std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory);

} // namespace srsran
