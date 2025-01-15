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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Read-only baseband buffer interface.
///
/// It comprises get_nof_channels() channels of get_nof_samples() samples each.
class baseband_gateway_buffer_reader
{
public:
  virtual ~baseband_gateway_buffer_reader() = default;

  /// Gets the number of channels.
  virtual unsigned get_nof_channels() const = 0;

  /// Gets the number of samples.
  virtual unsigned get_nof_samples() const = 0;

  /// \brief Gets a channel buffer.
  /// \param[in] channel_idx Indicates the channel index.
  /// \return A view to a baseband buffer.
  /// \note An invalid channel index causes undefined behaviour.
  virtual span<const cf_t> get_channel_buffer(unsigned channel_idx) const = 0;

  /// See get_channel_buffer() documentation.
  span<const cf_t> operator[](unsigned channel_index) const { return get_channel_buffer(channel_index); }
};

} // namespace srsran
