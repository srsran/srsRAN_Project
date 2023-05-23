/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsran {
namespace ofh {

/// Maximum allowed bit width of compressed IQ data.
static constexpr unsigned MAX_IQ_WIDTH = 16U;

/// Bit width used by quantization of input complex IQ samples.
static constexpr unsigned Q_BIT_WIDTH = MAX_IQ_WIDTH;

/// Number of compression types supported.
static constexpr unsigned NOF_COMPRESSION_TYPES_SUPPORTED = 7U;

/// Compression type used to (de)compress IQ samples.
enum class compression_type {
  /// No compression.
  none,
  /// Block floating point compression (BFP).
  BFP,
  /// Block scaling compression.
  block_scaling,
  /// Mu-law compression.
  mu_law,
  /// Modulation compression.
  modulation,
  /// BFP + selective RE sending.
  bfp_selective,
  /// Modulation compression + selective RE sending.
  mod_selective
};

/// IQ data compression parameters.
struct ru_compression_params {
  /// IQ data compression used in the packet.
  compression_type type;
  /// Width of I and Q sample in the packet.
  unsigned data_width;
};

inline std::string to_string(compression_type c)
{
  switch (c) {
    case compression_type::none:
      return "none";
    case compression_type::BFP:
      return "BFP";
    case compression_type::bfp_selective:
      return "BFP selective";
    case compression_type::block_scaling:
      return "Block scaling";
    case compression_type::mu_law:
      return "Mu law";
    case compression_type::modulation:
      return "Modulation";
    default:
      return "Modulation selective";
  }
}

} // namespace ofh
} // namespace srsran
