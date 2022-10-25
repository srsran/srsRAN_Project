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
    invalid = 0,
    /// Single Input Single Output (SISO) channel.
    siso,
    /// Single Input Multiple Output (SIMO) channel.
    simo,
    /// Multiple Input Multiple Output (MIMO) channel.
    mimo
  };

  /// Default destructor.
  ~spatial_topology() = default;

  /// Default constructor.
  constexpr spatial_topology() = default;

  /// Construct from a given number or Rx ports and Tx layers.
  constexpr spatial_topology(unsigned nof_rx_ports, unsigned nof_tx_layers)
  {
    // Determine the channel type.
    if (nof_tx_layers == 1) {
      if (nof_rx_ports == 1) {
        value = siso;
      } else if ((nof_rx_ports > 1) && (nof_rx_ports <= MAX_PORTS)) {
        value = simo;
      } else {
        value = invalid;
      }
    } else if ((nof_tx_layers == 2) && (nof_rx_ports == 2)) {
      value = mimo;
    } else {
      value = invalid;
    }

    if (value != invalid) {
      this->nof_rx_ports  = nof_rx_ports;
      this->nof_tx_layers = nof_tx_layers;
    }
  }

  /// Converts a channel topology into string format.
  std::string to_string() const
  {
    switch (value) {
      case siso:
        return "siso";
      case simo:
        return "simo" + fmt::to_string(nof_tx_layers) + "x" + fmt::to_string(nof_rx_ports);
      case mimo:
        return "mimo" + fmt::to_string(nof_tx_layers) + "x" + fmt::to_string(nof_rx_ports);
      case invalid:
      default:
        return "invalid";
    }
  }

  /// \name Channel dimension getters.
  ///@{

  /// Gets the number of Rx ports from a \c spatial_topology instance.
  constexpr unsigned get_nof_rx_ports() const { return nof_rx_ports; }

  /// Gets the number of Tx layers from a \c spatial_topology instance.
  constexpr unsigned get_nof_tx_layers() const { return nof_tx_layers; }
  ///@}

  /// Check if the spatial topology is valid.
  constexpr bool is_valid() const { return value != invalid; }

  /// Get the channel spatial topology.
  constexpr options get_topology() const { return value; }

  ///@{
  /// \brief Compare two spatial topologies.
  ///
  /// The spatial topologies are considered equal if both have the same number of receive ports and transmit layers.
  bool operator==(const spatial_topology& other) const
  {
    return (value == other.value) && (nof_rx_ports == other.nof_rx_ports) && (nof_tx_layers == other.nof_tx_layers);
  }
  bool operator!=(const spatial_topology& other) const
  {
    return (value != other.value) || (nof_rx_ports != other.nof_rx_ports) || (nof_tx_layers != other.nof_tx_layers);
  }
  ///@}

private:
  /// Stores the channel spatial topology.
  options value = invalid;

  unsigned nof_rx_ports  = 0;
  unsigned nof_tx_layers = 0;
};
} // namespace srsgnb
