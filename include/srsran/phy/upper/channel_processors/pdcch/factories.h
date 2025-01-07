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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pdcch/pdcch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdcch/pdcch_modulator.h"
#include "srsran/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include <memory>

namespace srsran {

class pdcch_modulator_factory
{
public:
  virtual ~pdcch_modulator_factory()                = default;
  virtual std::unique_ptr<pdcch_modulator> create() = 0;
};

std::shared_ptr<pdcch_modulator_factory>
    create_pdcch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>,
                                      std::shared_ptr<pseudo_random_generator_factory>,
                                      std::shared_ptr<resource_grid_mapper_factory>);

class pdcch_encoder_factory
{
public:
  virtual ~pdcch_encoder_factory()                = default;
  virtual std::unique_ptr<pdcch_encoder> create() = 0;
};

std::shared_ptr<pdcch_encoder_factory>
create_pdcch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory,
                                std::shared_ptr<polar_factory>          encoder_factory);

class pdcch_processor_factory
{
public:
  virtual ~pdcch_processor_factory()                              = default;
  virtual std::unique_ptr<pdcch_processor>     create()           = 0;
  virtual std::unique_ptr<pdcch_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<pdcch_processor>     create(srslog::basic_logger& logger, bool enable_logging_broadcast);
};

std::shared_ptr<pdcch_processor_factory>
create_pdcch_processor_factory_sw(std::shared_ptr<pdcch_encoder_factory>        encoder_factory,
                                  std::shared_ptr<pdcch_modulator_factory>      modulator_factory,
                                  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory);

std::shared_ptr<pdcch_processor_factory>
create_pdcch_processor_pool_factory(std::shared_ptr<pdcch_processor_factory> processor_factory,
                                    unsigned                                 nof_concurrent_threads);

} // namespace srsran