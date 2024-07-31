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

/// Implements the list of channel estimates with views to each of the channels.
template <unsigned MaxNofElements>
class modular_ch_est_list : public channel_equalizer::ch_est_list
{
public:
  /// \brief Creates a list of channel estimates from a maximum number of receive ports and layers.
  /// \param[in] max_nof_rx_ports_ Maximum number of receive ports.
  /// \param[in] max_nof_layers_   Maximum number of layers.
  modular_ch_est_list(unsigned max_nof_rx_ports_, unsigned max_nof_layers_) :
    max_nof_rx_ports(max_nof_rx_ports_), max_nof_layers(max_nof_layers_), data({max_nof_rx_ports, max_nof_layers})
  {
    srsran_assert(max_nof_rx_ports * max_nof_layers <= MaxNofElements,
                  "The maximum number of layers (i.e., {}) times the maximum number of ports (i.e., {}) exceeds the "
                  "maximum number of elements (i.e., {})",
                  max_nof_layers,
                  max_nof_rx_ports,
                  MaxNofElements);
  }

  /// \brief Sets the contents of a channel.
  ///
  /// \param[in] buffer    Channel view for the given port and layer.
  /// \param[in] i_rx_port Receive port index.
  /// \param[in] i_layer   Transmit layer index.
  void set_channel(span<const cbf16_t> buffer, unsigned i_rx_port, unsigned i_layer)
  {
    srsran_assert(buffer.size() == nof_re,
                  "Buffer size (i.e., {}) is not equal to the number of configured RE (i.e., {}).",
                  buffer.size(),
                  nof_re);
    data[{i_rx_port, i_layer}] = buffer;
  }

  /// \brief Resizes the channel estimates list.
  ///
  /// \remark An assertion is triggered if the number of receive ports exceeds the maximum number of receive ports.
  /// \remark An assertion is triggered if the number of layers exceeds the maximum number of layers.
  void resize(unsigned nof_re_, unsigned nof_rx_ports, unsigned nof_layers)
  {
    srsran_assert(nof_rx_ports <= max_nof_rx_ports,
                  "The number of receive ports (i.e., {}) exceeds the maximum number of receive ports (i.e., {}).",
                  nof_rx_ports,
                  max_nof_rx_ports);
    srsran_assert(nof_layers <= max_nof_layers,
                  "The number of layers (i.e., {}) exceeds the maximum number of layers (i.e., {}).",
                  nof_rx_ports,
                  max_nof_layers);
    nof_re = nof_re_;
    data.resize({nof_rx_ports, nof_layers});

    // Reset buffer views.
    for (span<const cbf16_t>& view : data.get_data()) {
      view = {};
    }
  }

  // See interface for documentation.
  span<const cbf16_t> get_channel(unsigned i_rx_port, unsigned i_layer) const override
  {
    return data[{i_rx_port, i_layer}];
  }

  // See interface for documentation.
  unsigned get_nof_re() const override { return nof_re; }

  // See interface for documentation.
  unsigned get_nof_rx_ports() const override { return data.get_dimension_size(ch_dims::rx_port); }

  // See interface for documentation.
  unsigned get_nof_tx_layers() const override { return data.get_dimension_size(ch_dims::tx_layer); }

private:
  /// Dimensions, i.e. number of coordinates, spanned by each indexing level of the channel estimation data.
  enum class ch_dims : unsigned {
    /// Set of all channel coefficients corresponding to a single Tx&ndash;Rx channel path.
    rx_port = 0,
    /// Set of all channel coefficients corresponding to all Tx&ndash;Rx paths for a single Tx layer.
    tx_layer = 1,
    /// Total number of dimensions.
    nof_dims = 2
  };

  /// Number of resource elements.
  unsigned nof_re = 0;
  /// Maximum number of receive ports.
  unsigned max_nof_rx_ports;
  /// Maximum number of layers.
  unsigned max_nof_layers;
  /// Data storage as a tensor of views for each channel.
  static_tensor<std::underlying_type_t<ch_dims>(ch_dims::nof_dims), span<const cbf16_t>, MaxNofElements, ch_dims> data;
};

} // namespace srsran