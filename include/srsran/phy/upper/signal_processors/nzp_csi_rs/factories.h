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
