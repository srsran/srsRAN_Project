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

#include "aggregators/channel_modulation_metrics_aggregator.h"
#include "aggregators/channel_precoder_metrics_aggregator.h"
#include "aggregators/crc_calculator_metrics_aggregator.h"
#include "aggregators/pdsch_dmrs_generator_metrics_aggregator.h"
#include "aggregators/pdsch_processor_metrics_aggregator.h"
#include "aggregators/scrambling_metrics_aggregator.h"
#include "srsran/du/du_low/o_du_low_metrics.h"

namespace srsran {
namespace srs_du {

/// PUSCH processing metrics collector.
class du_low_pdsch_metrics_collector
{
public:
  /// Collects the metrics from the PDSCH aggregators and writes them into the given metrics argument.
  void collect_metrics(o_du_low_pdsch_metrics& metrics);

  /// Returns CRC calculator metric aggregator's interface for metrics notification.
  crc_calculator_metric_notifier& get_pdsch_crc_calculator_notifier() { return pdsch_crc_calculator_aggregator; }

  /// Returns scrambling metric aggregator's interface for metrics notification.
  pseudo_random_sequence_generator_metric_notifier& get_pdsch_scrambling_notifier()
  {
    return pdsch_scrambling_aggregator;
  }

  /// Returns PDSCH channel precoder metric aggregator's interface for metrics notification.
  channel_precoder_metric_notifier& get_pdsch_channel_precoder_notifier() { return pdsch_channel_precoder_aggregator; }

  /// Returns PDSCH modulation mapper metric aggregator's interface for metrics notification.
  common_channel_modulation_metric_notifier& get_pdsch_modulation_mapper_notifier()
  {
    return pdsch_modulation_mapper_aggregator;
  }

  /// Returns PDSCH processor metric aggregator's interface for metrics notification.
  pdsch_processor_metric_notifier& get_pdsch_processor_notifier() { return pdsch_processor_aggregator; }

  /// Returns PDSCH DM-RS generator metric aggregator's interface for metrics notification.
  pdsch_dmrs_generator_metric_notifier& get_pdsch_dmrs_generator() { return pdsch_dmrs_generator_aggregator; }

private:
  /// PDSCH CRC calculator metrics aggregator.
  crc_calculator_metrics_aggregator pdsch_crc_calculator_aggregator;
  /// PDSCH scrambling metrics aggregator.
  scrambling_metrics_aggregator pdsch_scrambling_aggregator;
  /// PDSCH channel precoder metrics aggregator.
  channel_precoder_metrics_aggregator pdsch_channel_precoder_aggregator;
  /// PDSCH modulation mapper metrics aggregator.
  channel_modulation_metrics_aggregator pdsch_modulation_mapper_aggregator;
  /// PDSCH processor metric producer.
  pdsch_processor_metrics_aggregator pdsch_processor_aggregator;
  /// PDSCH DM-RS metric producer.
  pdsch_dmrs_generator_metrics_aggregator pdsch_dmrs_generator_aggregator;
};

} // namespace srs_du
} // namespace srsran
