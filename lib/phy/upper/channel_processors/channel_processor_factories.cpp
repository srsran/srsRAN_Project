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
#include "pdcch_processor_impl.h"
#include "pdsch_encoder_impl.h"
#include "pdsch_modulator_impl.h"
#include "pdsch_processor_impl.h"
#include "prach_detector_simple_impl.h"
#include "prach_generator_impl.h"
#include "pucch_processor_impl.h"
#include "pusch_decoder_impl.h"
#include "pusch_demodulator_impl.h"
#include "pusch_processor_impl.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "ssb_processor_impl.h"

using namespace srsgnb;

namespace {

class pbch_encoder_factory_sw : public pbch_encoder_factory
{
public:
  pbch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_calc_factory_,
                          std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                          std::shared_ptr<polar_factory>                   polar_fec_factory_) :
    crc_calc_factory(std::move(crc_calc_factory_)),
    prg_factory(std::move(prg_factory_)),
    polar_fec_factory(std::move(polar_fec_factory_))
  {
    srsgnb_assert(crc_calc_factory, "Invalid CRC factory.");
    srsgnb_assert(prg_factory, "Invalid CRC factory.");
    srsgnb_assert(polar_fec_factory, "Invalid polar factory.");
  }

  std::unique_ptr<pbch_encoder> create() override
  {
    return std::make_unique<pbch_encoder_impl>(create_crc_calculator_factory_sw()->create(crc_generator_poly::CRC24C),
                                               create_pseudo_random_generator_sw_factory()->create(),
                                               polar_fec_factory->create_interleaver(),
                                               polar_fec_factory->create_allocator(),
                                               polar_fec_factory->create_code(),
                                               polar_fec_factory->create_encoder(),
                                               polar_fec_factory->create_rate_matcher());
  }

private:
  std::shared_ptr<crc_calculator_factory>          crc_calc_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<polar_factory>                   polar_fec_factory;
};

