/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC encoder interface.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

class ldpc_encoder_buffer;

/// LDPC encoder interface.
class ldpc_encoder
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder() = default;

  /// \brief Encodes a message.
  ///
  /// \param[in]  input   Message: original information bits, with the filler bits (if any) set to zero.
  /// \param[in]  cfg     Encoder configuration for the current codeblock.
  /// \return A reference to the LDPC encoder buffer.
  /// \note The length of the output codeblock is deduced from the size of parameter \c output.
  virtual const ldpc_encoder_buffer& encode(const bit_buffer&                             input,
                                            const codeblock_metadata::tb_common_metadata& cfg) = 0;
};

} // namespace srsran
