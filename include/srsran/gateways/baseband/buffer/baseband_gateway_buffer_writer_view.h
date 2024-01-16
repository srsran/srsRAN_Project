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
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Implements a baseband gateway buffer writer view based on another writer.
class baseband_gateway_buffer_writer_view : public baseband_gateway_buffer_writer
{
public:
  /// \brief Constructs a baseband buffer writer view from a baseband buffer writer for a given offset and a size.
  /// \param[in] buffer_      Buffer that contains the samples.
  /// \param[in] offset_      Offset from the beginning of the buffer.
  /// \param[in] nof_samples_ Number of samples.
  baseband_gateway_buffer_writer_view(baseband_gateway_buffer_writer& buffer_,
                                      unsigned                        offset_,
                                      unsigned                        nof_samples_) :
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
  span<cf_t> get_channel_buffer(unsigned i_channel) override
  {
    return buffer.get_channel_buffer(i_channel).subspan(offset, nof_samples);
  }

private:
  baseband_gateway_buffer_writer& buffer;
  unsigned                        offset;
  unsigned                        nof_samples;
};

} // namespace srsran