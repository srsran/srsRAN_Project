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

#include "srsran/ran/slot_point.h"
#include <chrono>
#include <utility>

namespace srsran {

/// Aggregates different LDPC related metrics.
struct upper_phy_ldpc_metrics {
  /// LDPC decoder metrics.
  struct ldpc_decoder_metrics {
    /// Average codeblock size in bits.
    double avg_cb_size;
    /// Average codeblock latency in microseconds.
    double avg_cb_latency_us;
    /// Minimum codeblock latency in microseconds.
    double min_cb_latency_us;
    /// Maximum codeblock latency in microseconds.
    double max_cb_latency_us;
    /// Average number of iterations per codeblock.
    double avg_nof_iterations;
    /// Processing rate in Mbps.
    double decoding_rate_Mbps;
    /// Total time spent in the decoder.
    std::chrono::nanoseconds total_proc_time;
    /// CPU usage in microseconds of the decoder.
    double cpu_usage_us;
  };

  /// LDPC encoder metrics.
  struct ldpc_encoder_metrics {
    /// Average codeblock size in bits.
    double avg_cb_size;
    /// Average codeblock latency in microseconds.
    double avg_cb_latency_us;
    /// Minimum codeblock latency in microseconds.
    double min_cb_latency_us;
    /// Maximum codeblock latency in microseconds.
    double max_cb_latency_us;
    /// Processing rate in Mbps.
    double encoding_rate_Mbps;
    /// CPU usage in microseconds of the encoder.
    double cpu_usage_us;
  };

  /// LDPC rate matching metrics.
  struct ldpc_rate_match_metrics {
    /// Average codeblock latency in microseconds.
    double avg_cb_latency;
    /// Minimum codeblock latency in microseconds.
    double min_cb_latency_us;
    /// Maximum codeblock latency in microseconds.
    double max_cb_latency_us;
    /// Processing rate in Mbps.
    double processing_rate_Mbps;
    /// CPU usage in microseconds of the rate matcher.
    double cpu_usage_us;
  };

  /// LDPC rate dematching metrics.
  struct ldpc_rate_dematch_metrics {
    /// Average codeblock latency in microseconds.
    double avg_cb_latency;
    /// Minimum codeblock latency in microseconds.
    double min_cb_latency_us;
    /// Maximum codeblock latency in microseconds.
    double max_cb_latency_us;
    /// Processing rate in Mbps.
    double processing_rate_Mbps;
    /// CPU usage in microseconds of the rate dematcher.
    double cpu_usage_us;
  };

  ldpc_decoder_metrics      decoder_metrics;
  ldpc_encoder_metrics      encoder_metrics;
  ldpc_rate_match_metrics   rate_match_metrics;
  ldpc_rate_dematch_metrics rate_dematch_metrics;
};

/// CRC calculation metrics.
struct upper_phy_crc_metrics {
  /// Average CRC16 calculation latency in microseconds.
  double crc16_avg_latency_us;
  /// Average CRC16 processing rate.
  double crc16_avg_rate_Mbps;
  /// Average CRC24A calculation latency in microseconds.
  double crc24a_avg_latency_us;
  /// Average CRC24A processing rate.
  double crc24a_avg_rate_Mbps;
  /// Average CRC24B calculation latency in microseconds.
  double crc24b_avg_latency_us;
  /// Average CRC24B processing rate.
  double crc24b_avg_rate_Mbps;
  /// CPU usage in microseconds of all CRC calculations.
  double cpu_usage_us;
};

/// Pseudo-random generator metrics.
struct upper_phy_scrambling_metrics {
  /// Average initialization time in microseconds.
  double avg_init_time_us;
  /// Average advance rate in Mbps.
  double advance_rate_Mbps;
  /// Average generation rate in Mbps.
  double generate_rate_Mbps;
  /// Total time spent in the scrabler/descrambler.
  std::chrono::nanoseconds total_proc_time;
  /// CPU usage in microseconds of the scrambling process.
  double cpu_usage_us;
};

/// Modulation mapper and EVM calculator metrics.
struct upper_phy_mod_demod_evm_metrics {
  /// Average processing rate in megabits per second
  /// for different modulation schemes.
  double qpsk_avg_rate_Mbps;
  double qam16_avg_rate_Mbps;
  double qam64_avg_rate_Mbps;
  double qam256_avg_rate_Mbps;
  /// Total time spent in the modulation/demodulation mapper.
  std::chrono::nanoseconds total_proc_time;
  /// CPU usage in microseconds of the modulation/demodulation.
  double cpu_usage_us;
};

/// Aggregates different PUSCH processing metrics.
struct upper_phy_pusch_metrics {
  /// PUSCH Channel estimator metrics.
  struct pusch_ch_estimator_metrics {
    /// Average processing latency in microseconds.
    double avg_processing_latency;
    /// Minimum processing latency in microseconds.
    double min_proc_latency_us;
    /// Maximum processing latency in microseconds.
    double max_proc_latency_us;
    /// Average processing rate in Megabits per second.
    double processing_rate_Mbps;
    /// Total time spent in PUSCH channel estimator.
    std::chrono::nanoseconds total_processing_latency;
    /// CPU usage in microseconds.
    double cpu_usage_us;
  };

