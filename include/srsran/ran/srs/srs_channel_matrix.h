/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/ran/srs/srs_constants.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_assert.h"
#include <cmath>
#include <complex>
#include <cstdint>
#include <initializer_list>
#include <numeric>

namespace srsran {

/// \brief SRS-based estimated channel matrix.
///
/// Wideband channel matrix estimated from the Sounding Reference Signals (SRS). The complex channel coefficients are
/// arranged by i) receive ports and ii) transmit ports.
class srs_channel_matrix
{
public:
  /// Channel matrix dimensions.
  enum class dims : uint8_t { rx_port = 0, tx_port, all };

  /// Default constructor - constructs a channel matrix with no coefficients.
  srs_channel_matrix() = default;

  /// \brief Constructs a channel matrix with the desired number of transmit and receive ports.
  ///
  /// The channel coefficients are initialized to zero.
  ///
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds \ref srs_constants::max_nof_rx_ports.
  /// \remark An assertion is triggered if the number of transmit ports exceeds \ref srs_constants::max_nof_tx_ports.
  srs_channel_matrix(unsigned nof_rx_ports, unsigned nof_tx_ports) : data({nof_rx_ports, nof_tx_ports})
  {
    srsran_assert(nof_rx_ports <= srs_constants::max_nof_rx_ports,
                  "The number of receive ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_rx_ports,
                  srs_constants::max_nof_rx_ports);
    srsran_assert(nof_tx_ports <= srs_constants::max_nof_tx_ports,
                  "The number of transmit ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_tx_ports,
                  srs_constants::max_nof_tx_ports);
    // Initialize coefficients to zero.
    srsvec::zero(data.get_data());
  }

  /// \brief Constructs a channel matrix with the desired number of receive and transmit ports.
  ///
  /// Creates a channel matrix with the specified dimensions, and sets its contents to the provided coefficients.
  ///
  /// \param[in] coefficients Channel coefficient list, arranged by i) receive port and ii) transmit port.
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds \ref srs_constants::max_nof_rx_ports.
  /// \remark An assertion is triggered if the number of transmit ports exceeds \ref srs_constants::max_nof_tx_ports.
  srs_channel_matrix(const std::initializer_list<cf_t>& coefficients, unsigned nof_rx_ports, unsigned nof_tx_ports) :
    srs_channel_matrix(span<const cf_t>(coefficients.begin(), coefficients.end()), nof_rx_ports, nof_tx_ports)
  {
  }

  /// \brief Constructs a channel matrix with the desired number of receive and transmit ports.
  ///
  /// Creates a channel matrix with the specified dimensions, and sets its contents to the provided coefficients
  /// values.
  ///
  /// \param[in] coefficients Channel coefficient list, arranged by i) receive port and ii) transmit port.
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds \ref srs_constants::max_nof_rx_ports.
  /// \remark An assertion is triggered if the number of transmit ports exceeds \ref srs_constants::max_nof_tx_ports.
  srs_channel_matrix(span<const cf_t> coefficients, unsigned nof_rx_ports, unsigned nof_tx_ports) :
    data({nof_rx_ports, nof_tx_ports})
  {
    srsran_assert(coefficients.size() == nof_rx_ports * nof_tx_ports,
                  "The number of coefficients, i.e., {}, does not match the specified matrix dimensions, i.e., {} "
                  "transmit ports, {} receive ports.",
                  coefficients.size(),
                  nof_tx_ports,
                  nof_rx_ports);

    srsran_assert(nof_rx_ports <= srs_constants::max_nof_rx_ports,
                  "The number of receive ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_rx_ports,
                  srs_constants::max_nof_rx_ports);
    srsran_assert(nof_tx_ports <= srs_constants::max_nof_tx_ports,
                  "The number of transmit ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_tx_ports,
                  srs_constants::max_nof_tx_ports);

    // Copy the weights into the tensor.
    srsvec::copy(data.get_data(), coefficients);
  }

  /// Copy constructor.
  srs_channel_matrix(const srs_channel_matrix& other) : data({other.get_nof_rx_ports(), other.get_nof_tx_ports()})
  {
    // Copy the weights into the tensor.
    srsvec::copy(data.get_data(), other.data.get_data());
  }

  /// \brief Overload assignment operator.
  /// \param[in] other Channel matrix to copy.
  srs_channel_matrix& operator=(const srs_channel_matrix& other)
  {
    if (this == &other) {
      return *this;
    }

    // Resize the tensor.
    resize(other.get_nof_rx_ports(), other.get_nof_tx_ports());
    // Copy the weights into the tensor.
    srsvec::copy(data.get_data(), other.data.get_data());
    return *this;
  }

  /// \brief Near equal comparison method.
  ///
  /// Checks whether two matrices are similar, that is whether their Frobenius distance does not exceed the given
  /// tolerance, relative to the norm of the first matrix.
  ///
  /// \param[in] other     Channel matrix to compare against.
  /// \param[in] tolerance Maximum relative error.
  /// \return \c true if the relative distance between the two channel matrices is lower than \c tolerance, \c false
  ///         otherwise.
  bool is_near(const srs_channel_matrix& other, float tolerance) const
  {
    unsigned nof_rx_ports = get_nof_rx_ports();
    unsigned nof_tx_ports = get_nof_tx_ports();

    if (nof_rx_ports != other.get_nof_rx_ports()) {
      return false;
    }
    if (nof_tx_ports != other.get_nof_tx_ports()) {
      return false;
    }

    // Normalize both matrices.
    srs_channel_matrix left  = this->normalize();
    srs_channel_matrix right = other.normalize();

    // Calculate the difference of both matrices.
    srs_channel_matrix diff = left - right;

    // Finally, calculate the distance between the matrices.
    return (diff.frobenius_norm() / left.frobenius_norm()) < tolerance;
  }

