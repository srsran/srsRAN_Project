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
#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/upper_phy_factories.h"

namespace srsran {

/// Creates an LDPC encoder metric decorator factory.
std::shared_ptr<crc_calculator_factory>
create_crc_calculator_metric_decorator_factory(std::shared_ptr<crc_calculator_factory> base_factory,
                                               crc_calculator_metric_notifier&         notifier);

/// Creates an LDPC encoder metric decorator factory.
std::shared_ptr<ldpc_encoder_factory>
create_ldpc_encoder_metric_decorator_factory(std::shared_ptr<ldpc_encoder_factory> base_factory,
                                             ldpc_encoder_metric_notifier&         notifier);
/// Creates an LDPC rate matcher metric decorator factory.
std::shared_ptr<ldpc_rate_matcher_factory>
create_ldpc_rate_matcher_metric_decorator_factory(std::shared_ptr<ldpc_rate_matcher_factory> base_factory,
                                                  ldpc_rate_matcher_metric_notifier&         notifier);

/// Creates an LDPC rate dematcher metric decorator factory.
std::shared_ptr<ldpc_rate_dematcher_factory>
create_ldpc_rate_dematcher_metric_decorator_factory(std::shared_ptr<ldpc_rate_dematcher_factory> base_factory,
                                                    ldpc_rate_dematcher_metric_notifier&         notifier);

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

/// Creates a UL-SCH demultiplex metric decorator factory.
std::shared_ptr<ulsch_demultiplex_factory>
create_ulsch_demultiplex_metric_decorator_factory(std::shared_ptr<ulsch_demultiplex_factory> base_factory,
                                                  ulsch_demultiplex_metric_notifier&         notifier);

/// Creates a PUSCH demodulator metric decorator factory.
std::shared_ptr<pusch_demodulator_factory>
create_pusch_demodulator_metric_decorator_factory(std::shared_ptr<pusch_demodulator_factory> base_factory,
                                                  pusch_demodulator_metric_notifier&         notifier);

/// Creates a time alignment estimator metric decorator factory.
std::shared_ptr<time_alignment_estimator_factory>
create_time_alignment_estimator_metric_decorator_factory(std::shared_ptr<time_alignment_estimator_factory> base_factory,
                                                         time_alignment_estimator_metric_notifier&         notifier);

/// Creates a port channel estimator metric decorator factory.
std::shared_ptr<port_channel_estimator_factory>
create_port_channel_estimator_metric_decorator_factory(std::shared_ptr<port_channel_estimator_factory> base_factory,
                                                       port_channel_estimator_metric_notifier&         notifier);

/// Creates a transform precoder metric decorator factory.
std::shared_ptr<transform_precoder_factory>
create_transform_precoder_metric_decorator_factory(std::shared_ptr<transform_precoder_factory> base_factory,
                                                   transform_precoder_metric_notifier&         notifier);

/// Creates a PDSCH processor metric decorator factory.
std::shared_ptr<pdsch_processor_factory>
create_pdsch_processor_metric_decorator_factory(std::shared_ptr<pdsch_processor_factory> base_factory,
                                                pdsch_processor_metric_notifier&         notifier);

/// Creates a PDSCH DM-RS generator metric decorator factory.
std::shared_ptr<dmrs_pdsch_processor_factory>
create_dmrs_pdsch_generator_metric_decorator_factory(std::shared_ptr<dmrs_pdsch_processor_factory> base_factory,
                                                     pdsch_dmrs_generator_metric_notifier&         notifier);

/// Creates a downlink processor metric decorator factory.
std::shared_ptr<downlink_processor_factory>
create_downlink_processor_generator_metric_decorator_factory(std::shared_ptr<downlink_processor_factory> base_factory,
                                                             downlink_processor_metric_notifier&         notifier);

} // namespace srsran
