/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsran {

/// Creates an LDPC encoder metric decorator factory.
std::shared_ptr<crc_calculator_factory>
create_crc_calculator_metric_decorator_factory(std::shared_ptr<crc_calculator_factory> base_factory,
                                               crc_calculator_metric_notifier&         notifier);

/// Creates an LDPC encoder metric decorator factory.
std::shared_ptr<ldpc_encoder_factory>
create_ldpc_encoder_metric_decorator_factory(std::shared_ptr<ldpc_encoder_factory> base_factory,
                                             ldpc_encoder_metric_notifier&         notifier);

/// Creates an LDPC decoder metric decorator factory.
std::shared_ptr<ldpc_decoder_factory>
create_ldpc_decoder_metric_decorator_factory(std::shared_ptr<ldpc_decoder_factory> base_factory,
                                             ldpc_decoder_metric_notifier&         notifier);

/// Creates a PUSCH channel processor metric decorator factory.
std::shared_ptr<dmrs_pusch_estimator_factory>
create_pusch_channel_estimator_metric_decorator_factory(std::shared_ptr<dmrs_pusch_estimator_factory> base_factory,
                                                        pusch_channel_estimator_metric_notifier&      notifier);

/// Creates PUSCH processor metric decorator factory.
std::shared_ptr<pusch_processor_factory>
create_pusch_processor_metric_decorator_factory(std::shared_ptr<pusch_processor_factory> base_factory,
                                                pusch_processor_metric_notifier&         notifier);

/// Creates pseudo-random generator metric decorator factory.
std::shared_ptr<pseudo_random_generator_factory>
create_pseudo_random_generator_metric_decorator_factory(std::shared_ptr<pseudo_random_generator_factory>  base_factory,
                                                        pseudo_random_sequence_generator_metric_notifier& notifier);

/// Creates channel precoder metric decorator factory.
std::shared_ptr<channel_precoder_factory>
create_channel_precoder_metric_decorator_factory(std::shared_ptr<channel_precoder_factory> base_factory,
                                                 channel_precoder_metric_notifier&         notifier);
/// Creates channel equalizer metric decorator factory.
std::shared_ptr<channel_equalizer_factory>
create_channel_equalizer_metric_decorator_factory(std::shared_ptr<channel_equalizer_factory> base_factory,
                                                  channel_equalizer_metric_notifier&         notifier);

/// Creates a modulation mapper metric decorator factory.
std::shared_ptr<modulation_mapper_factory>
create_modulation_mapper_metric_decorator_factory(std::shared_ptr<modulation_mapper_factory> base_factory,
                                                  common_channel_modulation_metric_notifier& notifier);

/// Creates a demodulation mapper metric decorator factory.
std::shared_ptr<demodulation_mapper_factory>
create_demodulation_mapper_metric_decorator_factory(std::shared_ptr<demodulation_mapper_factory> base_factory,
                                                    common_channel_modulation_metric_notifier&   notifier);

/// Creates an EVM calculator metric decorator factory.
std::shared_ptr<evm_calculator_factory>
create_evm_calculator_metric_decorator_factory(std::shared_ptr<evm_calculator_factory>    base_factory,
                                               common_channel_modulation_metric_notifier& notifier);

} // namespace srsran