  /// Gets the current number of receive ports.
  unsigned get_nof_rx_ports() const { return data.get_dimension_size(dims::rx_port); }

  /// Gets the current number of transmit ports.
  unsigned get_nof_tx_ports() const { return data.get_dimension_size(dims::tx_port); }

  /// \brief Gets a channel coefficient from the matrix.
  ///
  /// \param[in] i_rx_port Receive port index.
  /// \param[in] i_tx_port Transmit port index.
  /// \return The channel coefficient for the given transmit and receive ports.
  cf_t get_coefficient(unsigned i_rx_port, unsigned i_tx_port) const
  {
    srsran_assert(i_rx_port < get_nof_rx_ports(),
                  "The receive port index (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_rx_port,
                  get_nof_rx_ports() - 1);
    srsran_assert(i_tx_port < get_nof_tx_ports(),
                  "The transmit port index (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_tx_port,
                  get_nof_tx_ports() - 1);
    return data[{i_rx_port, i_tx_port}];
  }

  /// \brief Sets a channel coefficient in the matrix to a specified value.
  ///
  /// \param[in] coefficient Channel coefficient to set.
  /// \param[in] i_rx_port   Receive port index.
  /// \param[in] i_tx_port   Transmit port index.
  void set_coefficient(cf_t coefficient, unsigned i_rx_port, unsigned i_tx_port)
  {
    srsran_assert(i_rx_port < get_nof_rx_ports(),
                  "The receive port index (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_rx_port,
                  get_nof_rx_ports() - 1);
    srsran_assert(i_tx_port < get_nof_tx_ports(),
                  "The transmit port index (i.e., {}) exceeds the maximum (i.e., {}).",
                  i_tx_port,
                  get_nof_tx_ports() - 1);
    data[{i_rx_port, i_tx_port}] = coefficient;
  }

  /// Scales all the coefficients by a real scaling factor.
  srs_channel_matrix& operator*=(float scale)
  {
    srsvec::sc_prod(data.get_data(), data.get_data(), scale);
    return *this;
  }

  /// Scales all the coefficients by a complex scaling factor.
  srs_channel_matrix& operator*=(cf_t scale)
  {
    srsvec::sc_prod(data.get_data(), data.get_data(), scale);
    return *this;
  }

  /// Calculates the Frobenius norm of the channel matrix.
  float frobenius_norm() const
  {
    span<const cf_t> raw_data = data.get_data();

    float square_sum =
        std::accumulate(raw_data.begin(), raw_data.end(), 0.0F, [](float sum, cf_t in) { return sum + std::norm(in); });

    return std::sqrt(square_sum);
  }

  /// \brief Subtracts two channel matrices.
  /// \param[in] other The matrix to subtract.
  /// \return The difference matrix.
  /// \remark An assertion is triggered if the dimensions are not equal.
  srs_channel_matrix operator-(const srs_channel_matrix& other) const
  {
    srsran_assert(get_nof_tx_ports() == other.get_nof_tx_ports(), "The number of transmit ports is not equal.");
    srsran_assert(get_nof_rx_ports() == other.get_nof_rx_ports(), "The number of receive ports is not equal.");

    unsigned nof_rx_ports = get_nof_rx_ports();
    unsigned nof_tx_ports = get_nof_tx_ports();

    srs_channel_matrix result(nof_rx_ports, nof_tx_ports);
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
        result.set_coefficient(
            get_coefficient(i_rx_port, i_tx_port) - other.get_coefficient(i_rx_port, i_tx_port), i_rx_port, i_tx_port);
      }
    }

    return result;
  }

  /// \brief Normalizes the channel matrix.
  ///
  /// Applies a scaling factor to all the channel matrix coefficients. The scaling magnitude is equal to the square
  /// root of the number of coefficients divided by the Frobenius norm. The scaling argument is the opposite of the
  /// first coefficient argument.
  ///
  /// \return A normalized channel matrix with purely-real first coefficient and Frobenius norm equal to the square root
  /// of the number of elements.
  srs_channel_matrix normalize() const
  {
    unsigned nof_rx_ports = get_nof_rx_ports();
    unsigned nof_tx_ports = get_nof_tx_ports();

    float argument  = -std::arg(get_coefficient(0, 0));
    float amplitude = std::sqrt(nof_rx_ports * nof_tx_ports) / frobenius_norm();
    cf_t  scaling   = std::polar(amplitude, argument);

    srs_channel_matrix result = *this;
    result *= scaling;
    return result;
  }

private:
  /// \brief Resizes the number of coefficients to a desired number of receive and transmit ports.
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds \ref srs_constants::max_nof_rx_ports.
  /// \remark An assertion is triggered if the number of transmit ports exceeds \ref srs_constants::max_nof_tx_ports.
  void resize(unsigned nof_rx_ports, unsigned nof_tx_ports)
  {
    srsran_assert(nof_rx_ports <= srs_constants::max_nof_rx_ports,
                  "The number of receive ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_rx_ports,
                  srs_constants::max_nof_rx_ports);
    srsran_assert(nof_tx_ports <= srs_constants::max_nof_tx_ports,
                  "The number of transmit ports (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_tx_ports,
                  srs_constants::max_nof_tx_ports);

    data.resize({nof_rx_ports, nof_tx_ports});
  }

  /// Internal data storage.
  static_tensor<static_cast<unsigned>(dims::all),
                cf_t,
                srs_constants::max_nof_rx_ports * srs_constants::max_nof_tx_ports,
                dims>
      data;
};

} // namespace srsran
