/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Implements a baseband gateway buffer reader view based on another reader.
class baseband_gateway_buffer_reader_view : public baseband_gateway_buffer_reader
{
public:
  /// \brief Constructs a baseband buffer reader view from a baseband buffer reader for a given offset and a size.
  /// \param[in] buffer_      Buffer that contains the samples.
  /// \param[in] offset_      Offset from the beginning of the buffer.
  /// \param[in] nof_samples_ Number of samples.
  baseband_gateway_buffer_reader_view(const baseband_gateway_buffer_reader& buffer_,
                                      unsigned                              offset_,
                                      unsigned                              nof_samples_) :
    buffer(buffer_), offset(offset_), nof_samples(nof_samples_)
  {
    srsran_assert(buffer.get_nof_samples() >= offset + nof_samples,
                  "The offset (i.e., {}) plus the number of samples (i.e., {}) exceed the buffer size (i.e., {}).",
                  offset,
                  nof_samples,
                  buffer.get_nof_samples());
  }

  // See interface for documentation.
  unsigned get_nof_channels() const override { return buffer.get_nof_channels(); }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return nof_samples; }

  // See interface for documentation.
  span<const cf_t> get_channel_buffer(unsigned i_channel) const override
  {
    return buffer.get_channel_buffer(i_channel).subspan(offset, nof_samples);
  }

private:
  const baseband_gateway_buffer_reader& buffer;
  unsigned                              offset;
  unsigned                              nof_samples;
};

} // namespace srsran
