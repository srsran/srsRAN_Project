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

#include "precoding_weight_matrix.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/tensor.h"
#include "srsran/ran/precoding/precoding_constants.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

/// \brief Precoder configuration.
///
/// Describes the precoding pattern to apply to a certain physical channel. The grid is divided in Physical Resource
/// Block Groups (PRGs) of size \c prg_size. The lowest PRB in the lowest PRG coincides with CRB0 (PointA).
///
/// Each PRG contains a matrix of weights per layer and port.
class precoding_configuration
{
public:
  /// Default constructor - constructs a precoder configuration with no coefficients.
  precoding_configuration() = default;

  /// \brief Constructs a weight matrix with the desired number of layers, ports and PRGs.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_ports  Number of ports.
  /// \param[in] nof_prg    Number of PRGs.
  /// \param[in] prg_size_  Number of PRB that comprise a PRG.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  /// \remark An assertion is triggered if the number of PRGs exceeds \ref precoding_constants::MAX_NOF_PRG.
  /// \remark An assertion is triggered if the PRG size is lesser than \ref precoding_constants::MIN_PRG_SIZE.
  precoding_configuration(unsigned nof_layers, unsigned nof_ports, unsigned nof_prg, unsigned prg_size_)
  {
    resize(nof_layers, nof_ports, nof_prg, prg_size_);
  }

  /// \brief Copy constructor.
  /// \param[in] other Precoding configuration to copy.
  precoding_configuration(const precoding_configuration& other)
  {
    resize(other.get_nof_layers(), other.get_nof_ports(), other.get_nof_prg(), other.get_prg_size());
    srsvec::copy(coefficients.get_data(), other.coefficients.get_data());
  }

  /// \brief Overload assigment operator.
  /// \param[in] other Precoding configuration to copy.
  precoding_configuration& operator=(const precoding_configuration& other)
  {
    resize(other.get_nof_layers(), other.get_nof_ports(), other.get_nof_prg(), other.get_prg_size());
    srsvec::copy(coefficients.get_data(), other.coefficients.get_data());
    return *this;
  }

  /// \brief Resizes the number of coefficients to a desired number of layers, ports and PRGs.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_ports  Number of ports.
  /// \param[in] nof_prg    Number of PRGs.
  /// \param[in] prg_size_  Number of PRB that comprise a PRG.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  /// \remark An assertion is triggered if the number of PRGs exceeds \ref precoding_constants::MAX_NOF_PRG.
  /// \remark An assertion is triggered if the PRG size is lesser than \ref precoding_constants::MIN_PRG_SIZE.
  void resize(unsigned nof_layers, unsigned nof_ports, unsigned nof_prg, unsigned prg_size_)
  {
    srsran_assert(nof_layers <= precoding_constants::MAX_NOF_LAYERS,
                  "The number of layers (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_layers,
                  precoding_constants::MAX_NOF_LAYERS);
    srsran_assert(nof_ports <= precoding_constants::MAX_NOF_PORTS,
                  "The number of ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_ports,
                  precoding_constants::MAX_NOF_PORTS);
    srsran_assert(nof_prg <= precoding_constants::MAX_NOF_PRG,
                  "The number of PRG (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_prg,
                  precoding_constants::MAX_NOF_PRG);
    srsran_assert(prg_size_ >= precoding_constants::MIN_PRG_SIZE,
                  "The PRG size (i.e., {}) is lesser than the minimum (i.e., {}).",
                  prg_size_,
                  precoding_constants::MIN_PRG_SIZE);
    coefficients.resize({nof_layers, nof_ports, nof_prg});
    prg_size = prg_size_;
  }

  /// Gets the current number of layers.
  unsigned get_nof_layers() const { return coefficients.get_dimension_size(dims::layer); }

  /// Gets the current number of ports.
  unsigned get_nof_ports() const { return coefficients.get_dimension_size(dims::port); }

  /// Gets the current number of PRG.
  unsigned get_nof_prg() const { return coefficients.get_dimension_size(dims::prg); }

  /// Gets the current PRG size.
  unsigned get_prg_size() const { return prg_size; }