class pbch_modulator_factory_sw : public pbch_modulator_factory
{
private:
  std::shared_ptr<channel_modulation_factory>      modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pbch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory_,
                            std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(std::move(modulator_factory_)), prg_factory(std::move(prg_factory_))
  {
    srsgnb_assert(modulator_factory, "Invalid modulator factory.");
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
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
    modulator_factory(std::move(modulator_factory_)), prg_factory(std::move(prg_factory_))
  {
    srsgnb_assert(modulator_factory, "Invalid modulator factory.");
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
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
    crc_factory(std::move(crc_factory_)), polar_code_factory(std::move(polar_code_factory_))
  {
    srsgnb_assert(crc_factory, "Invalid CRC calculator factory.");
    srsgnb_assert(polar_code_factory, "Invalid Polar code factory.");
  }

  std::unique_ptr<pdcch_encoder> create() override
  {
    return std::make_unique<pdcch_encoder_impl>(crc_factory->create(crc_generator_poly::CRC24C),
                                                polar_code_factory->create_interleaver(),
                                                polar_code_factory->create_allocator(),
                                                polar_code_factory->create_code(),
                                                polar_code_factory->create_encoder(),
                                                polar_code_factory->create_rate_matcher());
  }
};

class pdcch_processor_factory_sw : public pdcch_processor_factory
{
private:
  std::shared_ptr<pdcch_encoder_factory>        encoder_factory;
  std::shared_ptr<pdcch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory;

public:
  pdcch_processor_factory_sw(std::shared_ptr<pdcch_encoder_factory>        encoder_factory_,
                             std::shared_ptr<pdcch_modulator_factory>      modulator_factory_,
                             std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory_) :
    encoder_factory(std::move(encoder_factory_)),
    modulator_factory(std::move(modulator_factory_)),
    dmrs_factory(std::move(dmrs_factory_))
  {
    srsgnb_assert(encoder_factory, "Invalid encoder factory.");
    srsgnb_assert(modulator_factory, "Invalid modulator factory.");
    srsgnb_assert(dmrs_factory, "Invalid DMRS factory.");
  }

  std::unique_ptr<pdcch_processor> create() override
  {
    return std::make_unique<pdcch_processor_impl>(
        encoder_factory->create(), modulator_factory->create(), dmrs_factory->create());
  }
};

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
    srsgnb_assert(encoder_factory, "Invalid LDPC encoder factory.");
    srsgnb_assert(rate_matcher_factory, "Invalid LDPC RM factory.");
    srsgnb_assert(segmenter_factory, "Invalid LDPC segmenter factory.");
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
  std::shared_ptr<channel_modulation_factory>      modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pdsch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(std::move(modulator_factory_)), prg_factory(std::move(prg_factory_))
  {
    srsgnb_assert(modulator_factory, "Invalid modulator factory.");
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pdsch_modulator> create() override
  {
    return std::make_unique<pdsch_modulator_impl>(modulator_factory->create_modulation_mapper(), prg_factory->create());
  }
};

class pdsch_processor_factory_sw : public pdsch_processor_factory
{
private:
  std::shared_ptr<pdsch_encoder_factory>        encoder_factory;
  std::shared_ptr<pdsch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory;

public:
  pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>        encoder_factory_,
                             std::shared_ptr<pdsch_modulator_factory>      modulator_factory_,
                             std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory_) :
    encoder_factory(std::move(encoder_factory_)),
    modulator_factory(std::move(modulator_factory_)),
    dmrs_factory(std::move(dmrs_factory_))
  {
    srsgnb_assert(encoder_factory, "Invalid encoder factory.");
    srsgnb_assert(modulator_factory, "Invalid modulator factory.");
    srsgnb_assert(dmrs_factory, "Invalid DMRS factory.");
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    return std::make_unique<pdsch_processor_impl>(
        encoder_factory->create(), modulator_factory->create(), dmrs_factory->create());
  }
};

class prach_detector_factory_simple : public prach_detector_factory
{
private:
  std::shared_ptr<dft_processor_factory>   dft_factory;
  std::shared_ptr<prach_generator_factory> prach_gen_factory;
  unsigned                                 dft_size_detector;

public:
  prach_detector_factory_simple(std::shared_ptr<dft_processor_factory>   dft_factory_,
                                std::shared_ptr<prach_generator_factory> prach_gen_factory_,
                                unsigned                                 dft_size_detector_) :
    dft_factory(std::move(dft_factory_)),
    prach_gen_factory(std::move(prach_gen_factory_)),
    dft_size_detector(dft_size_detector_)
  {
    srsgnb_assert(dft_factory, "Invalid DFT factory.");
    srsgnb_assert(prach_gen_factory, "Invalid PRACH generator factory.");
    srsgnb_assert(dft_size_detector, "Invalid DFT size.");
  }

  std::unique_ptr<prach_detector> create() override
  {
    dft_processor::configuration idft_config = {};
    idft_config.size                         = dft_size_detector;
    idft_config.dir                          = dft_processor::direction::INVERSE;
    return std::make_unique<prach_detector_simple_impl>(dft_factory->create(idft_config), prach_gen_factory->create());
  }
};

class prach_generator_factory_sw : public prach_generator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;

public:
  explicit prach_generator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory_) :
    dft_factory(std::move(dft_factory_))
  {
    srsgnb_assert(dft_factory, "Invalid DFT factory.");
  }

  std::unique_ptr<prach_generator> create() override
  {
    dft_processor::configuration dft_config_l839 = {};
    dft_config_l839.size                         = 839;
    dft_config_l839.dir                          = dft_processor::direction::DIRECT;
    dft_processor::configuration dft_config_l139 = {};
    dft_config_l139.size                         = 139;
    dft_config_l139.dir                          = dft_processor::direction::DIRECT;
    return std::make_unique<prach_generator_impl>(dft_factory->create(dft_config_l839),
                                                  dft_factory->create(dft_config_l139));
  }
};

class pucch_processor_factory_sw : public pucch_processor_factory
{
public:
  explicit pucch_processor_factory_sw()
  {
    // Do nothing.
  }

  std::unique_ptr<pucch_processor> create() override { return std::make_unique<pucch_processor_impl>(); }
};

