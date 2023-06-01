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
#include "srsran/adt/tensor.h"
#include "srsran/ran/precoding/precoding_constants.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

/// Precoding matrix, consisting of complex coefficients arranged by i) transmit layers and ii) antenna ports.
class precoding_weight_matrix
{
public:
  /// Precoding coefficient dimensions.
  enum class dims : unsigned { layer = 0, port, all };

  /// Default constructor - constructs a precoding weight matrix with no coefficients.
  precoding_weight_matrix() = default;

  /// \brief Constructs a weight matrix with the desired number of layers and ports.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_ports  Number of ports.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  precoding_weight_matrix(unsigned nof_layers, unsigned nof_ports) { resize(nof_layers, nof_ports); }

  /// \brief Constructs a weight matrix with the desired number of layers and ports.
  ///
  /// Creates a precoding weight matrix with the specified dimensions, and sets its contents to the provided weight
  /// values.
  ///
  /// \param[in] weights Precoding weight list, arranged by i) layer and ii) antenna port.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_ports  Number of ports.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  precoding_weight_matrix(span<const cf_t> weights, unsigned nof_layers, unsigned nof_ports)
  {
    srsran_assert(
        weights.size() == nof_ports * nof_layers,
        "The number of weights, i.e., {}, does not match the specified matrix dimensions, i.e., {} layers, {} ports.",
        weights.size(),
        nof_layers,
        nof_ports);

    // Resize the tensor.
    resize(nof_layers, nof_ports);
    // Copy the weights into the tensor.
    srsvec::copy(data.get_view<static_cast<unsigned>(dims::all)>({}), weights);
  }

  /// Gets the current number of layers.
  unsigned get_nof_layers() const { return data.get_dimension_size(dims::layer); }

  /// Gets the current number of ports.
  unsigned get_nof_ports() const { return data.get_dimension_size(dims::port); }

  /// \brief Gets a read-only view of the precoding weights for a given port.
  ///
  /// \param[in] i_port port identifier.
  /// \return The corresponding precoding weights, indexed by layer.
  /// \remark An assertion is triggered if the port identifier is greater than or equal to get_nof_ports().
  span<const cf_t> get_port_coefficients(unsigned i_port) const
  {
    srsran_assert((i_port < get_nof_ports()),
                  "The requested antenna port, i.e., {}, must be smaller than the number of ports, i.e., {}.",
                  i_port,
                  get_nof_ports());

    return data.get_view({i_port});
  }

  /// \brief Gets a read-write view of the precoding weights for a given port.
  ///
  /// \param[in] i_port port identifier.
  /// \return The corresponding precoding weights, indexed by layer.
  /// \remark An assertion is triggered if the port identifier is greater than or equal to get_nof_ports().
  span<cf_t> get_port_coefficients(unsigned i_port)
  {
    srsran_assert((i_port < get_nof_ports()),
                  "The requested antenna port, i.e., {}, must be smaller than the number of ports, i.e., {}.",
                  i_port,
                  get_nof_ports());

    return data.get_view({i_port});
  }

private:
  /// \brief Resizes the number of coefficients to a desired number of layers and ports.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_ports  Number of ports.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  void resize(unsigned nof_layers, unsigned nof_ports)
  {
    srsran_assert(nof_layers <= precoding_constants::MAX_NOF_LAYERS,
                  "The number of layers (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_layers,
                  precoding_constants::MAX_NOF_LAYERS);
    srsran_assert(nof_ports <= precoding_constants::MAX_NOF_PORTS,
                  "The number of ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_ports,
                  precoding_constants::MAX_NOF_PORTS);

    data.resize({nof_layers, nof_ports});
  }

  /// Internal data storage.
  static_tensor<static_cast<unsigned>(dims::all),
                cf_t,
                precoding_constants::MAX_NOF_LAYERS * precoding_constants::MAX_NOF_PORTS,
                dims>
      data;
};

} // namespace srsran
