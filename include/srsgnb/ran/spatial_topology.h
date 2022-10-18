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
    /// Single Input Multiple Output (SIMO) channel with 2 Rx ports, making use of receiver-side spatial diversity.
    simo_2x1,
    /// Single Input Multiple Output (SIMO) channel with 3 Rx ports, making use of receiver-side spatial diversity.
    simo_3x1,
    /// Single Input Multiple Output (SIMO) channel with 4 Rx ports, making use of receiver-side spatial diversity.
    simo_4x1,
    /// Multiple Input Multiple Output (MIMO) channel with 2 Tx and 2 Rx ports, making use of spatial multiplexing.
    mimo_2x2
  };

  /// Default destructor.
  ~spatial_topology() = default;

  /// Default constructor.
  constexpr spatial_topology() = default;

  /// Construct from value. Allows implicit conversion from a channel spatial topology option.
  constexpr spatial_topology(options opt) : value(opt) {}

  /// Construct from a given number or Rx ports and Tx layers.
  constexpr spatial_topology(unsigned nof_rx_ports, unsigned nof_tx_layers)
  {
    if ((nof_rx_ports == 1) && (nof_tx_layers == 1)) {
      value = siso;
    } else if ((nof_rx_ports == 2) && (nof_tx_layers == 1)) {
      value = simo_2x1;
    } else if ((nof_rx_ports == 3) && (nof_tx_layers == 1)) {
      value = simo_3x1;
    } else if ((nof_rx_ports == 4) && (nof_tx_layers == 1)) {
      value = simo_4x1;
    } else if ((nof_rx_ports == 2) && (nof_tx_layers == 2)) {
      value = mimo_2x2;
    } else {
      value = invalid;
    }
  }

  /// Converts a channel topology into string format.
  std::string to_string() const
  {
    switch (value) {
      case siso:
        return "siso";
      case simo_2x1:
        return "simo_2x1";
      case mimo_2x2:
        return "mimo_2x2";
      case simo_3x1:
        return "simo_3x1";
      case simo_4x1:
        return "simo_4x1";
      case invalid:
      default:
        return "invalid";
    }
  }

  /// \name Channel dimension getters.
  ///@{

  /// Gets the number of Rx ports from a \c spatial_topology::options value.
  static constexpr unsigned get_nof_rx_ports(spatial_topology::options value)
  {
    switch (value) {
      case siso:
        return 1;
      case simo_2x1:
      case mimo_2x2:
        return 2;
      case simo_3x1:
        return 3;
      case simo_4x1:
        return 4;
      case invalid:
      default:
        return 0;
    }
  }

  /// Gets the number of Rx ports from a \c spatial_topology instance.
  constexpr unsigned get_nof_rx_ports()
  {
    switch (value) {
      case siso:
        return 1;
      case simo_2x1:
      case mimo_2x2:
        return 2;
      case simo_3x1:
        return 3;
      case simo_4x1:
        return 4;
      case invalid:
      default:
        return 0;
    }
  }

  /// Gets the number of Tx layers from a \c spatial_topology::options value.
  static constexpr unsigned get_nof_tx_layers(spatial_topology::options value)
  {
    switch (value) {
      case siso:
      case simo_2x1:
      case simo_3x1:
      case simo_4x1:
        return 1;
      case mimo_2x2:
        return 2;
      case invalid:
      default:
        return 0;
    }
  }

  /// Gets the number of Tx layers from a \c spatial_topology instance.
  constexpr unsigned get_nof_tx_layers()
  {
    switch (value) {
      case siso:
      case simo_2x1:
      case simo_3x1:
      case simo_4x1:
        return 1;
      case mimo_2x2:
        return 2;
      case invalid:
      default:
        return 0;
    }
  }
  ///@}

  /// Check if the spatial topology is valid.
  constexpr bool is_valid() const { return value != invalid; }

  /// Get the channel spatial topology.
  constexpr options get_topology() const { return value; }

  /// \name Comparison operators.
  ///@{

  ///@{
  /// \brief compare two spatial topologies.
  ///
  /// The spatial topologies are considered equal if both have the same \c value.
  bool operator==(const spatial_topology& other) const { return value == other.value; }
  bool operator!=(const spatial_topology& other) const { return value != other.value; }
  ///@}

  ///@{
  /// \brief compare a spatial topology with one of its possible values.
  ///
  /// They are considered equal if both spatial topology values match.
  bool operator==(const options& other_value) const { return value == other_value; }
  bool operator!=(const options& other_value) const { return value != other_value; }
  ///@}

  ///@}
private:
  /// Stores the channel spatial topology.
  options value = invalid;
};
} // namespace srsgnb
