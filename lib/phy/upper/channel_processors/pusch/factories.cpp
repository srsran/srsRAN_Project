/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "logging_pusch_processor_decorator.h"
#include "pusch_codeblock_decoder.h"
#include "pusch_decoder_hw_impl.h"
#include "pusch_decoder_impl.h"
#include "pusch_demodulator_impl.h"
#include "pusch_processor_impl.h"
#include "pusch_processor_pool.h"
#include "ulsch_demultiplex_impl.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/support/format_utils.h"

using namespace srsran;

namespace {

class pusch_decoder_factory_generic : public pusch_decoder_factory
{
public:
  explicit pusch_decoder_factory_generic(pusch_decoder_factory_sw_configuration config) :
    crc_factory(std::move(config.crc_factory)),
    segmenter_factory(std::move(config.segmenter_factory)),
    executor(config.executor)
  {
    srsran_assert(crc_factory, "Invalid CRC calculator factory.");
    srsran_assert(config.decoder_factory, "Invalid LDPC decoder factory.");
    srsran_assert(config.dematcher_factory, "Invalid LDPC dematcher factory.");
    srsran_assert(segmenter_factory, "Invalid LDPC segmenter factory.");

    std::vector<std::unique_ptr<pusch_codeblock_decoder>> codeblock_decoders(
        std::max(1U, config.nof_pusch_decoder_threads));
    for (std::unique_ptr<pusch_codeblock_decoder>& codeblock_decoder : codeblock_decoders) {
      pusch_codeblock_decoder::sch_crc crcs1;
      crcs1.crc16  = crc_factory->create(crc_generator_poly::CRC16);
      crcs1.crc24A = crc_factory->create(crc_generator_poly::CRC24A);
      crcs1.crc24B = crc_factory->create(crc_generator_poly::CRC24B);

      codeblock_decoder = std::make_unique<pusch_codeblock_decoder>(
          config.dematcher_factory->create(), config.decoder_factory->create(), crcs1);
    }

    decoder_pool = std::make_unique<pusch_decoder_impl::codeblock_decoder_pool>(std::move(codeblock_decoders));
  }

  std::unique_ptr<pusch_decoder> create() override
  {
    pusch_decoder_impl::sch_crc crcs;
    crcs.crc16  = crc_factory->create(crc_generator_poly::CRC16);
    crcs.crc24A = crc_factory->create(crc_generator_poly::CRC24A);
    crcs.crc24B = crc_factory->create(crc_generator_poly::CRC24B);

    return std::make_unique<pusch_decoder_impl>(segmenter_factory->create(), decoder_pool, std::move(crcs), executor);
  }

private:
  std::shared_ptr<pusch_decoder_impl::codeblock_decoder_pool> decoder_pool;
  std::shared_ptr<crc_calculator_factory>                     crc_factory;
  std::shared_ptr<ldpc_segmenter_rx_factory>                  segmenter_factory;
  task_executor*                                              executor;
};

/// HW-accelerated PUSCH decoder factory.
class pusch_decoder_factory_hw : public pusch_decoder_factory
{
private:
  std::shared_ptr<ldpc_segmenter_rx_factory>             segmenter_factory;
  std::shared_ptr<crc_calculator_factory>                crc_factory;
  std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hw_decoder_factory;

public:
  explicit pusch_decoder_factory_hw(const pusch_decoder_factory_hw_configuration& config) :
    segmenter_factory(std::move(config.segmenter_factory)),
    crc_factory(std::move(config.crc_factory)),
    hw_decoder_factory(std::move(config.hw_decoder_factory))
  {
    srsran_assert(segmenter_factory, "Invalid LDPC segmenter factory.");
    srsran_assert(crc_factory, "Invalid CRC factory.");
    srsran_assert(hw_decoder_factory, "Invalid hardware accelerator factory.");
  }

