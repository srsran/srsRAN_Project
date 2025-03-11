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

#include "srsran/adt/span.h"
#include <stdint.h>

namespace srsran {

/// \brief LDPC encoder output buffer interface.
///
/// This interface allows the LDPC encoder to provide the encoded bits to the rate matcher without using an intermediate
/// buffer.
class ldpc_encoder_buffer
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder_buffer() = default;

  /// Gets the encoded codeblock length.
  virtual unsigned get_codeblock_length() const = 0;

  /// \brief Reads the encoded codeblock data starting at bit index \c offset.
  ///
  /// The data size plus the offset shall not exceed the encoded codeblock length.
  ///
  /// \param[out] data   Read destination.
  /// \param[in]  offset Initial read position.
  virtual void write_codeblock(span<uint8_t> data, unsigned offset) const = 0;
};

} // namespace srsran
