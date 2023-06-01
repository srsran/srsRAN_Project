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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/adt/tensor.h"
#include "srsran/gateways/baseband/baseband_gateway_base.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/srsran_assert.h"
#include <vector>

namespace srsran {

namespace detail {

enum class baseband_gateway_buffer_dims : unsigned { sample = 0, channel, all };

/// Implements a baseband gateway buffer reader based on a tensor.
class baseband_gateway_buffer_reader_tensor : public baseband_gateway_buffer_reader
{
public:
  using storage_type = tensor<static_cast<unsigned>(detail::baseband_gateway_buffer_dims::all),
                              cf_t,
                              detail::baseband_gateway_buffer_dims>;

  /// Creates a gateway buffer reader based on a tensor storage type.
  baseband_gateway_buffer_reader_tensor(const storage_type& data_) : data(data_) {}

  // See interface for documentation.
  unsigned get_nof_channels() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::channel); }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::sample); }

  // See interface for documentation.
  span<const cf_t> get_channel_buffer(unsigned i_channel) const override { return data.get_view({i_channel}); }

private:
  const storage_type& data;
};

/// Implements a baseband gateway buffer writer based on a tensor.
class baseband_gateway_buffer_writer_tensor : public baseband_gateway_buffer_writer
{
public:
  using storage_type = tensor<static_cast<unsigned>(detail::baseband_gateway_buffer_dims::all),
                              cf_t,
                              detail::baseband_gateway_buffer_dims>;

  /// Creates a gateway buffer writer based on a tensor storage type.
  baseband_gateway_buffer_writer_tensor(storage_type& data_) : data(data_) {}

  // See interface for documentation.
  unsigned get_nof_channels() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::channel); }

  // See interface for documentation.
  unsigned get_nof_samples() const override { return data.get_dimension_size(baseband_gateway_buffer_dims::sample); }

  // See interface for documentation.
  span<cf_t> get_channel_buffer(unsigned i_channel) override { return data.get_view({i_channel}); }

private:
  storage_type& data;
};

} // namespace detail

/// \brief Describes a baseband buffer implementation that comprises a fix number of channels that can be dynamically
/// resized.
///
/// It contains a fix get_nof_channels() number of channels that contain the same number of get_nof_samples() samples.
/// The number of samples can be changed in runtime without re-allocating memory using resize(). The number samples
/// shall never exceed the maximum number of samples indicated in the constructor.
class baseband_gateway_buffer_dynamic
{
public:
  /// Gets the number of channels.
  unsigned get_nof_channels() const { return data.get_dimension_size(detail::baseband_gateway_buffer_dims::channel); }

  /// Gets the current number of samples.
  unsigned get_nof_samples() const { return data.get_dimension_size(detail::baseband_gateway_buffer_dims::sample); }

  /// Gets the reader interface.
  const baseband_gateway_buffer_reader& get_reader() const { return reader; }

  /// Gets the writer interface.
  baseband_gateway_buffer_writer& get_writer() { return writer; }

  /// Gets a data view of a channel.
  span<cf_t> operator[](unsigned i_channel) { return writer.get_channel_buffer(i_channel); }

  /// \brief Resize buffer.
  /// \param[in] new_nof_samples Indicates the new number of samples per channel.
  /// \note The new number of samples must be greater than 0 and must not exceed the maximum number of samples.
  void resize(unsigned new_nof_samples)
  {
    report_fatal_error_if_not(!data.empty(), "Data is empty. Was the buffer moved?");
    data.resize({new_nof_samples, get_nof_channels()});
  }

  /// \brief Default constructor.
  /// \param[in] nof_channels_ Indicates the number of channels to create.
  /// \param[in] max_nof_samples_  Indicates the maximum number of samples.
  baseband_gateway_buffer_dynamic(unsigned nof_channels, unsigned max_nof_samples) :
    data({max_nof_samples, nof_channels}), reader(data), writer(data)
  {
  }

  /// Move constructor.
  baseband_gateway_buffer_dynamic(baseband_gateway_buffer_dynamic&& other) :
    data(std::move(other.data)), reader(data), writer(data)
  {
  }

private:
  dynamic_tensor<static_cast<unsigned>(detail::baseband_gateway_buffer_dims::all),
                 cf_t,
                 detail::baseband_gateway_buffer_dims>
                                                data;
  detail::baseband_gateway_buffer_reader_tensor reader;
  detail::baseband_gateway_buffer_writer_tensor writer;
};

} // namespace srsran
