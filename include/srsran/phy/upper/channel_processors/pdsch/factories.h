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

#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_modulator.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
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
create_pdsch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>,
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
                                  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory);

std::shared_ptr<pdsch_processor_factory>
create_pdsch_concurrent_processor_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_factory,
                                             std::shared_ptr<ldpc_encoder_factory>            ldpc_enc_factory,
                                             std::shared_ptr<ldpc_rate_matcher_factory>       ldpc_rm_factory,
                                             std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                             std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory,
                                             std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                             std::shared_ptr<dmrs_pdsch_processor_factory>    dmrs_factory,
                                             task_executor&                                   executor,
                                             unsigned                                         nof_concurrent_threads);

std::shared_ptr<pdsch_processor_factory>
create_pdsch_lite_processor_factory_sw(std::shared_ptr<ldpc_segmenter_tx_factory>       segmenter_factory,
                                       std::shared_ptr<ldpc_encoder_factory>            encoder_factory,
                                       std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory,
                                       std::shared_ptr<pseudo_random_generator_factory> scrambler_factory,
                                       std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                       std::shared_ptr<dmrs_pdsch_processor_factory>    dmrs_factory,
                                       std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory);

std::shared_ptr<pdsch_processor_factory>
create_pdsch_processor_asynchronous_pool(std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory,
                                         unsigned                                 max_nof_processors);

std::shared_ptr<pdsch_processor_factory>
create_pdsch_processor_pool(std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory, unsigned max_nof_processors);

} // namespace srsran