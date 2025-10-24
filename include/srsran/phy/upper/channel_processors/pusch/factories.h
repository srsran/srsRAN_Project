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

#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoding_factories.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/ulsch_demultiplex.h"
#include "srsran/phy/upper/channel_processors/uci/factories.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/phy/upper/signal_processors/pusch/factories.h"
#include "srsran/srslog/logger.h"
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

/// \brief Creates a factory for the empty PUSCH decoder.
///
/// See \ref pusch_decoder_empty_impl for more details on the implementation.
///
/// \param[in] nof_prb     Maximum number of PRB.
/// \param[in] nof_layers  Maximum number of layers.
/// \return An instance of PUSCH decoder factory.
std::shared_ptr<pusch_decoder_factory> create_pusch_decoder_empty_factory(unsigned nof_prb, unsigned nof_layers);

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
  unsigned                                               nof_pusch_decoder_threads;
  task_executor*                                         executor;
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
                                    std::shared_ptr<transform_precoder_factory>      precoder_factory,
                                    std::shared_ptr<demodulation_mapper_factory>     demodulation_factory,
                                    std::shared_ptr<evm_calculator_factory>          evm_calc_factory,
                                    std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                    unsigned                                         max_nof_prb,
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
  bool                                          dec_force_decoding    = false;
  unsigned                                      max_nof_concurrent_threads;
  channel_state_information::sinr_type csi_sinr_calc_method = channel_state_information::sinr_type::channel_estimator;
};

std::shared_ptr<pusch_processor_factory>
create_pusch_processor_factory_sw(pusch_processor_factory_sw_configuration& config);

/// \brief Collection of parameters necessary to build a PUSCH processor pool.
///
/// The PUSCH processor pool uses the regular factory for the asynchronous PUSCH decoding. When the regular PUSCH
/// processors are all reserved, it uses the processors created from the UCI only factory.
///
/// The UCI factory processors are executed synchronously and perform the exact same tasks as a regular PUSCH processor
/// except the shared channel decoding is skipped.
struct pusch_processor_pool_factory_config {
  /// PUSCH processor factory for regular processing.
  std::shared_ptr<pusch_processor_factory> factory;
  /// PUSCH processor factory for UCI only.
  std::shared_ptr<pusch_processor_factory> uci_factory;
  /// \brief Number of regular PUSCH processors.
  ///
  /// This is the maximum number of PUSCH processors that can be in the process of decoding shared channel. The number
  /// of regular processors is independent from the number of threads.
  unsigned nof_regular_processors;
  /// \brief Number of UCI PUSCH processors.
  ///
  /// This is the number of PUSCH processors that process only UCI in case all the regular PUSCH processors are
  /// reserved. The number of UCI processors must be equal to the number of threads that will use the PUSCH processor
  /// pool.
  unsigned nof_uci_processors;
};

/// Creates a PUSCH processor pool.
std::shared_ptr<pusch_processor_factory> create_pusch_processor_pool(pusch_processor_pool_factory_config& config);

class ulsch_demultiplex_factory
{
public:
  virtual ~ulsch_demultiplex_factory()                = default;
  virtual std::unique_ptr<ulsch_demultiplex> create() = 0;
};

std::shared_ptr<ulsch_demultiplex_factory> create_ulsch_demultiplex_factory_sw();

} // namespace srsran
