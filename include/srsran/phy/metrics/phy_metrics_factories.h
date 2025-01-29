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

} // namespace srsran
