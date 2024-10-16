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