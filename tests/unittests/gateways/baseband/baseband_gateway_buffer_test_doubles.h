/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// \brief Read-only baseband buffer.
///
/// Records all the baseband samples that pass through the baseband gateway spy classes.
class baseband_gateway_buffer_read_only : public baseband_gateway_buffer_reader
{
private:
  unsigned          nof_channels;
  unsigned          nof_samples;
  std::vector<cf_t> data;

public:
  // See interface for documentation.
  unsigned get_nof_channels() const override { return nof_channels; }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return nof_samples; }

  // See interface for documentation.
  span<const cf_t> get_channel_buffer(unsigned channel_idx) const override
  {
    report_fatal_error_if_not(!data.empty(), "Data is empty. Was the buffer moved?");
    report_fatal_error_if_not(
        channel_idx < nof_channels, "Channel index ({}) is out-of-range ({}).", channel_idx, nof_channels);
    unsigned offset = nof_samples * channel_idx;
    return span<const cf_t>(data).subspan(offset, nof_samples);
  }

  /// Creates a default empty buffer.
  baseband_gateway_buffer_read_only() : nof_channels(0), nof_samples(0)
  {
    // Do nothing.
  }

  /// Move constructor.
  baseband_gateway_buffer_read_only(baseband_gateway_buffer_read_only&& other) noexcept :
    nof_channels(other.nof_channels), nof_samples(other.nof_samples), data(std::move(other.data))
  {
    // Reset the other number of channels and samples to 0 to be consistent with other.data.size().
    other.nof_channels = 0;
    other.nof_samples  = 0;
  }

  /// Copy constructor.
  baseband_gateway_buffer_read_only(const baseband_gateway_buffer_read_only& other) noexcept :
    nof_channels(other.nof_channels), nof_samples(other.nof_samples), data(std::move(other.data))
  {
    // Do nothing.
  }

  /// Constructor from a reader interface.
  baseband_gateway_buffer_read_only(const baseband_gateway_buffer_reader& other) noexcept :
    nof_channels(other.get_nof_channels()), nof_samples(other.get_nof_samples()), data(nof_channels * nof_samples)
  {
    for (unsigned channel = 0; channel != nof_channels; ++channel) {
      span<cf_t> buffer = span<cf_t>(data).subspan(nof_samples * channel, nof_samples);
      srsvec::copy(buffer, other[channel]);
    }
  }

  /// Constructor from a writer interface.
  baseband_gateway_buffer_read_only(baseband_gateway_buffer_writer& other) noexcept :
    nof_channels(other.get_nof_channels()), nof_samples(other.get_nof_samples()), data(nof_channels * nof_samples)
  {
    for (unsigned channel = 0; channel != nof_channels; ++channel) {
      span<cf_t> buffer = span<cf_t>(data).subspan(nof_samples * channel, nof_samples);
      srsvec::copy(buffer, other[channel]);
    }
  }

  /// Overloads assign operator.
  baseband_gateway_buffer_read_only operator=(const baseband_gateway_buffer_read_only& other)
  {
    nof_channels = other.nof_channels;
    nof_samples  = other.nof_samples;
    data         = other.data;
    return *this;
  }
};

} // namespace srsran