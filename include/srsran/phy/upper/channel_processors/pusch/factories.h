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

#pragma once

#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/ulsch_demultiplex.h"
#include "srsran/phy/upper/channel_processors/uci/factories.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include <memory>

namespace srsran {

class ulsch_demultiplex_factory;
class task_executor;

class pusch_decoder_factory
{
public:
  virtual ~pusch_decoder_factory()                = default;
  virtual std::unique_ptr<pusch_decoder> create() = 0;
};

struct pusch_decoder_factory_sw_configuration {
  std::shared_ptr<crc_calculator_factory>      crc_factory;
  std::shared_ptr<ldpc_decoder_factory>        decoder_factory;
  std::shared_ptr<ldpc_rate_dematcher_factory> dematcher_factory;
  std::shared_ptr<ldpc_segmenter_rx_factory>   segmenter_factory;
  unsigned                                     nof_pusch_decoder_threads = 1;
  task_executor*                               executor                  = nullptr;
  unsigned                                     nof_prb;
  unsigned                                     nof_layers;
};

std::shared_ptr<pusch_decoder_factory> create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_configuration config);

/// HW-accelerated PUSCH decoder factory configuration parameters.
struct pusch_decoder_factory_hw_configuration {
  std::shared_ptr<ldpc_segmenter_rx_factory>             segmenter_factory;
  std::shared_ptr<crc_calculator_factory>                crc_factory;
  std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hw_decoder_factory;
};

std::shared_ptr<pusch_decoder_factory>
create_pusch_decoder_factory_hw(const pusch_decoder_factory_hw_configuration& config);

class pusch_demodulator_factory
{
public:
  virtual ~pusch_demodulator_factory()                = default;
  virtual std::unique_ptr<pusch_demodulator> create() = 0;
};

std::shared_ptr<pusch_demodulator_factory>
create_pusch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory,
                                    std::shared_ptr<channel_modulation_factory>      demodulation_factory,
                                    std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                    bool                                             enable_evm          = false,
                                    bool                                             enable_post_eq_sinr = false);

class pusch_processor_factory
{
public:
  virtual ~pusch_processor_factory()                              = default;
  virtual std::unique_ptr<pusch_processor>     create()           = 0;
  virtual std::unique_ptr<pusch_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<pusch_processor>     create(srslog::basic_logger& logger);
};

struct pusch_processor_factory_sw_configuration {
  std::shared_ptr<dmrs_pusch_estimator_factory> estimator_factory;
  std::shared_ptr<pusch_demodulator_factory>    demodulator_factory;
  std::shared_ptr<ulsch_demultiplex_factory>    demux_factory;
  std::shared_ptr<pusch_decoder_factory>        decoder_factory;
  std::shared_ptr<uci_decoder_factory>          uci_dec_factory;
  channel_estimate::channel_estimate_dimensions ch_estimate_dimensions;
  unsigned                                      dec_nof_iterations    = 10;
  bool                                          dec_enable_early_stop = true;
  unsigned                                      max_nof_concurrent_threads;
  channel_state_information::sinr_type csi_sinr_calc_method = channel_state_information::sinr_type::channel_estimator;
};

std::shared_ptr<pusch_processor_factory>
create_pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config);

std::shared_ptr<pusch_processor_factory> create_pusch_processor_pool(std::shared_ptr<pusch_processor_factory> factory,
                                                                     unsigned max_nof_processors,
                                                                     bool     blocking = false);

class ulsch_demultiplex_factory
{
public:
  virtual ~ulsch_demultiplex_factory()                = default;
  virtual std::unique_ptr<ulsch_demultiplex> create() = 0;
};

std::shared_ptr<ulsch_demultiplex_factory> create_ulsch_demultiplex_factory_sw();

} // namespace srsran
