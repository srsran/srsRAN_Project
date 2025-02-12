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

namespace srsran {
namespace srs_du {

/// Aggregates different LDPC related metrics.
struct o_du_low_ldpc_metrics {
  /// LDPC decoder metrics.
  struct ldpc_decoder_metrics {
    /// Average codeblock size in bits.
    double avg_cb_size;
    /// Average codeblock latency in microseconds.
    double avg_cb_latency_us;
    /// Average number of iterations per codeblock.
    double avg_nof_iterations;
    /// Processing rate in Mbps.
    double decoding_rate_Mbps;
    /// CPU usage in percentage of the decoder.
    double cpu_usage;
  };

  /// LDPC encoder metrics.
  struct ldpc_encoder_metrics {
    /// Average codeblock size in bits.
    double avg_cb_size;
    /// Average codeblock latency in microseconds.
    double avg_cb_latency;
    /// Processing rate in Mbps.
    double encoding_rate_Mbps;
  };

  /// LDPC rate matching metrics.
  struct ldpc_rate_match_metrics {
    /// Average codeblock latency in microseconds.
    double avg_cb_latency;
    /// Processing rate in Mbps.
    double processing_rate_Mbps;
  };

  /// LDPC rate dematching metrics.
  struct ldpc_rate_dematch_metrics {
    /// Average codeblock latency in microseconds.
    double avg_cb_latency;
    /// Processing rate in Mbps.
    double processing_rate_Mbps;
  };

  ldpc_decoder_metrics      decoder_metrics;
  ldpc_encoder_metrics      encoder_metrics;
  ldpc_rate_match_metrics   rate_match_metrics;
  ldpc_rate_dematch_metrics rate_dematch_metrics;
};

/// CRC calculation metrics.
struct o_du_low_crc_metrics {
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
};

/// Pseudo-random generator metrics.
struct o_du_low_scrambling_metrics {
  /// Average initialization time in microseconds.
  double scrambling_avg_init_time_us;
  /// Average advance rate in Mbps.
  double scrambling_advance_rate_Mbps;
  /// Average generation rate in Mbps.
  double scrambling_generate_rate_Mbps;
};

/// Modulator, demodulator and EVM calculator metrics.
struct o_du_low_mod_demod_evm_metrics {
  /// Average processing rate in megabits per second
  /// for different modulation schemes.
  double qpsk_avg_rate_Mbps;
  double qam16_avg_rate_Mbps;
  double qam64_avg_rate_Mbps;
  double qam256_avg_rate_Mbps;
};

/// Aggregates different PUSCH processing metrics.
struct o_du_low_pusch_metrics {
  /// PUSCH Channel estimator metrics.
  struct pusch_ch_estimator_metrics {
    double avg_processing_latency;
    double processing_rate_Mbps;
  };

  /// PUSCH channel equalizer metrics.
  struct pusch_ch_equalizer_metrics {
    /// Average processing rate in MREps (millions of RE per second) for different number of layers.
    double avg_rate_MREps_one_layer;
    double avg_rate_MREps_two_layers;
    double avg_rate_MREps_three_layers;
    double avg_rate_MREps_four_layers;
  };

  /// PUSCH transform precoder metrics.
  struct pusch_xform_precoder_metrics {
    /// Average processing rate in MREps (millions of resource elements per second).
    double avg_rate_MREps;
    /// Average processing latency in microseconds per symbol.
    double avg_latency_us;
  };

  /// PUSCH processor metrics.
  struct pusch_processing_metrics {
    /// Average codeblock latency in microseconds.
    double avg_data_latency;
    /// Average UCI processing latency in microseconds.
    double avg_uci_latency_us;
    /// Average PUSCH processing rate.
    double processing_rate_Mbps;
    /// Percent of total PUSCH processing time used by the LDPC decoder.
    double ldpc_decoding_percent;
    /// Percent of total PUSCH processing time used by the channel estimation.
    double channel_estimator_percent;
    /// Percent of total PUSCH processing time used by the channel equalizer.
    double channel_equalizer_percent;
    /// Percent of total PUSCH processing time used by the scrambling.
    double scrambling_percent;
  };

  o_du_low_crc_metrics           crc_metrics;
  o_du_low_scrambling_metrics    scrambling_metrics;
  pusch_ch_estimator_metrics     ch_estimator_metrics;
  pusch_ch_equalizer_metrics     ch_equalizer_metrics;
  pusch_xform_precoder_metrics   xform_precoder_metrics;
  o_du_low_mod_demod_evm_metrics demodulator_metrics;
  o_du_low_mod_demod_evm_metrics evm_calculator_metrics;
  pusch_processing_metrics       pusch_proc_metrics;
};

/// Aggregates different PDSCH processing metrics.
struct o_du_low_pdsch_metrics {
  /// PDSCH DM-RS generator metrics.
  struct pdsch_dmrs_metrics {
    /// Average latency in microseconds.
    double avg_latency_us;
  };

  /// PDSCH processor metrics.
  struct pdsch_processing_metrics {
    /// Average processing latency in microseconds.
    double avg_latency_us;
    /// Average return time in microseconds (processing time of a single-thread PDSCH processor).
    double avg_return_time_us;
    /// Processing rate in Mbps.
    double processing_rate_Mbps;
  };

  o_du_low_crc_metrics           crc_metrics;
  o_du_low_scrambling_metrics    scrambling_metrics;
  o_du_low_mod_demod_evm_metrics modulator_metrics;
  pdsch_dmrs_metrics             dmrs_metrics;
  pdsch_processing_metrics       pdsch_proc_metrics;
};

/// O-RAN DU low metrics.
struct o_du_low_metrics {
  o_du_low_ldpc_metrics  ldpc_metrics;
  o_du_low_pusch_metrics pusch_metrics;
  o_du_low_pdsch_metrics pdsch_metrics;
};

} // namespace srs_du
} // namespace srsran
