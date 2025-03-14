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

#include "upper_phy_pusch_metrics_collector.h"

using namespace srsran;

void upper_phy_pusch_metrics_collector::collect_metrics(upper_phy_pusch_metrics& metrics)
{
  // Collect metrics from the CRC metrics aggregator.
  auto& crc                 = metrics.crc_metrics;
  crc.crc16_avg_latency_us  = pusch_crc_calculator_aggregator.get_avg_latency_us(crc_generator_poly::CRC16);
  crc.crc16_avg_rate_Mbps   = pusch_crc_calculator_aggregator.get_avg_rate_Mbps(crc_generator_poly::CRC16);
  crc.crc24a_avg_latency_us = pusch_crc_calculator_aggregator.get_avg_latency_us(crc_generator_poly::CRC24A);
  crc.crc24a_avg_rate_Mbps  = pusch_crc_calculator_aggregator.get_avg_rate_Mbps(crc_generator_poly::CRC24A);
  crc.crc24b_avg_latency_us = pusch_crc_calculator_aggregator.get_avg_latency_us(crc_generator_poly::CRC24B);
  crc.crc24b_avg_rate_Mbps  = pusch_crc_calculator_aggregator.get_avg_rate_Mbps(crc_generator_poly::CRC24B);
  crc.cpu_usage_us          = pusch_crc_calculator_aggregator.get_cpu_usage_us();
  pusch_crc_calculator_aggregator.reset();

  // Collect metrics from the scrambling metrics aggregator.
  auto& scrambling              = metrics.scrambling_metrics;
  scrambling.avg_init_time_us   = pusch_scrambling_aggregator.get_average_init_time_us();
  scrambling.advance_rate_Mbps  = pusch_scrambling_aggregator.get_advance_rate_Mbps();
  scrambling.generate_rate_Mbps = pusch_scrambling_aggregator.get_generate_rate_Mbps();
  scrambling.total_proc_time    = pusch_scrambling_aggregator.get_total_time();
  scrambling.cpu_usage_us       = pusch_scrambling_aggregator.get_cpu_usage_us();
  pusch_scrambling_aggregator.reset();

  // Collect metrics from the channel equalizer metrics aggregator.
  auto& eq                       = metrics.ch_equalizer_metrics;
  eq.avg_rate_MREps_one_layer    = pusch_channel_equalizer_aggregator.get_average_rate_MRes(1);
  eq.avg_rate_MREps_two_layers   = pusch_channel_equalizer_aggregator.get_average_rate_MRes(2);
  eq.avg_rate_MREps_three_layers = pusch_channel_equalizer_aggregator.get_average_rate_MRes(3);
  eq.avg_rate_MREps_four_layers  = pusch_channel_equalizer_aggregator.get_average_rate_MRes(4);
  eq.total_proc_time             = pusch_channel_equalizer_aggregator.get_total_time();
  eq.cpu_usage_us                = pusch_channel_equalizer_aggregator.get_cpu_usage_us();
  pusch_channel_equalizer_aggregator.reset();

  // Collect metrics from PDSCH modulation mapper metrics aggregator.
  auto& demod_metrics               = metrics.demod_demapper_metrics;
  demod_metrics.qpsk_avg_rate_Mbps  = pusch_demodulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QPSK);
  demod_metrics.qam16_avg_rate_Mbps = pusch_demodulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM16);
  demod_metrics.qam64_avg_rate_Mbps = pusch_demodulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM64);
  demod_metrics.qam256_avg_rate_Mbps =
      pusch_demodulation_mapper_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM256);
  demod_metrics.cpu_usage_us    = pusch_demodulation_mapper_aggregator.get_cpu_usage_us();
  demod_metrics.total_proc_time = pusch_demodulation_mapper_aggregator.get_total_time();
  pusch_demodulation_mapper_aggregator.reset();

  // Collect metrics from PUSCH EVM calculator metrics aggregator.
  auto& evm_metrics                = metrics.evm_calculator_metrics;
  evm_metrics.qpsk_avg_rate_Mbps   = pusch_evm_calculator_aggregator.get_avg_rate_Mbps(modulation_scheme::QPSK);
  evm_metrics.qam16_avg_rate_Mbps  = pusch_evm_calculator_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM16);
  evm_metrics.qam64_avg_rate_Mbps  = pusch_evm_calculator_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM64);
  evm_metrics.qam256_avg_rate_Mbps = pusch_evm_calculator_aggregator.get_avg_rate_Mbps(modulation_scheme::QAM256);
  evm_metrics.total_proc_time      = pusch_evm_calculator_aggregator.get_total_time();
  evm_metrics.cpu_usage_us         = pusch_evm_calculator_aggregator.get_cpu_usage_us();
  pusch_evm_calculator_aggregator.reset();

  // Collect metrics from PUSCH port channel estimator metrics aggregator.
  metrics.port_ch_estimator_metrics.total_processing_latency = pusch_port_ch_estimator_aggregator.get_total_time();
  metrics.port_ch_estimator_metrics.avg_proc_latency_us      = pusch_port_ch_estimator_aggregator.get_avg_latency_us();
  pusch_port_ch_estimator_aggregator.reset();

  // Collect metrics from PUSCH channel estimator metrics aggregator.
  metrics.ch_estimator_metrics.total_processing_latency = pusch_channel_estimator_aggregator.get_total_time();
  metrics.ch_estimator_metrics.avg_processing_latency   = pusch_channel_estimator_aggregator.get_average_latency();
  metrics.ch_estimator_metrics.min_proc_latency_us      = pusch_channel_estimator_aggregator.get_min_latency_us();
  metrics.ch_estimator_metrics.max_proc_latency_us      = pusch_channel_estimator_aggregator.get_max_latency_us();
  metrics.ch_estimator_metrics.processing_rate_Mbps     = pusch_channel_estimator_aggregator.get_processing_rate();
  metrics.ch_estimator_metrics.cpu_usage_us             = pusch_channel_estimator_aggregator.get_cpu_usage_us();
  pusch_channel_estimator_aggregator.reset();

  // Collect metrics from PUSCH demodulator metrics aggregator.
  metrics.pusch_demod_metrics.total_proc_time = pusch_demodulator_aggregator.get_total_time();
  metrics.pusch_demod_metrics.cpu_usage_us    = pusch_demodulator_aggregator.get_cpu_usage_us();
  pusch_demodulator_aggregator.reset();

  // Collect metrics from PUSCH transform precoder metrics aggregator.
  metrics.xform_precoder_metrics.avg_latency_us     = pusch_transform_precoder_aggregator.get_avg_latency_us();
  metrics.xform_precoder_metrics.avg_rate_MREps     = pusch_transform_precoder_aggregator.get_avg_rate_MREps();
  metrics.xform_precoder_metrics.total_proc_latency = pusch_transform_precoder_aggregator.get_total_time();
  metrics.xform_precoder_metrics.cpu_usage_us       = pusch_transform_precoder_aggregator.get_cpu_usage_us();
  pusch_transform_precoder_aggregator.reset();

  // Collect metrics from PUSCH time alignment estimator metrics aggregator.
  metrics.pusch_ta_est_metrics.avg_latency_us  = pusch_ta_estimator_aggregator.get_avg_latency_us();
  metrics.pusch_ta_est_metrics.avg_rate_MREps  = pusch_ta_estimator_aggregator.get_avg_rate_MREps();
  metrics.pusch_ta_est_metrics.total_proc_time = pusch_ta_estimator_aggregator.get_total_time();
  metrics.pusch_ta_est_metrics.cpu_usage_us    = pusch_ta_estimator_aggregator.get_cpu_usage_us();
  pusch_ta_estimator_aggregator.reset();

  // Collect metrics from UL-SCH demultiplexer metrics aggregator.
  metrics.ulsch_demux_metrics.avg_init_time_us   = ulsch_demux_aggregator.get_average_init_time_us();
  metrics.ulsch_demux_metrics.avg_finish_time_us = ulsch_demux_aggregator.get_average_finish_time_us();
  metrics.ulsch_demux_metrics.avg_rate_Mbps      = ulsch_demux_aggregator.get_average_rate_Mbps();
  metrics.ulsch_demux_metrics.total_proc_time    = ulsch_demux_aggregator.get_total_time();
  metrics.ulsch_demux_metrics.cpu_usage_us       = ulsch_demux_aggregator.get_cpu_usage_us();
  ulsch_demux_aggregator.reset();

  // Collect metrics from PUSCH processor metrics aggregator.
  metrics.pusch_proc_metrics.processing_rate_Mbps = pusch_processor_aggregator.get_processing_rate_Mbps();
  metrics.pusch_proc_metrics.avg_data_latency_us  = pusch_processor_aggregator.get_avg_data_latency_us();
  metrics.pusch_proc_metrics.min_data_latency_us  = pusch_processor_aggregator.get_min_data_latency_us();
  metrics.pusch_proc_metrics.max_data_latency_us  = pusch_processor_aggregator.get_max_data_latency_us();
  metrics.pusch_proc_metrics.avg_uci_latency_us   = pusch_processor_aggregator.get_avg_uci_latency_us();
  metrics.pusch_proc_metrics.min_uci_latency_us   = pusch_processor_aggregator.get_min_uci_latency_us();
  metrics.pusch_proc_metrics.max_uci_latency_us   = pusch_processor_aggregator.get_max_uci_latency_us();
  metrics.pusch_proc_metrics.total_proc_time      = pusch_processor_aggregator.get_total_time();
  metrics.pusch_proc_metrics.total_wait_time      = pusch_processor_aggregator.get_total_wait_time();
  metrics.pusch_proc_metrics.cpu_usage_us         = pusch_processor_aggregator.get_cpu_usage_us();
  metrics.pusch_proc_metrics.sinr_dB              = pusch_processor_aggregator.get_ch_est_sinr();
  metrics.pusch_proc_metrics.evm                  = pusch_processor_aggregator.get_ch_est_evm();
  metrics.pusch_proc_metrics.decoding_bler        = pusch_processor_aggregator.get_decoding_bler();
  pusch_processor_aggregator.reset();
}