class pusch_decoder_factory_sw : public pusch_decoder_factory
{
private:
  std::shared_ptr<crc_calculator_factory>      crc_factory;
  std::shared_ptr<ldpc_decoder_factory>        decoder_factory;
  std::shared_ptr<ldpc_rate_dematcher_factory> dematcher_factory;
  std::shared_ptr<ldpc_segmenter_rx_factory>   segmenter_factory;

public:
  explicit pusch_decoder_factory_sw(pusch_decoder_factory_sw_configuration& config) :
    crc_factory(std::move(config.crc_factory)),
    decoder_factory(std::move(config.decoder_factory)),
    dematcher_factory(std::move(config.dematcher_factory)),
    segmenter_factory(std::move(config.segmenter_factory))
  {
    srsgnb_assert(crc_factory, "Invalid CRC calculator factory.");
    srsgnb_assert(decoder_factory, "Invalid LDPC decoder factory.");
    srsgnb_assert(dematcher_factory, "Invalid LDPC dematcher factory.");
    srsgnb_assert(segmenter_factory, "Invalid LDPC segmenter factory.");
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

class pusch_demodulator_factory_sw : public pusch_demodulator_factory
{
public:
  std::unique_ptr<pusch_demodulator> create() override
  {
    return std::make_unique<pusch_demodulator_impl>(
        equalizer_factory->create(), demodulation_factory->create_demodulation_mapper(), prg_factory->create());
  }

  pusch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory_,
                               std::shared_ptr<channel_modulation_factory>      demodulation_factory_,
                               std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    equalizer_factory(std::move(equalizer_factory_)),
    demodulation_factory(std::move(demodulation_factory_)),
    prg_factory(std::move(prg_factory_))
  {
    srsgnb_assert(equalizer_factory, "Invalid equalizer factory.");
    srsgnb_assert(demodulation_factory, "Invalid demodulation factory.");
    srsgnb_assert(prg_factory, "Invalid PRG factory.");
  }

private:
  std::shared_ptr<channel_equalizer_factory>       equalizer_factory;
  std::shared_ptr<channel_modulation_factory>      demodulation_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
};

class pusch_processor_factory_sw : public pusch_processor_factory
{
public:
  explicit pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config) :
    estimator_factory(config.estimator_factory),
    demodulator_factory(config.demodulator_factory),
    decoder_factory(config.decoder_factory),
    ch_estimate_dimensions(config.ch_estimate_dimensions)
  {
    srsgnb_assert(estimator_factory, "Invalid channel estimation factory.");
    srsgnb_assert(demodulator_factory, "Invalid demodulation factory.");
    srsgnb_assert(decoder_factory, "Invalid decoder factory.");
  }

  std::unique_ptr<pusch_processor> create() override
  {
    pusch_processor_configuration config;
    config.estimator   = estimator_factory->create();
    config.decoder     = decoder_factory->create();
    config.demodulator = demodulator_factory->create();
    config.ce_dims     = ch_estimate_dimensions;
    return std::make_unique<pusch_processor_impl>(config);
  }

private:
  std::shared_ptr<dmrs_pusch_estimator_factory> estimator_factory;
  std::shared_ptr<pusch_demodulator_factory>    demodulator_factory;
  std::shared_ptr<pusch_decoder_factory>        decoder_factory;
  channel_estimate::channel_estimate_dimensions ch_estimate_dimensions;
};

class ssb_processor_factory_sw : public ssb_processor_factory
{
public:
  explicit ssb_processor_factory_sw(ssb_processor_factory_sw_configuration& config) :
    encoder_factory(std::move(config.encoder_factory)),
    modulator_factory(std::move(config.modulator_factory)),
    dmrs_factory(std::move(config.dmrs_factory)),
    pss_factory(std::move(config.pss_factory)),
    sss_factory(std::move(config.sss_factory))
  {
    srsgnb_assert(encoder_factory, "Invalid encoder factory");
    srsgnb_assert(modulator_factory, "Invalid modulator factory");
    srsgnb_assert(dmrs_factory, "Invalid DMRS factory");
    srsgnb_assert(pss_factory, "Invalid PSS factory");
    srsgnb_assert(sss_factory, "Invalid SSS factory");
  }

