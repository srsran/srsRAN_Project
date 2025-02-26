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

#include "aggregators/ulsch_demultiplex_metrics_aggregator.h"
#include "du_low_downlink_processor_metrics_collector.h"
#include "du_low_ldpc_metrics_collector.h"
#include "du_low_pdsch_metrics_collector.h"
#include "du_low_pusch_metrics_collector.h"
#include "srsran/du/du_low/o_du_low_metrics_collector.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"

namespace srsran {
namespace srs_du {

/// O-RAN DU low metrics collector implementation.
class o_du_low_metrics_collector_impl : public o_du_low_metrics_collector, public upper_phy_metrics_notifiers
{
public:
  // See interface for documentation.
  void collect_o_du_low_metrics(o_du_low_metrics& metrics) override;

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
  du_low_downlink_processor_metrics_collector dl_processor_collector;
  /// LDPC metrics collector.
  du_low_ldpc_metrics_collector ldpc_collector;
  /// PUSCH processing metrics collector.
  du_low_pusch_metrics_collector pusch_collector;
  /// PDSCH processing metrics collector.
  du_low_pdsch_metrics_collector pdsch_collector;
  /// UL-SCH demultiplexer metrics aggregator.
  ulsch_demultiplex_metrics_aggregator ulsch_demultiplex_aggregator;
  /// Save the time when the metrics where collected last time.
  std::chrono::high_resolution_clock::time_point last_timestamp = {};
};

} // namespace srs_du
} // namespace srsran
