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

#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/uci/factories.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class pucch_demodulator_factory
{
public:
  virtual ~pucch_demodulator_factory()                = default;
  virtual std::unique_ptr<pucch_demodulator> create() = 0;
};

std::shared_ptr<pucch_demodulator_factory>
create_pucch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory,
                                    std::shared_ptr<channel_modulation_factory>      demodulation_factory,
                                    std::shared_ptr<pseudo_random_generator_factory> prg_factory);

class pucch_detector_factory
{
public:
  virtual ~pucch_detector_factory()                = default;
  virtual std::unique_ptr<pucch_detector> create() = 0;
};

std::shared_ptr<pucch_detector_factory>
create_pucch_detector_factory_sw(std::shared_ptr<low_papr_sequence_collection_factory> lpcf,
                                 std::shared_ptr<pseudo_random_generator_factory>      prgf,
                                 std::shared_ptr<channel_equalizer_factory>            eqzrf);

class pucch_processor_factory
{
public:
  virtual ~pucch_processor_factory()                              = default;
  virtual std::unique_ptr<pucch_processor>     create()           = 0;
  virtual std::unique_ptr<pucch_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<pucch_processor>     create(srslog::basic_logger& logger);
};

std::shared_ptr<pucch_processor_factory>
create_pucch_processor_factory_sw(std::shared_ptr<dmrs_pucch_estimator_factory>        dmrs_factory,
                                  std::shared_ptr<pucch_detector_factory>              detector_factory,
                                  std::shared_ptr<pucch_demodulator_factory>           demodulator_factory,
                                  std::shared_ptr<uci_decoder_factory>                 decoder_factory,
                                  const channel_estimate::channel_estimate_dimensions& channel_estimate_dimensions);

std::shared_ptr<pucch_processor_factory>
create_pucch_processor_pool_factory(std::shared_ptr<pucch_processor_factory> factory, unsigned nof_concurrent_threads);

} // namespace srsran