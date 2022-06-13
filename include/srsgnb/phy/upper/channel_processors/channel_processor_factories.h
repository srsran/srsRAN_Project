/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_CHANNEL_PROCESSOR_FACTORIES_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_CHANNEL_PROCESSOR_FACTORIES_H

#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pbch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pusch_decoder.h"
#include "srsgnb/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include <memory>

namespace srsgnb {

std::unique_ptr<pbch_encoder> create_pbch_encoder();

class pbch_modulator_factory
{
public:
  virtual ~pbch_modulator_factory()                = default;
  virtual std::unique_ptr<pbch_modulator> create() = 0;
};

std::shared_ptr<pbch_modulator_factory>
    create_pbch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>,
                                     std::shared_ptr<pseudo_random_generator_factory>);

class pdcch_modulator_factory
{
public:
  virtual ~pdcch_modulator_factory()                = default;
  virtual std::unique_ptr<pdcch_modulator> create() = 0;
};

std::shared_ptr<pdcch_modulator_factory>
    create_pdcch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>,
                                      std::shared_ptr<pseudo_random_generator_factory>);

class pdsch_encoder_factory
{
public:
  virtual ~pdsch_encoder_factory()                = default;
  virtual std::unique_ptr<pdsch_encoder> create() = 0;
};

class pdsch_modulator_factory
{
public:
  virtual ~pdsch_modulator_factory()                = default;
  virtual std::unique_ptr<pdsch_modulator> create() = 0;
};

std::shared_ptr<pdsch_modulator_factory>
    create_pdsch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>,
                                      std::shared_ptr<pseudo_random_generator_factory>);

/// Describes the software PDSCH encoder factory configuration.
struct pdsch_encoder_factory_sw_configuration {
  /// LDPC encoder factory.
  std::shared_ptr<ldpc_encoder_factory> encoder_factory;
  /// LDPC rate matcher factory.
  std::shared_ptr<ldpc_rate_matcher_factory> rate_matcher_factory;
  /// LDPC Transmit segmenter factory.
  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory;
};

std::shared_ptr<pdsch_encoder_factory> create_pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config);

class pusch_decoder_factory
{
public:
  virtual ~pusch_decoder_factory()                = default;
  virtual std::unique_ptr<pusch_decoder> create() = 0;
};

/// Describes the software PUSCH decoder factory configuration.
struct pusch_decoder_factory_sw_configuration {
  /// CRC calculator factory.
  std::shared_ptr<crc_calculator_factory> crc_factory;
  /// LDPC decoder factory.
  std::shared_ptr<ldpc_decoder_factory> decoder_factory;
  /// LDPC rate dematcher factory.
  std::shared_ptr<ldpc_rate_dematcher_factory> dematcher_factory;
  /// LDPC Receive segmenter factory.
  std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_factory;
};

std::shared_ptr<pusch_decoder_factory> create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_configuration& config);

class pusch_demodulator_factory
{
public:
  virtual ~pusch_demodulator_factory()                = default;
  virtual std::unique_ptr<pusch_demodulator> create() = 0;
};

class pusch_processor_factory
{
public:
  virtual ~pusch_processor_factory()                = default;
  virtual std::unique_ptr<pusch_processor> create() = 0;
};

/// Describes the software PUSCH processor factory configuration.
struct pusch_processor_factory_sw_configuration {
  // TBD.
};

std::shared_ptr<pusch_processor_factory>
create_pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_CHANNEL_PROCESSOR_FACTORIES_H
