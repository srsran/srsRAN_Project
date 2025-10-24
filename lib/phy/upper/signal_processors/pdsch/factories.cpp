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

#include "srsran/phy/upper/signal_processors/pdsch/factories.h"
#include "dmrs_pdsch_processor_impl.h"

using namespace srsran;

namespace {

class dmrs_pdsch_processor_sw_factory : public dmrs_pdsch_processor_factory
{
private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory;

public:
  explicit dmrs_pdsch_processor_sw_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                           std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory_) :
    prg_factory(std::move(prg_factory_)), rg_mapper_factory(std::move(rg_mapper_factory_))
  {
    srsran_assert(prg_factory, "Invalid PRG factory.");
    srsran_assert(rg_mapper_factory, "Invalid resource grid mapper factory.");
  }

  std::unique_ptr<dmrs_pdsch_processor> create() override
  {
    return std::make_unique<dmrs_pdsch_processor_impl>(prg_factory->create(), rg_mapper_factory->create());
  }
};

} // namespace

std::shared_ptr<dmrs_pdsch_processor_factory>
srsran::create_dmrs_pdsch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                               std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory)
{
  return std::make_shared<dmrs_pdsch_processor_sw_factory>(std::move(prg_factory), std::move(rg_mapper_factory));
}
