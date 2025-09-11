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

#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "logging_pdsch_processor_decorator.h"
#include "pdsch_block_processor_hw_impl.h"
#include "pdsch_block_processor_impl.h"
#include "pdsch_encoder_hw_impl.h"
#include "pdsch_encoder_impl.h"
#include "pdsch_modulator_impl.h"
#include "pdsch_processor_flexible_impl.h"
#include "pdsch_processor_impl.h"
#include "pdsch_processor_pool.h"
#include "pdsch_processor_validator_impl.h"
#include "srsran/phy/support/support_factories.h"
#include <memory>

using namespace srsran;

namespace {

class pdsch_encoder_factory_sw : public pdsch_encoder_factory
{
private:
  std::shared_ptr<ldpc_encoder_factory>      encoder_factory;
  std::shared_ptr<ldpc_rate_matcher_factory> rate_matcher_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory;

public:
  explicit pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config) :
    encoder_factory(std::move(config.encoder_factory)),
    rate_matcher_factory(std::move(config.rate_matcher_factory)),
    segmenter_factory(std::move(config.segmenter_factory))
  {
    srsran_assert(encoder_factory, "Invalid LDPC encoder factory.");
    srsran_assert(rate_matcher_factory, "Invalid LDPC RM factory.");
    srsran_assert(segmenter_factory, "Invalid LDPC segmenter factory.");
  }

  std::unique_ptr<pdsch_encoder> create() override
  {
    return std::make_unique<pdsch_encoder_impl>(
        segmenter_factory->create(), encoder_factory->create(), rate_matcher_factory->create());
  }
};

/// HW-accelerated PDSCH encoder factory.
class pdsch_encoder_factory_hw : public pdsch_encoder_factory
{
private:
  std::shared_ptr<crc_calculator_factory>                crc_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory>             segmenter_factory;
  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory;

public:
  explicit pdsch_encoder_factory_hw(const pdsch_encoder_factory_hw_configuration& config) :
    crc_factory(std::move(config.crc_factory)),
    segmenter_factory(std::move(config.segmenter_factory)),
    hw_encoder_factory(std::move(config.hw_encoder_factory))
  {
    srsran_assert(crc_factory, "Invalid CRC factory.");
    srsran_assert(segmenter_factory, "Invalid LDPC segmenter factory.");
    srsran_assert(hw_encoder_factory, "Invalid hardware accelerator factory.");
  }

  std::unique_ptr<pdsch_encoder> create() override
  {
    pdsch_encoder_hw_impl::sch_crc crc = {
        crc_factory->create(crc_generator_poly::CRC16),
        crc_factory->create(crc_generator_poly::CRC24A),
        crc_factory->create(crc_generator_poly::CRC24B),
    };
    return std::make_unique<pdsch_encoder_hw_impl>(crc, segmenter_factory->create(), hw_encoder_factory->create());
  }
};

class pdsch_modulator_factory_sw : public pdsch_modulator_factory
{
private:
  std::shared_ptr<modulation_mapper_factory>       modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory;

public:
  pdsch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                             std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory_) :
    modulator_factory(std::move(modulator_factory_)),
    prg_factory(std::move(prg_factory_)),
    rg_mapper_factory(std::move(rg_mapper_factory_))
  {
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
    srsran_assert(rg_mapper_factory, "Invalid resource grid mapper factory.");
  }

  std::unique_ptr<pdsch_modulator> create() override
  {
    return std::make_unique<pdsch_modulator_impl>(
        modulator_factory->create(), prg_factory->create(), rg_mapper_factory->create());
  }
};

class pdsch_processor_factory_sw : public pdsch_processor_factory
{
private:
  std::shared_ptr<pdsch_encoder_factory>        encoder_factory;
  std::shared_ptr<pdsch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory;
  std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_factory;

public:
  pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>        encoder_factory_,
                             std::shared_ptr<pdsch_modulator_factory>      modulator_factory_,
                             std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory_,
                             std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_factory_) :
    encoder_factory(std::move(encoder_factory_)),
    modulator_factory(std::move(modulator_factory_)),
    dmrs_factory(std::move(dmrs_factory_)),
    ptrs_factory(std::move(ptrs_factory_))
  {
    srsran_assert(encoder_factory, "Invalid encoder factory.");
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(dmrs_factory, "Invalid DM-RS factory.");
    srsran_assert(ptrs_factory, "Invalid PT-RS factory.");
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    return std::make_unique<pdsch_processor_impl>(
        encoder_factory->create(), modulator_factory->create(), dmrs_factory->create(), ptrs_factory->create());
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override
  {
    return std::make_unique<pdsch_processor_validator_impl>();
  }
};