  std::unique_ptr<pusch_decoder> create() override
  {
    pusch_decoder_hw_impl::sch_crc crc = {
        crc_factory->create(crc_generator_poly::CRC16),
        crc_factory->create(crc_generator_poly::CRC24A),
        crc_factory->create(crc_generator_poly::CRC24B),
    };
    return std::make_unique<pusch_decoder_hw_impl>(segmenter_factory->create(), crc, hw_decoder_factory->create());
  }
};

class pusch_demodulator_factory_generic : public pusch_demodulator_factory
{
public:
  std::unique_ptr<pusch_demodulator> create() override
  {
    std::unique_ptr<evm_calculator> evm_calc;
    if (enable_evm) {
      evm_calc = demodulation_factory->create_evm_calculator();
    }
    return std::make_unique<pusch_demodulator_impl>(equalizer_factory->create(),
                                                    demodulation_factory->create_demodulation_mapper(),
                                                    std::move(evm_calc),
                                                    prg_factory->create(),
                                                    enable_post_eq_sinr);
  }

  pusch_demodulator_factory_generic(std::shared_ptr<channel_equalizer_factory>       equalizer_factory_,
                                    std::shared_ptr<channel_modulation_factory>      demodulation_factory_,
                                    std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                    bool                                             enable_evm_,
                                    bool                                             enable_post_eq_sinr_) :
    equalizer_factory(std::move(equalizer_factory_)),
    demodulation_factory(std::move(demodulation_factory_)),
    prg_factory(std::move(prg_factory_)),
    enable_evm(enable_evm_),
    enable_post_eq_sinr(enable_post_eq_sinr_)
  {
    srsran_assert(equalizer_factory, "Invalid equalizer factory.");
    srsran_assert(demodulation_factory, "Invalid demodulation factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
  }

private:
  std::shared_ptr<channel_equalizer_factory>       equalizer_factory;
  std::shared_ptr<channel_modulation_factory>      demodulation_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  bool                                             enable_evm;
  bool                                             enable_post_eq_sinr;
};

class pusch_processor_factory_generic : public pusch_processor_factory
{
public:
  explicit pusch_processor_factory_generic(pusch_processor_factory_sw_configuration& config) :
    estimator_factory(config.estimator_factory),
    demodulator_factory(config.demodulator_factory),
    demux_factory(config.demux_factory),
    decoder_factory(config.decoder_factory),
    uci_dec_factory(config.uci_dec_factory),
    ch_estimate_dimensions(config.ch_estimate_dimensions),
    dec_nof_iterations(config.dec_nof_iterations),
    dec_enable_early_stop(config.dec_enable_early_stop),
    csi_sinr_calc_method(config.csi_sinr_calc_method)
  {
    srsran_assert(estimator_factory, "Invalid channel estimation factory.");
    srsran_assert(demodulator_factory, "Invalid demodulation factory.");
    srsran_assert(demux_factory, "Invalid demux factory.");
    srsran_assert(decoder_factory, "Invalid decoder factory.");
    srsran_assert(uci_dec_factory, "Invalid UCI decoder factory.");

    // Create common dependencies.
    std::vector<std::unique_ptr<pusch_processor_impl::concurrent_dependencies>> dependencies(
        config.max_nof_concurrent_threads);
    std::generate(dependencies.begin(), dependencies.end(), [this]() {
      return std::make_unique<pusch_processor_impl::concurrent_dependencies>(estimator_factory->create(),
                                                                             demodulator_factory->create(),
                                                                             demux_factory->create(),
                                                                             uci_dec_factory->create(),
                                                                             ch_estimate_dimensions);
    });

    // Create common dependencies pool.
    dependencies_pool =
        std::make_shared<pusch_processor_impl::concurrent_dependencies_pool_type>(std::move(dependencies));
  }

  std::unique_ptr<pusch_processor> create() override
  {
    pusch_processor_impl::configuration config;
    config.thread_local_dependencies_pool = dependencies_pool;
    config.decoder                        = decoder_factory->create();
    config.dec_nof_iterations             = dec_nof_iterations;
    config.dec_enable_early_stop          = dec_enable_early_stop;
    config.csi_sinr_calc_method           = csi_sinr_calc_method;
    return std::make_unique<pusch_processor_impl>(config);
  }

