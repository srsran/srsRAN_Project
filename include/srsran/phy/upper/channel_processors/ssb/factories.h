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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/ssb/pbch_encoder.h"
#include "srsran/phy/upper/channel_processors/ssb/pbch_modulator.h"
#include "srsran/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "srsran/phy/upper/signal_processors/ssb/factories.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class pbch_encoder_factory
{
public:
  virtual ~pbch_encoder_factory()                = default;
  virtual std::unique_ptr<pbch_encoder> create() = 0;
};

std::shared_ptr<pbch_encoder_factory>
create_pbch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_factory,
                               std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                               std::shared_ptr<polar_factory>                   polar_factory);

class pbch_modulator_factory
{
public:
  virtual ~pbch_modulator_factory()                = default;
  virtual std::unique_ptr<pbch_modulator> create() = 0;
};

std::shared_ptr<pbch_modulator_factory>
    create_pbch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>,
                                     std::shared_ptr<pseudo_random_generator_factory>);

class ssb_processor_factory
{
public:
  virtual ~ssb_processor_factory()                              = default;
  virtual std::unique_ptr<ssb_processor>     create()           = 0;
  virtual std::unique_ptr<ssb_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<ssb_processor>     create(srslog::basic_logger& logger);
};

struct ssb_processor_factory_sw_configuration {
  std::shared_ptr<pbch_encoder_factory>        encoder_factory;
  std::shared_ptr<pbch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_factory;
  std::shared_ptr<pss_processor_factory>       pss_factory;
  std::shared_ptr<sss_processor_factory>       sss_factory;
};

std::shared_ptr<ssb_processor_factory> create_ssb_processor_factory_sw(ssb_processor_factory_sw_configuration& config);

std::shared_ptr<ssb_processor_factory>
create_ssb_processor_pool_factory(std::shared_ptr<ssb_processor_factory> processor_factory,
                                  unsigned                               nof_concurrent_threads);

} // namespace srsran
