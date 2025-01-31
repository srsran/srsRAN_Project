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

#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/ran/sch/modulation_scheme.h"
#include "srsran/support/units.h"
#include <chrono>
#include <optional>

namespace srsran {

/// Collects CRC calculator metrics.
struct crc_calculator_metrics {
  /// CRC polynomial.
  crc_generator_poly poly;
  /// Input size.
  units::bits nof_bits;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects LDPC encoder metrics.
struct ldpc_encoder_metrics {
  /// Codeblock size.
  units::bits cb_sz;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects LDPC rate matcher metrics.
struct ldpc_rate_matcher_metrics {
  /// Codeblock size.
  units::bits output_size;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects LDPC rate dematcher metrics.
struct ldpc_rate_dematcher_metrics {
  /// Number of input bits.
  units::bits input_size;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects LDPC decoder metrics.
struct ldpc_decoder_metrics {
  /// Codeblock size.
  units::bits cb_sz;
  /// Number of iterations.
  unsigned nof_iterations;
  /// Set to true if the CRC matches.
  bool crc_ok;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects PUSCH channel estimator metrics.
struct pusch_channel_estimator_metrics {
  /// PUSCH transmission bandwidth.
  unsigned nof_prb;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects PUSCH processor metrics.
struct pusch_processor_metrics {
  /// Codeblock size.
  units::bytes tbs;
  /// Set to true if the CRC matches.
  bool crc_ok;
  /// Elapsed time between the start of the processing and the return.
  std::chrono::nanoseconds elapsed_return;
  /// Elapsed time between the start of the processing and the completion of the processing.
  std::chrono::nanoseconds elapsed_data;
  /// Optional elapsed time between the start of the processing and the UCI notification. Set to default value if UCI
  /// is not present.
  std::optional<std::chrono::nanoseconds> elapsed_uci;
};

/// Collects scrambling metrics for PDSCH and PUSCH.
struct pseudo_random_sequence_generator_metrics {
  /// Pseudo random-sequence generator possible methods.
  enum class methods : uint8_t {
    init = 0,
    advance,
    apply_xor_soft_bit,
    apply_xor_unpacked,
    apply_xor_packed,
    generate_bit_packed,
    generate_float
  };
  /// Method called.
  methods method;
  /// Number of processed soft bits.
  std::size_t nof_bits;
  /// Elapsed time during processing.
  std::chrono::nanoseconds elapsed;
};

/// Collects channel precoder metrics.
struct channel_precoder_metrics {
  /// Channel precoder possible methods.
  enum class methods : uint8_t { apply_precoding = 0, apply_layer_map_and_precoding };
  /// Channel precoder method.
  methods method;
  /// Number of resource elements.
  unsigned nof_re;
  /// Number of layers.
  unsigned nof_layers;
  /// Number of transmit ports.
  unsigned nof_ports;
  /// Elapsed time during processing.
  std::chrono::nanoseconds elapsed;
};

/// Collects channel equalizer metrics.
struct channel_equalizer_metrics {
  /// Number of resource elements.
  unsigned nof_re;
  /// Number of layers.
  unsigned nof_layers;
  /// Number of transmit ports.
  unsigned nof_ports;
  /// Elapsed time during processing.
  std::chrono::nanoseconds elapsed;
};

/// Collects modulation metrics.
struct channel_modulation_metrics {
  /// Modulation.
  modulation_scheme modulation;
  /// Number of complex symbols to modulate.
  unsigned nof_symbols;
  /// Elapsed time during processing.
  std::chrono::nanoseconds elapsed;
};

/// Modulation mapper metrics, use common channel modulation metrics.
using modulation_mapper_metrics = channel_modulation_metrics;

/// Demodulation mapper metrics, use common channel modulation metrics.
using demodulation_mapper_metrics = channel_modulation_metrics;

/// EVM calculator metrics, use common channel modulation metrics.
using evm_calculator_metrics = channel_modulation_metrics;

/// Collects UL-SCH demultiplexer metrics.
struct ulsch_demultiplex_metrics {
  /// Elapsed time during the new transmission initialization.
  std::chrono::nanoseconds elapsed_init;
  /// Accumulated elapsed time during the new block notifications.
  std::chrono::nanoseconds elapsed_on_new_block;
  /// Elapsed time during upon the notification of the end of the codeword.
  std::chrono::nanoseconds elapsed_on_end_codeword;
  /// Total number of bits that went through the demultiplexer.
  unsigned sum_nof_bits;
};

/// Collects PUSCH demodulator metrics.
struct pusch_demodulator_metrics {
  /// Total PUSCH demodulator elapsed time.
  std::chrono::nanoseconds elapsed;
  /// Elapsed time calling codeword buffer methods.
  std::chrono::nanoseconds elapsed_buffer;
};

/// Collects time alignment estimator metrics.
struct time_alignment_estimator_metrics {
  /// Total elapsed time.
  std::chrono::nanoseconds elapsed;
  /// Bandwidth in RE.
  unsigned nof_re;
};

/// Collects port channel estimator metrics.
struct port_channel_estimator_metrics {
  /// Total elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects transform precoder metrics.
struct transform_precoder_metrics {
  /// Total elapsed time.
  std::chrono::nanoseconds elapsed;
  /// Number of subcarriers.
  unsigned nof_re;
};

} // namespace srsran
