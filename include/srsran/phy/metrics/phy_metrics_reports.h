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

} // namespace srsran