  std::unique_ptr<pusch_pdu_validator> create_validator() override
  {
    return std::make_unique<pusch_processor_validator_impl>(ch_estimate_dimensions);
  }

private:
  std::shared_ptr<pusch_processor_impl::concurrent_dependencies_pool_type> dependencies_pool;
  std::shared_ptr<dmrs_pusch_estimator_factory>                            estimator_factory;
  std::shared_ptr<pusch_demodulator_factory>                               demodulator_factory;
  std::shared_ptr<ulsch_demultiplex_factory>                               demux_factory;
  std::shared_ptr<pusch_decoder_factory>                                   decoder_factory;
  std::shared_ptr<uci_decoder_factory>                                     uci_dec_factory;
  channel_estimate::channel_estimate_dimensions                            ch_estimate_dimensions;
  unsigned                                                                 dec_nof_iterations;
  bool                                                                     dec_enable_early_stop;
  channel_state_information::sinr_type                                     csi_sinr_calc_method;
};

class pusch_processor_pool_factory : public pusch_processor_factory
{
public:
  pusch_processor_pool_factory(std::shared_ptr<pusch_processor_factory> factory_, unsigned max_nof_processors_) :
    factory(std::move(factory_)), max_nof_processors(max_nof_processors_)
  {
    srsran_assert(factory, "Invalid PUSCH factory.");
  }

  std::unique_ptr<pusch_processor> create() override
  {
    if (max_nof_processors <= 1) {
      return factory->create();
    }

    std::vector<std::unique_ptr<pusch_processor>> processors(max_nof_processors);
    for (std::unique_ptr<pusch_processor>& processor : processors) {
      processor = factory->create();
    }

    return std::make_unique<pusch_processor_pool>(processors);
  }

  std::unique_ptr<pusch_processor> create(srslog::basic_logger& logger) override
  {
    if (max_nof_processors <= 1) {
      return factory->create(logger);
    }

    std::vector<std::unique_ptr<pusch_processor>> processors(max_nof_processors);
    for (std::unique_ptr<pusch_processor>& processor : processors) {
      processor = factory->create(logger);
    }

    return std::make_unique<pusch_processor_pool>(processors);
  }

  std::unique_ptr<pusch_pdu_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<pusch_processor_factory> factory;
  unsigned                                 max_nof_processors;
};

class ulsch_demultiplex_factory_sw : public ulsch_demultiplex_factory
{
public:
  std::unique_ptr<ulsch_demultiplex> create() override { return std::make_unique<ulsch_demultiplex_impl>(); }
};

} // namespace

std::shared_ptr<pusch_decoder_factory>
srsran::create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_configuration config)
{
  return std::make_shared<pusch_decoder_factory_generic>(std::move(config));
}

std::shared_ptr<pusch_decoder_factory>
srsran::create_pusch_decoder_factory_hw(const pusch_decoder_factory_hw_configuration& config)
{
  return std::make_shared<pusch_decoder_factory_hw>(config);
}

std::shared_ptr<pusch_demodulator_factory>
srsran::create_pusch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory,
                                            std::shared_ptr<channel_modulation_factory>      demodulation_factory,
                                            std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                            bool                                             enable_evm,
                                            bool                                             enable_post_eq_sinr)
{
  return std::make_shared<pusch_demodulator_factory_generic>(std::move(equalizer_factory),
                                                             std::move(demodulation_factory),
                                                             std::move(prg_factory),
                                                             enable_evm,
                                                             enable_post_eq_sinr);
}

std::shared_ptr<pusch_processor_factory>
srsran::create_pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config)
{
  return std::make_shared<pusch_processor_factory_generic>(config);
}

std::shared_ptr<pusch_processor_factory>
srsran::create_pusch_processor_pool(std::shared_ptr<pusch_processor_factory> factory, unsigned max_nof_processors)
{
  return std::make_shared<pusch_processor_pool_factory>(std::move(factory), max_nof_processors);
}

std::unique_ptr<pusch_processor> pusch_processor_factory::create(srslog::basic_logger& logger)
{
  return std::make_unique<logging_pusch_processor_decorator>(logger, create());
}

std::shared_ptr<ulsch_demultiplex_factory> srsran::create_ulsch_demultiplex_factory_sw()
{
  return std::make_shared<ulsch_demultiplex_factory_sw>();
}
