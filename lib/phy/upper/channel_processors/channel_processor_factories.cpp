/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "pbch_encoder_impl.h"
#include "pbch_modulator_impl.h"
#include "pdcch_modulator_impl.h"
#include "pdsch_encoder_impl.h"
#include "pdsch_modulator_impl.h"
#include "pusch_decoder_impl.h"
#include "pusch_processor_impl.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"

using namespace srsgnb;

namespace {

class pbch_modulator_factory_sw : public pbch_modulator_factory
{
private:
  std::shared_ptr<modulation_mapper_factory>       modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pbch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory_,
                            std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(modulator_factory_), prg_factory(prg_factory_)
  {
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pbch_modulator> create() override
  {
    return std::make_unique<pbch_modulator_impl>(modulator_factory->create(), prg_factory->create());
  }
};

class pdcch_modulator_factory_sw : public pdcch_modulator_factory
{
private:
  std::shared_ptr<modulation_mapper_factory>       modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pdcch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(modulator_factory_), prg_factory(prg_factory_)
  {
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pdcch_modulator> create() override
  {
    return std::make_unique<pdcch_modulator_impl>(modulator_factory->create(), prg_factory->create());
  }
};

class pdsch_encoder_factory_sw : public pdsch_encoder_factory
{
private:
  std::shared_ptr<ldpc_encoder_factory>      encoder_factory;
  std::shared_ptr<ldpc_rate_matcher_factory> rate_matcher_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory;

public:
  pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config) :
    encoder_factory(config.encoder_factory),
    rate_matcher_factory(config.rate_matcher_factory),
    segmenter_factory(config.segmenter_factory)
  {
    srsran_assert(config.encoder_factory, "Invalid LDPC encoder factory.");
    srsran_assert(config.rate_matcher_factory, "Invalid LDPC RM factory.");
    srsran_assert(config.segmenter_factory, "Invalid LDPC segmenter factory.");
  }

  std::unique_ptr<pdsch_encoder> create() override
  {
    return std::make_unique<pdsch_encoder_impl>(
        segmenter_factory->create(), encoder_factory->create(), rate_matcher_factory->create());
  }
};

class pdsch_modulator_factory_sw : public pdsch_modulator_factory
{
private:
  std::shared_ptr<modulation_mapper_factory>       modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pdsch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(modulator_factory_), prg_factory(prg_factory_)
  {
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pdsch_modulator> create() override
  {
    return std::make_unique<pdsch_modulator_impl>(modulator_factory->create(), prg_factory->create());
  }
};

class pusch_decoder_factory_sw : public pusch_decoder_factory
{
private:
  std::shared_ptr<crc_calculator_factory>      crc_factory;
  std::shared_ptr<ldpc_decoder_factory>        decoder_factory;
  std::shared_ptr<ldpc_rate_dematcher_factory> dematcher_factory;
  std::shared_ptr<ldpc_segmenter_rx_factory>   segmenter_factory;

public:
  pusch_decoder_factory_sw(pusch_decoder_factory_sw_configuration& config) :
    crc_factory(config.crc_factory),
    decoder_factory(config.decoder_factory),
    dematcher_factory(config.dematcher_factory),
    segmenter_factory(config.segmenter_factory)
  {
    srsran_assert(config.crc_factory, "Invalid CRC calculator factory.");
    srsran_assert(config.decoder_factory, "Invalid LDPC decoder factory.");
    srsran_assert(config.dematcher_factory, "Invalid LDPC dematcher factory.");
    srsran_assert(config.segmenter_factory, "Invalid LDPC segmenter factory.");
  }

  std::unique_ptr<pusch_decoder> create() override
  {
    pusch_decoder_impl::sch_crc crcs;
    crcs.crc16  = crc_factory->create(crc_generator_poly::CRC16);
    crcs.crc24A = crc_factory->create(crc_generator_poly::CRC24A);
    crcs.crc24B = crc_factory->create(crc_generator_poly::CRC24B);
    return std::make_unique<pusch_decoder_impl>(
        segmenter_factory->create(), dematcher_factory->create(), decoder_factory->create(), std::move(crcs));
  }
};

class pusch_processor_factory_sw : public pusch_processor_factory
{
private:
  // TBD.

public:
  pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config)
  {
    // TBD.
  }

  std::unique_ptr<pusch_processor> create() override
  {
    pusch_processor_configuration config;
    config.estimator   = nullptr;
    config.decoder     = nullptr;
    config.demodulator = nullptr;
    return std::make_unique<pusch_processor_impl>(config);
  }
};

} // namespace

std::unique_ptr<pbch_encoder> srsgnb::create_pbch_encoder()
{
  return std::make_unique<pbch_encoder_impl>(create_crc_calculator_factory_sw()->create(crc_generator_poly::CRC24C),
                                             create_pseudo_random_generator_sw_factory()->create());
}

std::shared_ptr<pbch_modulator_factory>
srsgnb::create_pbch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory,
                                         std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pbch_modulator_factory_sw>(modulator_factory, prg_factory);
}

std::shared_ptr<pdcch_modulator_factory>
srsgnb::create_pdcch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory,
                                          std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pdcch_modulator_factory_sw>(modulator_factory, prg_factory);
}

std::shared_ptr<pdsch_encoder_factory>
srsgnb::create_pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config)
{
  return std::make_shared<pdsch_encoder_factory_sw>(config);
}

std::shared_ptr<pdsch_modulator_factory>
srsgnb::create_pdsch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory,
                                          std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pdsch_modulator_factory_sw>(modulator_factory, prg_factory);
}

std::shared_ptr<pusch_decoder_factory>
srsgnb::create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_configuration& config)
{
  return std::make_shared<pusch_decoder_factory_sw>(config);
}

std::shared_ptr<pusch_processor_factory>
srsgnb::create_pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config)
{
  return std::make_shared<pusch_processor_factory_sw>(config);
}