class pdsch_block_processor_factory_sw : public pdsch_block_processor_factory
{
public:
  pdsch_block_processor_factory_sw(std::shared_ptr<ldpc_encoder_factory>            encoder_factory_,
                                   std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory_,
                                   std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                   std::shared_ptr<modulation_mapper_factory>       modulator_factory_) :
    encoder_factory(std::move(encoder_factory_)),
    rate_matcher_factory(std::move(rate_matcher_factory_)),
    prg_factory(std::move(prg_factory_)),
    modulator_factory(std::move(modulator_factory_))
  {
    srsran_assert(encoder_factory, "Invalid encoder factory.");
    srsran_assert(rate_matcher_factory, "Invalid rate matcher factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
    srsran_assert(modulator_factory, "Invalid modulator factory.");
  }

  std::unique_ptr<pdsch_block_processor> create() override
  {
    return std::make_unique<pdsch_block_processor_impl>(
        encoder_factory->create(), rate_matcher_factory->create(), prg_factory->create(), modulator_factory->create());
  }

private:
  std::shared_ptr<ldpc_encoder_factory>            encoder_factory;
  std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<modulation_mapper_factory>       modulator_factory;
};

class pdsch_block_processor_factory_hw : public pdsch_block_processor_factory
{
public:
  pdsch_block_processor_factory_hw(std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> encoder_factory_,
                                   std::shared_ptr<pseudo_random_generator_factory>       prg_factory_,
                                   std::shared_ptr<modulation_mapper_factory>             modulator_factory_) :
    encoder_factory(std::move(encoder_factory_)),
    prg_factory(std::move(prg_factory_)),
    modulator_factory(std::move(modulator_factory_))
  {
    srsran_assert(encoder_factory, "Invalid hardware-accelerated encoder factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
    srsran_assert(modulator_factory, "Invalid modulator factory.");
  }

  std::unique_ptr<pdsch_block_processor> create() override
  {
    return std::make_unique<pdsch_block_processor_hw_impl>(
        encoder_factory->create(), prg_factory->create(), modulator_factory->create());
  }

private:
  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> encoder_factory;
  std::shared_ptr<pseudo_random_generator_factory>       prg_factory;
  std::shared_ptr<modulation_mapper_factory>             modulator_factory;
};

class pdsch_block_processor_pool_factory_sw
{
public:
  pdsch_block_processor_pool_factory_sw(std::shared_ptr<pdsch_block_processor_factory> block_processor_factory_,
                                        unsigned                                       nof_concurrent_threads_) :
    block_processor_factory(std::move(block_processor_factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(block_processor_factory, "Invalid PDSCH block processor factory.");
    srsran_assert(nof_concurrent_threads > 0, "Number of concurrent threads must be greater or equal to one.");
  }

  std::shared_ptr<pdsch_processor_flexible_impl::pdsch_block_processor_pool> create()
  {
    if (!pool) {
      // Create vector of PDSCH block processors.
      std::vector<std::unique_ptr<pdsch_block_processor>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this]() { return block_processor_factory->create(); });
      pool = std::make_shared<pdsch_processor_flexible_impl::pdsch_block_processor_pool>(instances);
    }

    return pool;
  }

private:
  std::shared_ptr<pdsch_processor_flexible_impl::pdsch_block_processor_pool> pool;
  std::shared_ptr<pdsch_block_processor_factory>                             block_processor_factory;
  unsigned                                                                   nof_concurrent_threads;
};

class dmrs_generator_pool_factory_sw
{
public:
  dmrs_generator_pool_factory_sw(std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory_,
                                 unsigned                                      nof_concurrent_threads_) :
    dmrs_factory(std::move(dmrs_factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(dmrs_factory, "Invalid DM-RS factory.");
    srsran_assert(nof_concurrent_threads > 0, "Number of concurrent threads must be greater or equal to one.");
  }

  std::shared_ptr<pdsch_processor_flexible_impl::pdsch_dmrs_generator_pool> create()
  {
    if (!pool) {
      // Create vector of PDSCH DM-RS generators.
      std::vector<std::unique_ptr<dmrs_pdsch_processor>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this]() { return dmrs_factory->create(); });
      pool = std::make_shared<pdsch_processor_flexible_impl::pdsch_dmrs_generator_pool>(instances);
    }
    return pool;
  }

private:
  std::shared_ptr<pdsch_processor_flexible_impl::pdsch_dmrs_generator_pool> pool;
  std::shared_ptr<dmrs_pdsch_processor_factory>                             dmrs_factory;
  unsigned                                                                  nof_concurrent_threads;
};

