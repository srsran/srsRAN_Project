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
#include "srsran/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_generator.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

class nzp_csi_rs_generator_factory
{
public:
  virtual ~nzp_csi_rs_generator_factory()                                        = default;
  virtual std::unique_ptr<nzp_csi_rs_generator>               create()           = 0;
  virtual std::unique_ptr<nzp_csi_rs_configuration_validator> create_validator() = 0;
  virtual std::unique_ptr<nzp_csi_rs_generator>               create(srslog::basic_logger& logger);
};

std::shared_ptr<nzp_csi_rs_generator_factory>
create_nzp_csi_rs_generator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                       std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory);

std::shared_ptr<nzp_csi_rs_generator_factory>
create_nzp_csi_rs_generator_pool_factory(std::shared_ptr<nzp_csi_rs_generator_factory> generator_factory,
                                         unsigned                                      nof_concurrent_threads);

} // namespace srsran
