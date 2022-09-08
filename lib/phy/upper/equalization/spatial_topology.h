/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {
/// Represents spatial channel topologies.
class spatial_topology
{
public:
  /// Supported channel configurations.
  enum options {
    /// Invalid or unsupported channel spatial topology.
    INVALID = 0,
    /// Single Input Single Output (SISO) channel.
    SISO,
    /// Multiple Input Single Output (MISO) channel with 2 Rx ports, making use of receiver-side spatial diversity.
    MISO_2X1,
    /// Multiple Input Multiple Output (MIMO) channel with 2 Tx and 2 Rx ports, making use of spatial multiplexing.
    MIMO_2X2
  };

  /// Default destructor.
  ~spatial_topology() = default;

  /// Default constructor.
  constexpr spatial_topology() = default;

  /// Construct from value. Allows implicit conversion from a channel spatial topology option.
  explicit constexpr spatial_topology(options opt) : value(opt) {}

  /// Construct from a given number or Rx ports and Tx layers.
  constexpr spatial_topology(unsigned nof_rx_ports, unsigned nof_tx_layers)
  {
    if ((nof_rx_ports == 1) && (nof_tx_layers == 1)) {
      value = SISO;
    } else if ((nof_rx_ports == 2) && (nof_tx_layers == 1)) {
      value = MISO_2X1;
    } else if ((nof_rx_ports == 2) && (nof_tx_layers == 2)) {
      value = MIMO_2X2;
    } else {
      value = INVALID;
    }
  }

  /// Check if the spatial topology is valid.
  constexpr bool is_valid() const { return value != INVALID; }

  /// Get the channel spatial topology.
  constexpr options get_topology() const { return value; }

  /// Get the number of Tx layers of the channel, according to its spatial topology.
  constexpr unsigned get_nof_tx_layers() const
  {
    switch (value) {
      case SISO:
      case MISO_2X1:
        return 1;
      case MIMO_2X2:
        return 2;
      case INVALID:
      default:
        return 0;
    }
  }

  /// Get the number of Rx ports of the channel, according to its spatial topology.
  constexpr unsigned get_nof_rx_ports() const
  {
    switch (value) {
      case SISO:
        return 1;
      case MISO_2X1:
      case MIMO_2X2:
        return 2;
      case INVALID:
      default:
        return 0;
    }
  }

  /// \brief compares two spatial topologies.
  ///
  /// \returns \c true if both spatial topologies have the same \c value.
  bool operator==(const spatial_topology& other) { return value == other.value; }

  bool operator!=(const spatial_topology& other) { return value != other.value; }

  /// \brief compares a spatial topology with one of its possible values.
  ///
  /// \returns \c true if both spatial topology values match.
  bool operator==(const options& other_value) { return value == other_value; }

  bool operator!=(const options& other_value) { return value != other_value; }

private:
  /// Stores the channel spatial topology.
  options value = INVALID;
};
} // namespace srsgnb
