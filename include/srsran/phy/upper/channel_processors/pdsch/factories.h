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

#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_block_processor.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_modulator.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/pdsch/factories.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator_factory.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {

class pdsch_encoder_factory
{
public:
  virtual ~pdsch_encoder_factory()                = default;
  virtual std::unique_ptr<pdsch_encoder> create() = 0;
};

struct pdsch_encoder_factory_sw_configuration {
  std::shared_ptr<ldpc_encoder_factory>      encoder_factory;
  std::shared_ptr<ldpc_rate_matcher_factory> rate_matcher_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory;
};

std::shared_ptr<pdsch_encoder_factory> create_pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config);

/// HW-accelerated PDSCH encoder factory configuration parameters.
struct pdsch_encoder_factory_hw_configuration {
  std::shared_ptr<crc_calculator_factory>                crc_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory>             segmenter_factory;
  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory;
};

std::shared_ptr<pdsch_encoder_factory>
create_pdsch_encoder_factory_hw(const pdsch_encoder_factory_hw_configuration& config);

class pdsch_modulator_factory
{
public:
  virtual ~pdsch_modulator_factory()                = default;
  virtual std::unique_ptr<pdsch_modulator> create() = 0;
};

std::shared_ptr<pdsch_modulator_factory>
create_pdsch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>,
                                  std::shared_ptr<pseudo_random_generator_factory>,
                                  std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory);

class pdsch_processor_factory
{
public:
  virtual ~pdsch_processor_factory()                              = default;
  virtual std::unique_ptr<pdsch_processor>     create()           = 0;
  virtual std::unique_ptr<pdsch_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<pdsch_processor>     create(srslog::basic_logger& logger, bool enable_logging_broadcast);
};

std::shared_ptr<pdsch_processor_factory>
create_pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>        encoder_factory,
                                  std::shared_ptr<pdsch_modulator_factory>      modulator_factory,
                                  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory,
                                  std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_factory);

class pdsch_block_processor_factory
{
public:
  virtual ~pdsch_block_processor_factory()                = default;
  virtual std::unique_ptr<pdsch_block_processor> create() = 0;
};

std::shared_ptr<pdsch_block_processor_factory>
create_pdsch_block_processor_factory_sw(std::shared_ptr<ldpc_encoder_factory>            encoder_factory,
                                        std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory,
                                        std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                        std::shared_ptr<modulation_mapper_factory>       modulator_factory);

std::shared_ptr<pdsch_block_processor_factory>
create_pdsch_block_processor_factory_hw(std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> encoder_factory,
                                        std::shared_ptr<pseudo_random_generator_factory>       prg_factory,
                                        std::shared_ptr<modulation_mapper_factory>             modulator_factory);

std::shared_ptr<pdsch_block_processor_factory>
create_pdsch_block_processor_pool_factory(std::shared_ptr<pdsch_block_processor_factory> base,
                                          task_executor&                                 executor,
                                          unsigned                                       nof_concurrent_threads);

std::shared_ptr<pdsch_processor_factory>
create_pdsch_flexible_processor_factory_sw(std::shared_ptr<ldpc_segmenter_tx_factory>     ldpc_segmenter_factory,
                                           std::shared_ptr<pdsch_block_processor_factory> block_processor_factory,
                                           std::shared_ptr<resource_grid_mapper_factory>  rg_mapper_factory,
                                           std::shared_ptr<dmrs_pdsch_processor_factory>  dmrs_factory,
                                           std::shared_ptr<ptrs_pdsch_generator_factory>  ptrs_factory,
                                           task_executor&                                 executor,
                                           unsigned                                       nof_concurrent_threads,
                                           unsigned                                       cb_batch_length = 0);

std::shared_ptr<pdsch_processor_factory>
create_pdsch_processor_pool(std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory, unsigned max_nof_processors);

} // namespace srsran
