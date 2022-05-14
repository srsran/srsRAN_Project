/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC decoder interface.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_DECODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_DECODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"

namespace srsgnb {

/// LDPC decoder interface.
class ldpc_decoder
{
public:
  /// Default destructor.
  virtual ~ldpc_decoder() = default;

  /// Decoder configuration.
  struct configuration {
    struct algorithm_details {
      /// Maximum number of iterations.
      unsigned max_iterations = 6;
      /// Scaling factor of the normalized min-sum decoding algorithm.
      float scaling_factor = 0.8;
    };

    codeblock_metadata block_conf     = {};
    algorithm_details  algorithm_conf = {};
  };

  /// \brief Decodes a codeblock.
  ///
  /// \param[out] output  Reconstructed message of information bits.
  /// \param[in]  input   Log-likelihood ratios of the codeblock to be decoded.
  /// \param[in]  cfg     Decoder configuration.
  virtual unsigned decode(span<uint8_t> output, span<const int8_t> input, const configuration& cfg) = 0;

  /// \brief Decodes a codeblock.
  ///
  /// \param[out] output  Reconstructed message of information bits.
  /// \param[in]  input   Log-likelihood ratios of the codeblock to be decoded.
  /// \param[in]  crc     Pointer to a CRC calculator for early stopping.
  /// \param[in]  cfg     Decoder configuration.
  virtual unsigned
  decode(span<uint8_t> output, span<const int8_t> input, crc_calculator* crc, const configuration& cfg) = 0;
};

std::unique_ptr<ldpc_decoder> create_ldpc_decoder(const std::string& dec_type);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_DECODER_H
