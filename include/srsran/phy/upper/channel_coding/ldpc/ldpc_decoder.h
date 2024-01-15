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

/// \file
/// \brief LDPC decoder interface.

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

namespace srsran {

/// LDPC decoder interface.
class ldpc_decoder
{
public:
  /// Default destructor.
  virtual ~ldpc_decoder() = default;

  /// Decoder configuration.
  struct configuration {
    /// LDPC decoding algorithm configuration parameters.
    struct algorithm_details {
      /// Maximum number of iterations.
      unsigned max_iterations = 6;
      /// Scaling factor of the normalized min-sum decoding algorithm.
      float scaling_factor = 0.8;
    };

    /// Codeblock configuration.
    codeblock_metadata block_conf;
    /// LDPC decoding algorithm configuration.
    algorithm_details algorithm_conf;
  };

  /// \brief Decodes a codeblock.
  ///
  /// By passing a CRC calculator, the CRC is verified after each iteration allowing, when successful, an early stop of
  /// the decoding process.
  ///
  /// \param[out] output  Reconstructed message of information bits.
  /// \param[in]  input   Log-likelihood ratios of the codeblock to be decoded.
  /// \param[in]  crc     Pointer to a CRC calculator for early stopping. Set to \c nullptr for disabling early
  ///                     stopping.
  /// \param[in]  cfg     Decoder configuration.
  /// \return If the decoding is successful, returns the number of LDPC iterations needed by the decoder. Otherwise, no
  ///         value is returned.
  /// \note A codeblock of all zero-valued log-likelihood ratios will automatically will return an empty value (i.e.,
  /// failed CRC) and set all the output bits to one.
  virtual optional<unsigned>
  decode(bit_buffer& output, span<const log_likelihood_ratio> input, crc_calculator* crc, const configuration& cfg) = 0;
};

} // namespace srsran
