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

#include "upper_phy_pdsch_metrics_collector.h"

using namespace srsran;

void upper_phy_pdsch_metrics_collector::collect_metrics(upper_phy_pdsch_metrics& metrics)
{
  // Collect metrics from the CRC metrics aggregator.
  metrics.crc_metrics.crc16_avg_latency_us =
      pdsch_crc_calculator_aggregator.get_avg_latency_us(crc_generator_poly::CRC16);
  metrics.crc_metrics.crc16_avg_rate_Mbps =
      pdsch_crc_calculator_aggregator.get_avg_rate_Mbps(crc_generator_poly::CRC16);
  metrics.crc_metrics.crc24a_avg_latency_us =
      pdsch_crc_calculator_aggregator.get_avg_latency_us(crc_generator_poly::CRC24A);
  metrics.crc_metrics.crc24a_avg_rate_Mbps =
      pdsch_crc_calculator_aggregator.get_avg_rate_Mbps(crc_generator_poly::CRC24A);
  metrics.crc_metrics.crc24b_avg_latency_us =
      pdsch_crc_calculator_aggregator.get_avg_latency_us(crc_generator_poly::CRC24B);
  metrics.crc_metrics.crc24b_avg_rate_Mbps =
      pdsch_crc_calculator_aggregator.get_avg_rate_Mbps(crc_generator_poly::CRC24B);
  metrics.crc_metrics.cpu_usage_us = pdsch_crc_calculator_aggregator.get_cpu_usage_us();
  pdsch_crc_calculator_aggregator.reset();

  // Collect metrics from the scrambling metrics aggregator.
  metrics.scrambling_metrics.avg_init_time_us   = pdsch_scrambling_aggregator.get_average_init_time_us();
  metrics.scrambling_metrics.advance_rate_Mbps  = pdsch_scrambling_aggregator.get_advance_rate_Mbps();
  metrics.scrambling_metrics.generate_rate_Mbps = pdsch_scrambling_aggregator.get_generate_rate_Mbps();
  metrics.scrambling_metrics.cpu_usage_us       = pdsch_scrambling_aggregator.get_cpu_usage_us();
  pdsch_scrambling_aggregator.reset();

  // Collect metrics from the layer mapper and precoder metrics aggregator.
  metrics.precoding_metrics.avg_rate_MRes_one_layer    = pdsch_channel_precoder_aggregator.get_average_rate_MRes(1);
  metrics.precoding_metrics.avg_rate_MRes_two_layers   = pdsch_channel_precoder_aggregator.get_average_rate_MRes(2);
  metrics.precoding_metrics.avg_rate_MRes_three_layers = pdsch_channel_precoder_aggregator.get_average_rate_MRes(3);
  metrics.precoding_metrics.avg_rate_MRes_four_layers  = pdsch_channel_precoder_aggregator.get_average_rate_MRes(4);
  metrics.precoding_metrics.total_latency_ns           = pdsch_channel_precoder_aggregator.get_total_time_ns();
  metrics.precoding_metrics.cpu_usage_us               = pdsch_channel_precoder_aggregator.get_cpu_usage_us();
  pdsch_channel_precoder_aggregator.reset();

  // Collect metrics from PDSCH modulation mapper metrics aggregator.
  metrics.modulator_metrics.qpsk_avg_rate_Mbps =
      pdsch_modulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QPSK);
  metrics.modulator_metrics.qam16_avg_rate_Mbps =
      pdsch_modulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM16);
  metrics.modulator_metrics.qam64_avg_rate_Mbps =
      pdsch_modulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM64);
  metrics.modulator_metrics.qam256_avg_rate_Mbps =
      pdsch_modulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM256);
  metrics.modulator_metrics.cpu_usage_us = pdsch_modulation_mapper_aggregator.get_cpu_usage_us();
  pdsch_modulation_mapper_aggregator.reset();

  // Collect metrics from PDSCH DM-RS metric aggregator.
  metrics.dmrs_metrics.avg_latency_us = pdsch_dmrs_generator_aggregator.get_avg_latency_us();
  metrics.dmrs_metrics.cpu_usage_us   = pdsch_dmrs_generator_aggregator.get_cpu_usage_us();
  pdsch_dmrs_generator_aggregator.reset();

  // Collect metrics from PDSCH processor metric aggregator.
  metrics.pdsch_proc_metrics.avg_latency_us       = pdsch_processor_aggregator.get_avg_latency_us();
  metrics.pdsch_proc_metrics.min_latency_us       = pdsch_processor_aggregator.get_min_latency_us();
  metrics.pdsch_proc_metrics.max_latency_us       = pdsch_processor_aggregator.get_max_latency_us();
  metrics.pdsch_proc_metrics.avg_return_time_us   = pdsch_processor_aggregator.get_avg_return_time_us();
  metrics.pdsch_proc_metrics.processing_rate_Mbps = pdsch_processor_aggregator.get_process_rate_Mbps();
  metrics.pdsch_proc_metrics.tbs_processed        = pdsch_processor_aggregator.get_nof_processed_tbs();
  metrics.pdsch_proc_metrics.cpu_usage_us         = pdsch_processor_aggregator.get_cpu_usage_us();
  pdsch_processor_aggregator.reset();
}