  /// \brief Sets a specific coefficient for a given layer, port and PRG.
  /// \param[in] coefficient Coefficient value.
  /// \param[in] i_layer     Layer identifier.
  /// \param[in] i_port      Port identifier.
  /// \param[in] i_prg       PRG identifier.
  /// \remark An assertion is triggered if the layer identifier is greater than or equal to get_nof_layers().
  /// \remark An assertion is triggered if the port identifier is greater than or equal to get_nof_ports().
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  void set_coefficient(cf_t coefficient, unsigned i_layer, unsigned i_port, unsigned i_prg)
  {
    srsran_assert(i_layer < get_nof_layers(),
                  "The layer identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_layer,
                  get_nof_layers() - 1);
    srsran_assert(i_port < get_nof_ports(),
                  "The ports identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_port,
                  get_nof_ports() - 1);
    srsran_assert(i_prg < get_nof_prg(),
                  "The PRG identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_prg,
                  get_nof_prg() - 1);
    coefficients[{i_layer, i_port, i_prg}] = coefficient;
  }

  /// \brief Gets a specific coefficient for a given layer, port and PRG.
  /// \param[in] i_layer     Layer identifier.
  /// \param[in] i_port      Port identifier.
  /// \param[in] i_prg       PRG identifier.
  /// \remark An assertion is triggered if the layer identifier is greater than or equal to get_nof_layers().
  /// \remark An assertion is triggered if the port identifier is greater than or equal to get_nof_ports().
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  /// \return The specified coefficient.
  cf_t get_coefficient(unsigned i_layer, unsigned i_port, unsigned i_prg) const
  {
    srsran_assert(i_layer < get_nof_layers(),
                  "The layer identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_layer,
                  get_nof_layers() - 1);
    srsran_assert(i_port < get_nof_ports(),
                  "The ports identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_port,
                  get_nof_ports() - 1);
    srsran_assert(i_prg < get_nof_prg(),
                  "The PRG identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_prg,
                  get_nof_prg() - 1);
    return coefficients[{i_layer, i_port, i_prg}];
  }

  /// \brief Gets the precoding weights for a given PRG.
  ///
  /// \param[in] i_prg PRG identifier.
  /// \return The corresponding precoding weight matrix.
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  precoding_weight_matrix get_prg_coefficients(unsigned i_prg) const
  {
    srsran_assert((i_prg < get_nof_prg()),
                  "The requested PRG, i.e., {}, must be smaller than the number of PRG, i.e., {}.",
                  i_prg,
                  get_nof_prg());

    return precoding_weight_matrix(
        coefficients.get_view<static_cast<unsigned>(dims::prg)>({i_prg}), get_nof_layers(), get_nof_ports());
  }

private:
  /// Internal tensor dimensions.
  enum class dims : unsigned { layer = 0, port, prg, all };

  /// Number of physical resource blocks per PRG.
  unsigned prg_size = 0;
  /// Internal data storage.
  static_tensor<static_cast<unsigned>(dims::all),
                cf_t,
                precoding_constants::MAX_NOF_LAYERS * precoding_constants::MAX_NOF_PORTS *
                    precoding_constants::MAX_NOF_PRG,
                dims>
      coefficients;
};

/// \brief Overload equal to operator.
/// \param[in] lhs First precoding configuration.
/// \param[in] rhs Second precoding configuration.
/// \return True if both precoding configurations are exactly the same.
inline bool operator==(const precoding_configuration& lhs, const precoding_configuration& rhs)
{
  if (lhs.get_nof_prg() != rhs.get_nof_prg()) {
    return false;
  }
  if (lhs.get_nof_layers() != rhs.get_nof_layers()) {
    return false;
  }
  if (lhs.get_nof_ports() != rhs.get_nof_ports()) {
    return false;
  }
  if (lhs.get_prg_size() != rhs.get_prg_size()) {
    return false;
  }

  unsigned nof_prg    = lhs.get_nof_prg();
  unsigned nof_layers = lhs.get_nof_layers();
  unsigned nof_ports  = lhs.get_nof_ports();

  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        if (lhs.get_coefficient(i_layer, i_port, i_prg) != rhs.get_coefficient(i_layer, i_port, i_prg)) {
          return false;
        }
      }
    }
  }

  return true;
}

} // namespace srsran