class ptrs_generator_pool_factory_sw
{
public:
  ptrs_generator_pool_factory_sw(std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_factory_,
                                 unsigned                                      nof_concurrent_threads_) :
    ptrs_factory(std::move(ptrs_factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(ptrs_factory, "Invalid PT-RS factory.");
    srsran_assert(nof_concurrent_threads > 0, "Number of concurrent threads must be greater or equal to one.");
  }

  std::shared_ptr<pdsch_processor_flexible_impl::pdsch_ptrs_generator_pool> create()
  {
    if (!pool) {
      // Create vector of PDSCH PT-RS generators.
      std::vector<std::unique_ptr<ptrs_pdsch_generator>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this]() { return ptrs_factory->create(); });
      pool = std::make_shared<pdsch_processor_flexible_impl::pdsch_ptrs_generator_pool>(instances);
    }
    return pool;
  }

private:
  std::shared_ptr<pdsch_processor_flexible_impl::pdsch_ptrs_generator_pool> pool;
  std::shared_ptr<ptrs_pdsch_generator_factory>                             ptrs_factory;
  unsigned                                                                  nof_concurrent_threads;
};

class pdsch_processor_flexible_factory_sw : public pdsch_processor_factory
{
public:
  pdsch_processor_flexible_factory_sw(std::shared_ptr<ldpc_segmenter_tx_factory>     segmenter_factory_,
                                      std::shared_ptr<pdsch_block_processor_factory> block_processor_factory,
                                      std::shared_ptr<resource_grid_mapper_factory>  rg_mapper_factory_,
                                      std::shared_ptr<dmrs_pdsch_processor_factory>  dmrs_factory,
                                      std::shared_ptr<ptrs_pdsch_generator_factory>  ptrs_factory,
                                      task_executor&                                 executor_,
                                      unsigned                                       nof_concurrent_threads_,
                                      unsigned                                       cb_batch_length_) :
    segmenter_factory(std::move(segmenter_factory_)),
    rg_mapper_factory(std::move(rg_mapper_factory_)),
    executor(executor_),
    block_pool_factory(std::move(block_processor_factory), nof_concurrent_threads_),
    dmrs_pool_factory(std::move(dmrs_factory), nof_concurrent_threads_),
    ptrs_pool_factory(std::move(ptrs_factory), nof_concurrent_threads_),
    cb_batch_length(cb_batch_length_)
  {
    srsran_assert(segmenter_factory, "Invalid segmenter factory.");
    srsran_assert(rg_mapper_factory, "Invalid resource grid mapper factory.");
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    return std::make_unique<pdsch_processor_flexible_impl>(segmenter_factory->create(),
                                                           rg_mapper_factory->create(),
                                                           block_pool_factory.create(),
                                                           dmrs_pool_factory.create(),
                                                           ptrs_pool_factory.create(),
                                                           executor,
                                                           cb_batch_length);
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override
  {
    return std::make_unique<pdsch_processor_validator_impl>();
  }

private:
  std::shared_ptr<ldpc_segmenter_tx_factory>    segmenter_factory;
  std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory;
  task_executor&                                executor;
  pdsch_block_processor_pool_factory_sw         block_pool_factory;
  dmrs_generator_pool_factory_sw                dmrs_pool_factory;
  ptrs_generator_pool_factory_sw                ptrs_pool_factory;
  unsigned                                      cb_batch_length;
};

class pdsch_processor_asynchronous_pool_factory : public pdsch_processor_factory
{
public:
  pdsch_processor_asynchronous_pool_factory(std::shared_ptr<pdsch_processor_factory> factory_,
                                            unsigned                                 max_nof_processors_) :
    factory(std::move(factory_)), max_nof_processors(max_nof_processors_)
  {
    srsran_assert(factory, "Invalid PDSCH processor factory.");
    srsran_assert(max_nof_processors >= 1,
                  "The number of processors (i.e., {}) must be greater than or equal to one.",
                  max_nof_processors);
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    // Create processors.
    std::vector<std::unique_ptr<pdsch_processor_wrapper>> processors(max_nof_processors);
    for (std::unique_ptr<pdsch_processor_wrapper>& processor : processors) {
      processor = std::make_unique<pdsch_processor_wrapper>(factory->create());
    }

    return std::make_unique<pdsch_processor_pool>(processors);
  }

