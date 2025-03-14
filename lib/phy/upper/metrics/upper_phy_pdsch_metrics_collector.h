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

#include "aggregators/channel_modulation_metrics_aggregator.h"
#include "aggregators/channel_precoder_metrics_aggregator.h"
#include "aggregators/crc_calculator_metrics_aggregator.h"
#include "aggregators/pdsch_dmrs_generator_metrics_aggregator.h"
#include "aggregators/pdsch_processor_metrics_aggregator.h"
#include "aggregators/scrambling_metrics_aggregator.h"
#include "srsran/phy/upper/upper_phy_metrics.h"

namespace srsran {

/// PDSCH processing metrics collector.
class upper_phy_pdsch_metrics_collector
{
public:
  /// Collects the metrics from the PDSCH aggregators and writes them into the given metrics argument.
  void collect_metrics(upper_phy_pdsch_metrics& metrics);

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
  /// PDSCH processor metric aggregator.
  pdsch_processor_metrics_aggregator pdsch_processor_aggregator;
  /// PDSCH DM-RS metric aggregator.
  pdsch_dmrs_generator_metrics_aggregator pdsch_dmrs_generator_aggregator;
};

} // namespace srsran
