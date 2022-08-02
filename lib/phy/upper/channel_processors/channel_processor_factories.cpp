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
#include "pdcch_encoder_impl.h"
#include "pdcch_modulator_impl.h"
#include "pdsch_encoder_impl.h"
#include "pdsch_modulator_impl.h"
#include "prach_detector_simple_impl.h"
#include "prach_generator_impl.h"
#include "pusch_decoder_impl.h"
#include "pusch_processor_impl.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

namespace {

class pbch_modulator_factory_sw : public pbch_modulator_factory
{
private:
  std::shared_ptr<channel_modulation_factory>      modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pbch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory_,
                            std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(modulator_factory_), prg_factory(prg_factory_)
  {
    report_fatal_error_if_not(modulator_factory, "Invalid modulator factory.");
    report_fatal_error_if_not(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pbch_modulator> create() override
  {
    return std::make_unique<pbch_modulator_impl>(modulator_factory->create_modulation_mapper(), prg_factory->create());
  }
};

class pdcch_modulator_factory_sw : public pdcch_modulator_factory
{
private:
  std::shared_ptr<channel_modulation_factory>      modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pdcch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(modulator_factory_), prg_factory(prg_factory_)
  {
    report_fatal_error_if_not(modulator_factory, "Invalid modulator factory.");
    report_fatal_error_if_not(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pdcch_modulator> create() override
  {
    return std::make_unique<pdcch_modulator_impl>(modulator_factory->create_modulation_mapper(), prg_factory->create());
  }
};

class pdcch_encoder_factory_sw : public pdcch_encoder_factory
{
private:
  std::shared_ptr<crc_calculator_factory> crc_factory;
  std::shared_ptr<polar_factory>          polar_code_factory;

public:
  pdcch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory_,
                           std::shared_ptr<polar_factory>          polar_code_factory_) :
    crc_factory(crc_factory_), polar_code_factory(polar_code_factory_)
  {
    report_fatal_error_if_not(crc_factory, "Invalid CRC calculator factory.");
    report_fatal_error_if_not(polar_code_factory, "Invalid Polar code factory.");
  }

  std::unique_ptr<pdcch_encoder> create() override
  {
    return std::make_unique<pdcch_encoder_impl>(crc_factory->create(crc_generator_poly::CRC24C),
                                                polar_code_factory->create_interleaver(),
                                                polar_code_factory->create_allocator(),
                                                polar_code_factory->create_code(),
                                                polar_code_factory->create_encoder(polar_code::NMAX_LOG),
                                                polar_code_factory->create_rate_matcher());
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
    report_fatal_error_if_not(config.encoder_factory, "Invalid LDPC encoder factory.");
    report_fatal_error_if_not(config.rate_matcher_factory, "Invalid LDPC RM factory.");
    report_fatal_error_if_not(config.segmenter_factory, "Invalid LDPC segmenter factory.");
  }

  std::unique_ptr<pdsch_encoder> create() override
  {
    return std::make_unique<pdsch_encoder_impl>(
        segmenter_factory->create(), encoder_factory->create(), rate_matcher_factory->create());
  }
};

class prach_detector_factory_simple : public prach_detector_factory
{
private:
  std::shared_ptr<dft_processor_factory>   dft_factory;
  std::shared_ptr<prach_generator_factory> prach_gen_factory;
  unsigned                                 dft_size_15kHz;

public:
  prach_detector_factory_simple(std::shared_ptr<dft_processor_factory>   dft_factory_,
                                std::shared_ptr<prach_generator_factory> prach_gen_factory_,
                                unsigned                                 dft_size_15kHz_) :
    dft_factory(dft_factory_), prach_gen_factory(prach_gen_factory_), dft_size_15kHz(dft_size_15kHz_)
  {
    // Do nothing.
  }

  std::unique_ptr<prach_detector> create() override
  {
    dft_processor::configuration dft_config_1_25_kHz;
    dft_config_1_25_kHz.size = (dft_size_15kHz * 15000) / 1250;
    dft_config_1_25_kHz.dir  = dft_processor::direction::DIRECT;
    dft_processor::configuration idft_config_1_25_kHz;
    idft_config_1_25_kHz.size = dft_config_1_25_kHz.size;
    idft_config_1_25_kHz.dir  = dft_processor::direction::INVERSE;
    dft_processor::configuration dft_config_5_kHz;
    dft_config_5_kHz.size = (dft_size_15kHz * 15000) / 5000;
    dft_config_5_kHz.dir  = dft_processor::direction::DIRECT;
    dft_processor::configuration idft_config_5_kHz;
    idft_config_5_kHz.size = dft_config_5_kHz.size;
    idft_config_5_kHz.dir  = dft_processor::direction::INVERSE;
    return std::make_unique<prach_detector_simple_impl>(dft_factory->create(dft_config_1_25_kHz),
                                                        dft_factory->create(idft_config_1_25_kHz),
                                                        dft_factory->create(dft_config_5_kHz),
                                                        dft_factory->create(idft_config_5_kHz),
                                                        prach_gen_factory->create(),
                                                        dft_size_15kHz);
  }
};

class prach_generator_factory_sw : public prach_generator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;
  unsigned                               nof_prb_ul_grid;
  unsigned                               dft_size_15kHz;

public:
  prach_generator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory_,
                             unsigned                               nof_prb_ul_grid_,
                             unsigned                               dft_size_15kHz_) :
    dft_factory(dft_factory_), nof_prb_ul_grid(nof_prb_ul_grid_), dft_size_15kHz(dft_size_15kHz_)
  {
    // Do nothing.
  }

  std::unique_ptr<prach_generator> create() override
  {
    dft_processor::configuration dft_config_l839;
    dft_config_l839.size = 839;
    dft_config_l839.dir  = dft_processor::direction::DIRECT;
    dft_processor::configuration dft_config_l139;
    dft_config_l139.size = 139;
    dft_config_l139.dir  = dft_processor::direction::DIRECT;
    dft_processor::configuration dft_config_1_25_kHz;
    dft_config_1_25_kHz.size = (dft_size_15kHz * 15000) / 1250;
    dft_config_1_25_kHz.dir  = dft_processor::direction::INVERSE;
    dft_processor::configuration dft_config_5_kHz;
    dft_config_5_kHz.size = (dft_size_15kHz * 15000) / 5000;
    dft_config_5_kHz.dir  = dft_processor::direction::INVERSE;
    return std::make_unique<prach_generator_impl>(nof_prb_ul_grid,
                                                  dft_size_15kHz,
                                                  dft_factory->create(dft_config_l839),
                                                  dft_factory->create(dft_config_l139),
                                                  dft_factory->create(dft_config_1_25_kHz),
                                                  dft_factory->create(dft_config_5_kHz),
                                                  nullptr);
  }
};

class pdsch_modulator_factory_sw : public pdsch_modulator_factory
{
private:
  std::shared_ptr<channel_modulation_factory>      modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pdsch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(modulator_factory_), prg_factory(prg_factory_)
  {
    report_fatal_error_if_not(modulator_factory, "Invalid modulator factory.");
    report_fatal_error_if_not(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pdsch_modulator> create() override
  {
    return std::make_unique<pdsch_modulator_impl>(modulator_factory->create_modulation_mapper(), prg_factory->create());
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
    report_fatal_error_if_not(config.crc_factory, "Invalid CRC calculator factory.");
    report_fatal_error_if_not(config.decoder_factory, "Invalid LDPC decoder factory.");
    report_fatal_error_if_not(config.dematcher_factory, "Invalid LDPC dematcher factory.");
    report_fatal_error_if_not(config.segmenter_factory, "Invalid LDPC segmenter factory.");
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
  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  return std::make_unique<pbch_encoder_impl>(create_crc_calculator_factory_sw()->create(crc_generator_poly::CRC24C),
                                             create_pseudo_random_generator_sw_factory()->create(),
                                             polar_factory->create_interleaver(),
                                             polar_factory->create_allocator(),
                                             polar_factory->create_code(),
                                             polar_factory->create_encoder(pbch_encoder::POLAR_N_MAX_LOG),
                                             polar_factory->create_rate_matcher());
}

std::shared_ptr<pbch_modulator_factory>
srsgnb::create_pbch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                         std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pbch_modulator_factory_sw>(modulator_factory, prg_factory);
}

std::shared_ptr<pdcch_modulator_factory>
srsgnb::create_pdcch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                          std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pdcch_modulator_factory_sw>(modulator_factory, prg_factory);
}

std::shared_ptr<pdcch_encoder_factory>
srsgnb::create_pdcch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory,
                                        std::shared_ptr<polar_factory>          encoder_factory)
{
  return std::make_shared<pdcch_encoder_factory_sw>(crc_factory, encoder_factory);
}

std::shared_ptr<pdsch_encoder_factory>
srsgnb::create_pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config)
{
  return std::make_shared<pdsch_encoder_factory_sw>(config);
}

std::shared_ptr<pdsch_modulator_factory>
srsgnb::create_pdsch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                          std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pdsch_modulator_factory_sw>(modulator_factory, prg_factory);
}

std::shared_ptr<prach_detector_factory>
srsgnb::create_prach_detector_factory_simple(std::shared_ptr<dft_processor_factory>   dft_factory,
                                             std::shared_ptr<prach_generator_factory> prach_gen_factory,
                                             unsigned                                 dft_size_15kHz)
{
  return std::make_shared<prach_detector_factory_simple>(dft_factory, prach_gen_factory, dft_size_15kHz);
}

std::shared_ptr<prach_generator_factory>
srsgnb::create_prach_generator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory,
                                          unsigned                               nof_prb_ul_grid,
                                          unsigned                               dft_size_15kHz)
{
  return std::make_shared<prach_generator_factory_sw>(dft_factory, nof_prb_ul_grid, dft_size_15kHz);
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