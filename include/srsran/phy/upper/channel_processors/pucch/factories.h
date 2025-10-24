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

#include "srsran/phy/generic_functions/transform_precoding/transform_precoding_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/uci/factories.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/pucch/factories.h"
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
                                    std::shared_ptr<demodulation_mapper_factory>     demodulation_factory,
                                    std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                    std::shared_ptr<transform_precoder_factory>      precoder_factory);

class pucch_detector_factory
{
public:
  virtual ~pucch_detector_factory()                = default;
  virtual std::unique_ptr<pucch_detector> create() = 0;
};

std::shared_ptr<pucch_detector_factory>
create_pucch_detector_factory_sw(std::shared_ptr<low_papr_sequence_collection_factory> lpcf,
                                 std::shared_ptr<pseudo_random_generator_factory>      prgf,
                                 std::shared_ptr<channel_equalizer_factory>            eqzrf,
                                 std::shared_ptr<dft_processor_factory>                dftf);

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
