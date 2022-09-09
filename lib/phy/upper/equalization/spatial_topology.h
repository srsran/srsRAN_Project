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
    /// Multiple Input Single Output (MISO) channel with 2 Rx ports, making use of receiver-side spatial diversity.
    miso_2x1,
    /// Multiple Input Multiple Output (MIMO) channel with 2 Tx and 2 Rx ports, making use of spatial multiplexing.
    mimo_2x2
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
      value = siso;
    } else if ((nof_rx_ports == 2) && (nof_tx_layers == 1)) {
      value = miso_2x1;
    } else if ((nof_rx_ports == 2) && (nof_tx_layers == 2)) {
      value = mimo_2x2;
    } else {
      value = invalid;
    }
  }

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
