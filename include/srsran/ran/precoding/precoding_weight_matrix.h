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
#include "srsran/adt/tensor.h"
#include "srsran/ran/precoding/precoding_constants.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"

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
  ///
  /// The coefficients are set to zero.
  ///
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_ports  Number of ports.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  precoding_weight_matrix(unsigned nof_layers, unsigned nof_ports) : data({nof_layers, nof_ports})
  {
    srsran_assert(nof_layers <= precoding_constants::MAX_NOF_LAYERS,
                  "The number of layers (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_layers,
                  precoding_constants::MAX_NOF_LAYERS);
    srsran_assert(nof_ports <= precoding_constants::MAX_NOF_PORTS,
                  "The number of ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_ports,
                  precoding_constants::MAX_NOF_PORTS);

    // Zero all data.
    srsvec::zero(data.get_data());
  }

  /// \brief Constructs a precoding weight matrix with the desired number of layers and ports.
  ///
  /// Creates a precoding weight matrix with the specified dimensions, and sets its contents to the provided weight
  /// values.
  ///
  /// \param[in] weights Precoding weight list, arranged by i) layer and ii) antenna port.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_ports  Number of ports.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  precoding_weight_matrix(const std::initializer_list<cf_t>& weights, unsigned nof_layers, unsigned nof_ports) :
    precoding_weight_matrix(span<const cf_t>(weights.begin(), weights.end()), nof_layers, nof_ports)
  {
  }

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
  precoding_weight_matrix(span<const cf_t> weights, unsigned nof_layers, unsigned nof_ports) :
    data({nof_layers, nof_ports})
  {
    srsran_assert(
        weights.size() == nof_ports * nof_layers,
        "The number of weights, i.e., {}, does not match the specified matrix dimensions, i.e., {} layers, {} ports.",
        weights.size(),
        nof_layers,
        nof_ports);

    srsran_assert(nof_layers <= precoding_constants::MAX_NOF_LAYERS,
                  "The number of layers (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_layers,
                  precoding_constants::MAX_NOF_LAYERS);
    srsran_assert(nof_ports <= precoding_constants::MAX_NOF_PORTS,
                  "The number of ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_ports,
                  precoding_constants::MAX_NOF_PORTS);

    // Copy the weights into the tensor.
    srsvec::copy(data.get_view<static_cast<unsigned>(dims::all)>({}), weights);
  }

  /// Copy constructor.
  precoding_weight_matrix(const precoding_weight_matrix& other) : data({other.get_nof_layers(), other.get_nof_ports()})
  {
    // Copy the weights into the tensor.
    srsvec::copy(data.get_view<static_cast<unsigned>(dims::all)>({}),
                 other.data.get_view<static_cast<unsigned>(dims::all)>({}));
  }

  /// \brief Overload assigment operator.
  /// \param[in] other Precoding weight matrix to copy.
  precoding_weight_matrix& operator=(const precoding_weight_matrix& other)
  {
    if (this == &other) {
      return *this;
    }

    // Resize the tensor.
    resize(other.get_nof_layers(), other.get_nof_ports());
    // Copy the weights into the tensor.
    srsvec::copy(data.get_view<static_cast<unsigned>(dims::all)>({}),
                 other.data.get_view<static_cast<unsigned>(dims::all)>({}));
    return *this;
  }

  /// \brief Overload equality comparison operator.
  /// \param[in] other Precoding weight matrix to compare against.
  /// \return \c true if both precoding matrices are exactly the same, \c false otherwise.
  bool operator==(const precoding_weight_matrix& other) const
  {
    unsigned nof_layers = get_nof_layers();
    unsigned nof_ports  = get_nof_ports();

    if (nof_layers != other.get_nof_layers()) {
      return false;
    }
    if (nof_ports != other.get_nof_ports()) {
      return false;
    }

    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        if (get_coefficient(i_layer, i_port) != other.get_coefficient(i_layer, i_port)) {
          return false;
        }
      }
    }

    return true;
  }

  /// Overload inequality comparison operator.
  bool operator!=(const precoding_weight_matrix& other) const { return !(*this == other); }

  /// Gets the current number of layers.
  unsigned get_nof_layers() const { return data.get_dimension_size(dims::layer); }

  /// Gets the current number of ports.
  unsigned get_nof_ports() const { return data.get_dimension_size(dims::port); }

  /// \brief Gets a read-only view of the precoding weights for a given port.
  ///
  /// \param[in] i_port Port identifier.
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
  /// \param[in] i_port Port identifier.
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

  /// \brief Gets a precoding coefficient from the matrix.
  ///
  /// \param[in] i_layer Layer identifier.
  /// \param[in] i_port Port identifier.
  /// \return The precoding coefficient for the given layer and port.
  cf_t get_coefficient(unsigned i_layer, unsigned i_port) const
  {
    srsran_assert(i_layer < get_nof_layers(),
                  "The layer identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_layer,
                  get_nof_layers() - 1);
    srsran_assert(i_port < get_nof_ports(),
                  "The ports identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_port,
                  get_nof_ports() - 1);
    return data[{i_layer, i_port}];
  }

  /// \brief Sets a precoding coefficient in the matrix to a specified value.
  ///
  /// \param[in] i_layer Layer identifier.
  /// \param[in] i_port Port identifier.
  void set_coefficient(cf_t coefficient, unsigned i_layer, unsigned i_port)
  {
    srsran_assert(i_layer < get_nof_layers(),
                  "The layer identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_layer,
                  get_nof_layers() - 1);
    srsran_assert(i_port < get_nof_ports(),
                  "The ports identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_port,
                  get_nof_ports() - 1);
    data[{i_layer, i_port}] = coefficient;
  }

  /// Scales all the weights by a scaling factor.
  precoding_weight_matrix& operator*=(float scale)
  {
    srsvec::sc_prod(data.get_data(), scale, data.get_data());
    return *this;
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