  std::unique_ptr<ssb_processor> create() override
  {
    ssb_processor_config config;
    config.encoder   = encoder_factory->create();
    config.modulator = modulator_factory->create();
    config.dmrs      = dmrs_factory->create();
    config.pss       = pss_factory->create();
    config.sss       = sss_factory->create();

    return std::make_unique<ssb_processor_impl>(std::move(config));
  }

private:
  std::shared_ptr<pbch_encoder_factory>        encoder_factory;
  std::shared_ptr<pbch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_factory;
  std::shared_ptr<pss_processor_factory>       pss_factory;
  std::shared_ptr<sss_processor_factory>       sss_factory;
};

} // namespace

std::shared_ptr<pbch_encoder_factory>
srsgnb::create_pbch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_factory,
                                       std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                       std::shared_ptr<polar_factory>                   polar_factory)
{
  return std::make_shared<pbch_encoder_factory_sw>(
      std::move(crc_factory), std::move(prg_factory), std::move(polar_factory));
}

std::shared_ptr<pbch_modulator_factory>
srsgnb::create_pbch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                         std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pbch_modulator_factory_sw>(std::move(modulator_factory), std::move(prg_factory));
}

std::shared_ptr<pdcch_encoder_factory>
srsgnb::create_pdcch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory,
                                        std::shared_ptr<polar_factory>          encoder_factory)
{
  return std::make_shared<pdcch_encoder_factory_sw>(std::move(crc_factory), std::move(encoder_factory));
}

std::shared_ptr<pdcch_modulator_factory>
srsgnb::create_pdcch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                          std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pdcch_modulator_factory_sw>(std::move(modulator_factory), std::move(prg_factory));
}

std::shared_ptr<pdcch_processor_factory>
srsgnb::create_pdcch_processor_factory_sw(std::shared_ptr<pdcch_encoder_factory>        encoder_factory,
                                          std::shared_ptr<pdcch_modulator_factory>      modulator_factory,
                                          std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory)
{
  return std::make_shared<pdcch_processor_factory_sw>(
      std::move(encoder_factory), std::move(modulator_factory), std::move(dmrs_factory));
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
  return std::make_shared<pdsch_modulator_factory_sw>(std::move(modulator_factory), std::move(prg_factory));
}

std::shared_ptr<pdsch_processor_factory>
srsgnb::create_pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>        encoder_factory,
                                          std::shared_ptr<pdsch_modulator_factory>      modulator_factory,
                                          std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory)
{
  return std::make_shared<pdsch_processor_factory_sw>(
      std::move(encoder_factory), std::move(modulator_factory), std::move(dmrs_factory));
}

std::shared_ptr<prach_detector_factory>
srsgnb::create_prach_detector_factory_simple(std::shared_ptr<dft_processor_factory>   dft_factory,
                                             std::shared_ptr<prach_generator_factory> prach_gen_factory,
                                             unsigned                                 dft_size_detector)
{
  return std::make_shared<prach_detector_factory_simple>(
      std::move(dft_factory), std::move(prach_gen_factory), dft_size_detector);
}

std::shared_ptr<prach_generator_factory>
srsgnb::create_prach_generator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory)
{
  return std::make_shared<prach_generator_factory_sw>(std::move(dft_factory));
}

std::shared_ptr<pusch_decoder_factory>
srsgnb::create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_configuration& config)
{
  return std::make_shared<pusch_decoder_factory_sw>(config);
}

std::shared_ptr<pusch_demodulator_factory>
srsgnb::create_pusch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory,
                                            std::shared_ptr<channel_modulation_factory>      demodulation_factory,
                                            std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pusch_demodulator_factory_sw>(
      std::move(equalizer_factory), std::move(demodulation_factory), std::move(prg_factory));
}

std::shared_ptr<pusch_processor_factory>
srsgnb::create_pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config)
{
  return std::make_shared<pusch_processor_factory_sw>(config);
}

std::shared_ptr<ssb_processor_factory>
srsgnb::create_ssb_processor_factory_sw(ssb_processor_factory_sw_configuration& config)
{
  return std::make_shared<ssb_processor_factory_sw>(config);
}
