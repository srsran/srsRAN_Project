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
/// \brief Polar decoding.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_DECODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_DECODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

/// Polar decoder interface.
class polar_decoder
{
public:
  /// Default destructor.
  virtual ~polar_decoder() = default;

  /// \brief Decodes the input (LLR) codeword with the specified polar decoder.
  /// \param[out] data_decoded The decoder output vector.
  /// \param[in]  input_llr    The decoder LLR input vector.
  /// \param[in]  code         Polar code descriptor.
  virtual void
  decode(span<uint8_t> data_decoded, span<const log_likelihood_ratio> input_llr, const polar_code& code) = 0;
};

/// \brief Polar decoder initialization.
///
/// Initializes all the polar decoder variables according to the Simplified Successive
/// Cancellation (SSC) decoder algorithm and the maximum given code size.
/// \param[in] enc           A polar encoder.
/// \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the decoder input/output vector.
/// \return A unique pointer to the initialized SSC polar decoder.
std::unique_ptr<polar_decoder> create_polar_decoder_ssc(std::unique_ptr<polar_encoder> enc, unsigned code_size_log);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_DECODER_H
