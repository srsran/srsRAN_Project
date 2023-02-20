/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC encoder interface.

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"

namespace srsgnb {

/// LDPC encoder interface.
class ldpc_encoder
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder() = default;

  /// \brief Encodes a message.
  ///
  /// \param[out] output  Resulting codeblock.
  /// \param[in]  input   Message: original information bits (can contain filler_bit).
  /// \param[in]  cfg     Encoder configuration for the current codeblock.
  /// \note The length of the output codeblock is deduced from the size of parameter \c output.
  virtual void
  encode(span<uint8_t> output, span<const uint8_t> input, const codeblock_metadata::tb_common_metadata& cfg) = 0;
};

} // namespace srsgnb