  std::unique_ptr<pdsch_processor> create(srslog::basic_logger& logger, bool enable_logging_broadcast) override
  {
    // Create processors.
    std::vector<std::unique_ptr<pdsch_processor_wrapper>> processors(max_nof_processors);
    for (std::unique_ptr<pdsch_processor_wrapper>& processor : processors) {
      processor = std::make_unique<pdsch_processor_wrapper>(factory->create(logger, enable_logging_broadcast));
    }

    return std::make_unique<pdsch_processor_pool>(processors);
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<pdsch_processor_factory> factory;
  unsigned                                 max_nof_processors;
};

} // namespace

std::shared_ptr<pdsch_encoder_factory>
srsran::create_pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config)
{
  return std::make_shared<pdsch_encoder_factory_sw>(config);
}

std::shared_ptr<pdsch_encoder_factory>
srsran::create_pdsch_encoder_factory_hw(const pdsch_encoder_factory_hw_configuration& config)
{
  return std::make_shared<pdsch_encoder_factory_hw>(config);
}

std::shared_ptr<pdsch_modulator_factory>
srsran::create_pdsch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory,
                                          std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                          std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory)
{
  return std::make_shared<pdsch_modulator_factory_sw>(
      std::move(modulator_factory), std::move(prg_factory), std::move(rg_mapper_factory));
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>        encoder_factory,
                                          std::shared_ptr<pdsch_modulator_factory>      modulator_factory,
                                          std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory,
                                          std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_factory)
{
  return std::make_shared<pdsch_processor_factory_sw>(
      std::move(encoder_factory), std::move(modulator_factory), std::move(dmrs_factory), std::move(ptrs_factory));
}

std::shared_ptr<pdsch_block_processor_factory>
srsran::create_pdsch_block_processor_factory_sw(std::shared_ptr<ldpc_encoder_factory>            encoder_factory,
                                                std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory,
                                                std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                                std::shared_ptr<modulation_mapper_factory>       modulator_factory)
{
  return std::make_shared<pdsch_block_processor_factory_sw>(std::move(encoder_factory),
                                                            std::move(rate_matcher_factory),
                                                            std::move(prg_factory),
                                                            std::move(modulator_factory));
}

std::shared_ptr<pdsch_block_processor_factory>
srsran::create_pdsch_block_processor_factory_hw(std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> encoder_factory,
                                                std::shared_ptr<pseudo_random_generator_factory>       prg_factory,
                                                std::shared_ptr<modulation_mapper_factory> modulator_factory)
{
  return std::make_shared<pdsch_block_processor_factory_hw>(
      std::move(encoder_factory), std::move(prg_factory), std::move(modulator_factory));
}

std::shared_ptr<pdsch_processor_factory> srsran::create_pdsch_flexible_processor_factory_sw(
    std::shared_ptr<ldpc_segmenter_tx_factory>     ldpc_segmenter_factory,
    std::shared_ptr<pdsch_block_processor_factory> block_processor_factory,
    std::shared_ptr<resource_grid_mapper_factory>  rg_mapper_factory,
    std::shared_ptr<dmrs_pdsch_processor_factory>  dmrs_factory,
    std::shared_ptr<ptrs_pdsch_generator_factory>  ptrs_factory,
    task_executor&                                 executor,
    unsigned                                       nof_concurrent_threads,
    unsigned                                       cb_batch_length)
{
  return std::make_shared<pdsch_processor_flexible_factory_sw>(std::move(ldpc_segmenter_factory),
                                                               std::move(block_processor_factory),
                                                               std::move(rg_mapper_factory),
                                                               std::move(dmrs_factory),
                                                               std::move(ptrs_factory),
                                                               executor,
                                                               nof_concurrent_threads,
                                                               cb_batch_length);
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_processor_pool(std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory,
                                    unsigned                                 max_nof_processors)
{
  return std::make_shared<pdsch_processor_asynchronous_pool_factory>(std::move(pdsch_proc_factory), max_nof_processors);
}

std::unique_ptr<pdsch_processor> pdsch_processor_factory::create(srslog::basic_logger& logger,
                                                                 bool                  enable_logging_broadcast)
{
  return std::make_unique<logging_pdsch_processor_decorator>(logger, enable_logging_broadcast, create());
}
