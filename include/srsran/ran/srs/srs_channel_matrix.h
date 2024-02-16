/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/adt/tensor.h"
#include "srsran/ran/srs/srs_constants.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/support/srsran_assert.h"
#include <cstdint>
#include <initializer_list>

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
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports  Number of transmit ports.
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
                  nof_rx_ports,
                  srs_constants::max_nof_rx_ports);
  }

  /// \brief Constructs a channel matrix with the desired number of receive and transmit ports.
  ///
  /// Creates a channel matrix with the specified dimensions, and sets its contents to the provided coefficients
  /// values.
  ///
  /// \param[in] coefficients Channel coefficient list, arranged by i) receive port and ii) transmit port.
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports  Number of transmit ports.
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
  /// \param[in] nof_tx_ports  Number of transmit ports.
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
    srsvec::copy(data.get_view<static_cast<unsigned>(dims::all)>({}), coefficients);
  }

  /// Copy constructor.
  srs_channel_matrix(const srs_channel_matrix& other) : data({other.get_nof_rx_ports(), other.get_nof_tx_ports()})
  {
    // Copy the weights into the tensor.
    srsvec::copy(data.get_view<static_cast<unsigned>(dims::all)>({}),
                 other.data.get_view<static_cast<unsigned>(dims::all)>({}));
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
    srsvec::copy(data.get_view<static_cast<unsigned>(dims::all)>({}),
                 other.data.get_view<static_cast<unsigned>(dims::all)>({}));
    return *this;
  }

  /// \brief Overload equality comparison operator.
  /// \param[in] other Channel matrix to compare against.
  /// \return \c true if both channel matrices are exactly the same, \c false otherwise.
  bool operator==(const srs_channel_matrix& other) const
  {
    unsigned nof_rx_ports = get_nof_rx_ports();
    unsigned nof_tx_ports = get_nof_tx_ports();

    if (nof_rx_ports != other.get_nof_rx_ports()) {
      return false;
    }
    if (nof_tx_ports != other.get_nof_tx_ports()) {
      return false;
    }

    for (unsigned i_layer = 0; i_layer != nof_rx_ports; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
        if (get_coefficient(i_layer, i_port) != other.get_coefficient(i_layer, i_port)) {
          return false;
        }
      }
    }

    return true;
  }

  /// Overload inequality comparison operator.
  bool operator!=(const srs_channel_matrix& other) const { return !(*this == other); }

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

  /// Scales all the coefficients by a scaling factor.
  srs_channel_matrix& operator*=(float scale)
  {
    srsvec::sc_prod(data.get_data(), scale, data.get_data());
    return *this;
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