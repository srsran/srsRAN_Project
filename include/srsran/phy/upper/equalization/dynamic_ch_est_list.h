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

#include "srsran/adt/tensor.h"
#include "srsran/phy/upper/equalization/channel_equalizer.h"

namespace srsran {

/// Channel estimates list with based in a tensor storage.
class dynamic_ch_est_list : public channel_equalizer::ch_est_list
{
public:
  dynamic_ch_est_list() = default;

  dynamic_ch_est_list(unsigned nof_re, unsigned nof_rx_ports, unsigned nof_layers) :
    data({nof_re, nof_rx_ports, nof_layers})
  {
  }

  /// Gets a read-write channel given a receive port and layer indices.
  span<cbf16_t> get_channel(unsigned i_rx_port, unsigned i_layer) { return data.get_view({i_rx_port, i_layer}); }

  // See interface for documentation.
  span<const cbf16_t> get_channel(unsigned i_rx_port, unsigned i_layer) const override
  {
    return data.get_view({i_rx_port, i_layer});
  }

  // See interface for documentation.
  unsigned get_nof_re() const override { return data.get_dimension_size(ch_dims::re); }

  // See interface for documentation.
  unsigned get_nof_rx_ports() const override { return data.get_dimension_size(ch_dims::rx_port); }

  // See interface for documentation.
  unsigned get_nof_tx_layers() const override { return data.get_dimension_size(ch_dims::tx_layer); }

  /// Resize the channel estimates list.
  void resize(unsigned nof_re, unsigned nof_rx_ports, unsigned nof_layers)
  {
    data.resize({nof_re, nof_rx_ports, nof_layers});
  }

  /// Gets a raw data view.
  span<cbf16_t> get_data() { return data.get_data(); }

private:
  /// Dimensions, i.e. number of coordinates, spanned by each indexing level of the channel estimation data.
  enum class ch_dims : unsigned {
    /// Channel coefficient for a single Resource Element and a single Tx&ndash;Rx channel path.
    re = 0,
    /// Set of all channel coefficients corresponding to a single Tx&ndash;Rx channel path.
    rx_port = 1,
    /// Set of all channel coefficients corresponding to all Tx&ndash;Rx paths for a single Tx layer.
    tx_layer = 2,
    /// Total number of dimensions.
    nof_dims = 3
  };

  // Data storage as a tensor.
  dynamic_tensor<std::underlying_type_t<ch_dims>(ch_dims::nof_dims), cbf16_t, ch_dims> data;
};

} // namespace srsran