  /// PUSCH port channel estimator metrics.
  struct pusch_port_ch_estimator_metrics {
    /// Average processing latency in microseconds.
    double avg_proc_latency_us;
    /// Total time spent in port channel estimator.
    std::chrono::nanoseconds total_processing_latency;
  };

  /// PUSCH channel equalizer metrics.
  struct pusch_ch_equalizer_metrics {
    /// Average processing rate in MREps (millions of RE per second) for different number of layers.
    double avg_rate_MREps_one_layer;
    double avg_rate_MREps_two_layers;
    double avg_rate_MREps_three_layers;
    double avg_rate_MREps_four_layers;
    /// Total time spent in the channel equalizer.
    std::chrono::nanoseconds total_proc_time;
    /// CPU usage in microseconds of the channel equalizer.
    double cpu_usage_us;
  };

  /// PUSCH demodulator metrics.
  struct pusch_demodulator_metrics {
    /// PUSCH demodulator total processing time excluding buffer operations.
    std::chrono::nanoseconds total_proc_time;
    /// CPU usage in microseconds.
    double cpu_usage_us;
  };

  /// PUSCH transform precoder metrics.
  struct pusch_xform_precoder_metrics {
    /// Total time spent in PUSCH transform precoder.
    std::chrono::nanoseconds total_proc_latency;
    /// Average processing rate in MREps (millions of resource elements per second).
    double avg_rate_MREps;
    /// Average processing latency in microseconds per symbol.
    double avg_latency_us;
    /// CPU usage in microseconds.
    double cpu_usage_us;
  };

  /// PUSCH Time alignment estimator metrics.
  struct pusch_ta_estimator_metrics {
    /// Average processing latency in microseconds.
    double avg_latency_us;
    /// Average processing rate in MREps (millions of resource elements per second).
    double avg_rate_MREps;
    /// Total processing time.
    std::chrono::nanoseconds total_proc_time;
    /// CPU usage in microseconds.
    double cpu_usage_us;
  };

  /// UL-SCH demultiplexer metrics.
  struct ulsch_demultiplex_metrics {
    /// Average initialization time in us.
    double avg_init_time_us;
    /// Average finalization time in us.
    double avg_finish_time_us;
    /// Average processing rate in Mbps.
    double avg_rate_Mbps;
    /// Total time spent by the UL-SCH demultiplexer.
    std::chrono::nanoseconds total_proc_time;
    /// CPU usage in microseconds.
    double cpu_usage_us;
  };

