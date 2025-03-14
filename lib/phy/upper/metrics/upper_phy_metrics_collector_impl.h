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

#include "aggregators/ulsch_demultiplex_metrics_aggregator.h"
#include "upper_phy_downlink_processor_metrics_collector.h"
#include "upper_phy_ldpc_metrics_collector.h"
#include "upper_phy_pdsch_metrics_collector.h"
#include "upper_phy_pusch_metrics_collector.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/upper/upper_phy_metrics.h"
#include "srsran/phy/upper/upper_phy_metrics_collector.h"

namespace srsran {

/// O-RAN DU low metrics collector implementation.
class upper_phy_metrics_collector_impl : public upper_phy_metrics_collector, public upper_phy_metrics_notifiers
{
public:
  // See interface for documentation.
  void collect_metrics(upper_phy_metrics& metrics) override;

private:
  // See interface for documentation.
  crc_calculator_metric_notifier& get_pdsch_crc_calculator_notifier() override
  {
    return pdsch_collector.get_pdsch_crc_calculator_notifier();
  }

  // See interface for documentation.
  crc_calculator_metric_notifier& get_pusch_crc_calculator_notifier() override
  {
    return pusch_collector.get_pusch_crc_calculator_notifier();
  }

  // See interface for documentation.
  ldpc_decoder_metric_notifier& get_ldpc_decoder_notifier() override
  {
    return ldpc_collector.get_ldpc_decoder_notifier();
  }

  // See interface for documentation.
  ldpc_encoder_metric_notifier& get_ldpc_encoder_notifier() override
  {
    return ldpc_collector.get_ldpc_encoder_notifier();
  }

  // See interface for documentation.
  ldpc_rate_matcher_metric_notifier& get_ldpc_rate_matcher_notifier() override
  {
    return ldpc_collector.get_ldpc_rate_matcher_notifier();
  }

  // See interface for documentation.
  ldpc_rate_dematcher_metric_notifier& get_ldpc_rate_dematcher_notifier() override
  {
    return ldpc_collector.get_ldpc_rate_dematcher_notifier();
  }

  // See interface for documentation.
  pusch_processor_metric_notifier& get_pusch_processor_notifier() override
  {
    return pusch_collector.get_pusch_processor_notifier();
  }

  // See interface for documentation.
  pusch_channel_estimator_metric_notifier& get_pusch_channel_estimator_notifier() override
  {
    return pusch_collector.get_pusch_channel_estimator_notifier();
  }

  // See interface for documentation.
  pseudo_random_sequence_generator_metric_notifier& get_pusch_scrambling_notifier() override
  {
    return pusch_collector.get_pusch_scrambling_notifier();
  }

  // See interface for documentation.
  channel_equalizer_metric_notifier& get_pusch_channel_equalizer_notifier() override
  {
    return pusch_collector.get_pusch_channel_equalizer_notifier();
  }

  // See interface for documentation.
  pseudo_random_sequence_generator_metric_notifier& get_pdsch_scrambling_notifier() override
  {
    return pdsch_collector.get_pdsch_scrambling_notifier();
  }

  // See interface for documentation.
  channel_precoder_metric_notifier& get_pdsch_channel_precoder_notifier() override
  {
    return pdsch_collector.get_pdsch_channel_precoder_notifier();
  }

  // See interface for documentation.
  common_channel_modulation_metric_notifier& get_pdsch_modulation_mapper_notifier() override
  {
    return pdsch_collector.get_pdsch_modulation_mapper_notifier();
  }

  // See interface for documentation.
  common_channel_modulation_metric_notifier& get_pusch_demodulation_mapper_notifier() override
  {
    return pusch_collector.get_pusch_demodulation_mapper_notifier();
  }

  // See interface for documentation.
  common_channel_modulation_metric_notifier& get_pusch_evm_calculator_notifier() override
  {
    return pusch_collector.get_pusch_evm_calculator_notifier();
  }

  // See interface for documentation.
  ulsch_demultiplex_metric_notifier& get_ulsch_demultiplex_notifier() override { return ulsch_demultiplex_aggregator; }

  // See interface for documentation.
  pusch_demodulator_metric_notifier& get_pusch_demodulator_notifier() override
  {
    return pusch_collector.get_pusch_demodulator_notifier();
  }

  // See interface for documentation.
  time_alignment_estimator_metric_notifier& get_pusch_ta_estimator_notifier() override
  {
    return pusch_collector.get_pusch_ta_estimator_notifier();
  }

  // See interface for documentation.
  port_channel_estimator_metric_notifier& get_pusch_port_channel_estimator_notifier() override
  {
    return pusch_collector.get_pusch_port_channel_estimator_notifier();
  }

  // See interface for documentation.
  transform_precoder_metric_notifier& get_pusch_transform_precoder_notifier() override
  {
    return pusch_collector.get_pusch_transform_precoder_notifier();
  }

  // See interface for documentation.
  pdsch_processor_metric_notifier& get_pdsch_processor_notifier() override
  {
    return pdsch_collector.get_pdsch_processor_notifier();
  }

  // See interface for documentation.
  pdsch_dmrs_generator_metric_notifier& get_pdsch_dmrs_generator_notifier() override
  {
    return pdsch_collector.get_pdsch_dmrs_generator();
  }

  // See interface for documentation.
  downlink_processor_metric_notifier& get_downlink_processor_notifier() override
  {
    return dl_processor_collector.get_notifier();
  }

  /// General downlink processor collector.
  upper_phy_downlink_processor_metrics_collector dl_processor_collector;
  /// LDPC metrics collector.
  upper_phy_ldpc_metrics_collector ldpc_collector;
  /// PUSCH processing metrics collector.
  upper_phy_pusch_metrics_collector pusch_collector;
  /// PDSCH processing metrics collector.
  upper_phy_pdsch_metrics_collector pdsch_collector;
  /// UL-SCH demultiplexer metrics aggregator.
  ulsch_demultiplex_metrics_aggregator ulsch_demultiplex_aggregator;
  /// Save the time when the metrics where collected last time.
  std::chrono::high_resolution_clock::time_point last_timestamp = {};
};

} // namespace srsran
