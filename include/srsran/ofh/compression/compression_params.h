/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/support/srsran_assert.h"
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

/// Converts and returns the given compression type into an integer.
constexpr unsigned to_value(compression_type c)
{
  return static_cast<unsigned>(c);
}

/// IQ data compression parameters.
struct ru_compression_params {
  /// IQ data compression used in the packet.
  compression_type type;
  /// Width of I and Q sample in the packet.
  unsigned data_width;
};

inline compression_type to_compression_type(unsigned compr_value)
{
  srsran_assert(compr_value < NOF_COMPRESSION_TYPES_SUPPORTED, "Unsupported compression type={}", compr_value);

  return static_cast<compression_type>(compr_value);
}

inline compression_type to_compression_type(const std::string& compr)
{
  std::string compr_lower = compr;
  std::transform(compr.begin(), compr.end(), compr_lower.begin(), [](char c) { return std::tolower(c); });

  if (compr_lower == "none") {
    return compression_type::none;
  }
  if (compr_lower == "bfp") {
    return compression_type::BFP;
  }
  if (compr_lower == "bfp selective") {
    return compression_type::bfp_selective;
  }
  if (compr_lower == "block scaling") {
    return compression_type::block_scaling;
  }
  if (compr_lower == "mu law") {
    return compression_type::mu_law;
  }
  if (compr_lower == "modulation") {
    return compression_type::modulation;
  }
  if (compr_lower == "modulation selective") {
    return compression_type::mod_selective;
  }

  srsran_assert(0, "Unknown compression method={}", compr);
  return ofh::compression_type::none;
}

inline std::string to_string(ofh::compression_type c)
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
