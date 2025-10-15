/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/pdcch/dmrs_pdcch_processor.h"

namespace srsran {

class dmrs_pdcch_processor_factory
{
public:
  virtual ~dmrs_pdcch_processor_factory()                = default;
  virtual std::unique_ptr<dmrs_pdcch_processor> create() = 0;
};

std::shared_ptr<dmrs_pdcch_processor_factory>
create_dmrs_pdcch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                       std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory);

} // namespace srsran