  /// PUSCH processor metrics.
  struct pusch_processing_metrics {
    /// Average data processing latency in microseconds.
    double avg_data_latency_us;
    /// Minimum data processing latency in microseconds.
    double min_data_latency_us;
    /// Maximum data processing latency in microseconds.
    std::pair<double, slot_point> max_data_latency_us;
    /// Average UCI processing latency in microseconds.
    double avg_uci_latency_us;
    /// Minimum UCI processing latency in microseconds.
    double min_uci_latency_us;
    /// Maximum UCI processing latency in microseconds.
    double max_uci_latency_us;
    /// Average PUSCH processing rate.
    double processing_rate_Mbps;
    /// Total time spent by the PUSCH processor.
    std::chrono::nanoseconds total_proc_time;
    /// The total time spent by the PUSCH processors waiting for the asynchronous decoding to be completed.
    std::chrono::nanoseconds total_wait_time;
    /// CPU usage in microseconds.
    double cpu_usage_us;
    /// SINR value expressed in decibels extracted from channel estimator.
    float sinr_dB;
    /// EVM value extracted from channel estimator.
    float evm;
    /// Decoding BLER.
    double decoding_bler;
  };

  ulsch_demultiplex_metrics       ulsch_demux_metrics;
  upper_phy_crc_metrics           crc_metrics;
  upper_phy_scrambling_metrics    scrambling_metrics;
  pusch_ch_estimator_metrics      ch_estimator_metrics;
  pusch_ch_equalizer_metrics      ch_equalizer_metrics;
  pusch_port_ch_estimator_metrics port_ch_estimator_metrics;
  pusch_demodulator_metrics       pusch_demod_metrics;
  pusch_xform_precoder_metrics    xform_precoder_metrics;
  pusch_ta_estimator_metrics      pusch_ta_est_metrics;
  upper_phy_mod_demod_evm_metrics demod_demapper_metrics;
  upper_phy_mod_demod_evm_metrics evm_calculator_metrics;
  pusch_processing_metrics        pusch_proc_metrics;
};

/// Aggregates different PDSCH processing metrics.
struct upper_phy_pdsch_metrics {
  /// PDSCH DM-RS generator metrics.
  struct pdsch_dmrs_metrics {
    /// Average latency in microseconds.
    double avg_latency_us;
    /// CPU usage in microseconds.
    double cpu_usage_us;
  };

  /// PDSCH Layer mapping and precoding metrics.
  struct pdsch_layermap_precode_metrics {
    /// Channel precoder average processing rate in MREps (Millions of RE per second) for different number of layers.
    double avg_rate_MRes_one_layer;
    double avg_rate_MRes_two_layers;
    double avg_rate_MRes_three_layers;
    double avg_rate_MRes_four_layers;
    /// Totol time in nanoseconds spent in the mapping and precoding block.
    uint64_t total_latency_ns;
    /// CPU usage in microseconds of the layer mapping and precoding.
    double cpu_usage_us;
  };

  /// PDSCH processor metrics.
  struct pdsch_processing_metrics {
    /// Average processing latency in microseconds.
    double avg_latency_us;
    /// Minimum processing latency in microseconds.
    double min_latency_us;
    /// Maximum processing latency in microseconds.
    std::pair<double, slot_point> max_latency_us;
    /// Average return time in microseconds (processing time of a single-thread PDSCH processor).
    double avg_return_time_us;
    /// Processing rate in Mbps.
    double processing_rate_Mbps;
    /// Number of processed TBs.
    uint64_t tbs_processed;
    /// CPU usage in microseconds.
    double cpu_usage_us;
  };

  upper_phy_crc_metrics           crc_metrics;
  upper_phy_scrambling_metrics    scrambling_metrics;
  upper_phy_mod_demod_evm_metrics modulator_metrics;
  pdsch_dmrs_metrics              dmrs_metrics;
  pdsch_layermap_precode_metrics  precoding_metrics;
  pdsch_processing_metrics        pdsch_proc_metrics;
};

/// Aggregates downlink processor metrics.
struct upper_phy_dl_processor_metrics {
  double                        min_latency_us;
  double                        avg_latency_us;
  std::pair<double, slot_point> max_latency_us;
};

/// Upper PHY metrics.
struct upper_phy_metrics {
  unsigned                       sector_id;
  upper_phy_ldpc_metrics         ldpc_metrics;
  upper_phy_pusch_metrics        pusch_metrics;
  upper_phy_pdsch_metrics        pdsch_metrics;
  upper_phy_dl_processor_metrics dl_processor_metrics;
  std::chrono::microseconds      metrics_period;
};

} // namespace srsran
