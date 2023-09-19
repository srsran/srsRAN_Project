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

/// \file
/// Channel estimation-related declarations.

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/channel_state_information.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Describes channel estimation results.
/// \warning Instantiating an object of this class entails a heap memory allocation.
class channel_estimate
{
private:
  /// Maximum supported number of receive ports.
  static constexpr unsigned MAX_RX_PORTS = 16;

  /// Maximum supported number of transmission layers.
  static constexpr unsigned MAX_TX_LAYERS = pusch_constants::MAX_NOF_LAYERS;

  /// Maximum number of transmit&ndash;receive paths.
  static constexpr unsigned MAX_TX_RX_PATHS = MAX_TX_LAYERS * MAX_RX_PORTS;

  /// Maximum total number of REs in a channel estimate.
  static constexpr unsigned MAX_BUFFER_SIZE = MAX_TX_RX_PATHS * MAX_RB * NRE * MAX_NSYMB_PER_SLOT;

public:
  /// Describes the data structure containing the channel estimate.
  struct channel_estimate_dimensions {
    /// Number of contiguous PRBs considered for channel estimation.
    unsigned nof_prb = 0;
    /// Number of contiguous OFDM symbols considered for channel estimation.
    unsigned nof_symbols = 0;
    /// Number of receive ports.
    unsigned nof_rx_ports = 0;
    /// Number of transmit layers.
    unsigned nof_tx_layers = 0;
  };

  /// Default constructor: reserves internal memory.
  channel_estimate() : nof_subcarriers(0), nof_symbols(0), nof_rx_ports(0), nof_tx_layers(0)
  {
    ce.reserve({MAX_RB * NRE, MAX_NSYMB_PER_SLOT, MAX_RX_PORTS, MAX_TX_LAYERS});
    noise_variance.reserve(MAX_TX_RX_PATHS);
    epre.reserve(MAX_TX_RX_PATHS);
    rsrp.reserve(MAX_TX_RX_PATHS);
    snr.reserve(MAX_TX_RX_PATHS);
    time_alignment.reserve(MAX_TX_RX_PATHS);
  }

  /// Constructor: sets the size of the internal buffers.
  explicit channel_estimate(const channel_estimate_dimensions& dims) :
    nof_subcarriers(dims.nof_prb * NRE),
    nof_symbols(dims.nof_symbols),
    nof_rx_ports(dims.nof_rx_ports),
    nof_tx_layers(dims.nof_tx_layers),
    ce({nof_subcarriers, nof_symbols, nof_rx_ports, nof_tx_layers})
  {
    srsran_assert(dims.nof_prb <= MAX_RB, "Requested {} RBs, but at most {} are allowed.", dims.nof_prb, MAX_RB);
    srsran_assert(dims.nof_symbols <= MAX_NSYMB_PER_SLOT,
                  "Requested {} OFDM symbols, but at most {} are allowed.",
                  dims.nof_symbols,
                  MAX_NSYMB_PER_SLOT);
    srsran_assert(dims.nof_rx_ports <= MAX_RX_PORTS,
                  "Requested {} receive ports, but at most {} are supported.",
                  dims.nof_rx_ports,
                  static_cast<unsigned>(MAX_RX_PORTS));
    srsran_assert(dims.nof_tx_layers <= MAX_TX_LAYERS,
                  "Requested {} transmission layers, but at most {} are supported.",
                  dims.nof_tx_layers,
                  static_cast<unsigned>(MAX_TX_LAYERS));

    unsigned nof_paths = dims.nof_tx_layers * dims.nof_rx_ports;

    // Reserve memory for channel estimates and initialize with 1.0.
    ce.resize({dims.nof_prb * NRE, dims.nof_symbols, dims.nof_rx_ports, dims.nof_tx_layers});

    // Set all reserved data to one.
    span<cf_t> data = ce.get_view<4>({});
    std::fill(data.begin(), data.end(), 1.0F);

    // Reserve memory for the rest of channel statistics.
    noise_variance.reserve(MAX_TX_RX_PATHS);
    noise_variance.resize(nof_paths);
    epre.reserve(MAX_TX_RX_PATHS);
    epre.resize(nof_paths);
    rsrp.reserve(MAX_TX_RX_PATHS);
    rsrp.resize(nof_paths);
    snr.reserve(MAX_TX_RX_PATHS);
    snr.resize(nof_paths);
    time_alignment.reserve(MAX_TX_RX_PATHS);
    time_alignment.resize(nof_paths);
  }

  /// Default destructor
  ~channel_estimate() = default;

  /// \name Getters.
  ///@{
  /// Returns the estimated noise variance for the path between the given Rx port and Tx layer (linear scale).
  float get_noise_variance(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return noise_variance[path_to_index(rx_port, tx_layer)];
  }

  /// Returns the estimated noise variance for the path between the given Rx port and Tx layer (dB scale).
  float get_noise_variance_dB(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return convert_power_to_dB(get_noise_variance(rx_port, tx_layer));
  }

  /// Returns the estimated RSRP for the path between the given Rx port and Tx layer (linear scale).
  float get_rsrp(unsigned rx_port, unsigned tx_layer = 0) const { return rsrp[path_to_index(rx_port, tx_layer)]; }

  /// Returns the estimated RSRP for the path between the given Rx port and Tx layer (dB scale).
  float get_rsrp_dB(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return convert_power_to_dB(get_rsrp(rx_port, tx_layer));
  }

  /// \brief Returns the average EPRE for the path between the given Rx port and Tx layer (linear scale).
  ///
  /// \remark The EPRE is defined as the average received power (including noise) across all REs carrying DM-RS.
  float get_epre(unsigned rx_port, unsigned tx_layer = 0) const { return epre[path_to_index(rx_port, tx_layer)]; }

  /// \brief Returns the average EPRE for the path between the given Rx port and Tx layer (dB scale).
  ///
  /// \remark The EPRE is defined as the average received power (including noise) across all REs carrying DM-RS.
  float get_epre_dB(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return convert_power_to_dB(get_epre(rx_port, tx_layer));
  }

  /// Returns the estimated SNR for the path between the given Rx port and Tx layer (linear scale).
  float get_snr(unsigned rx_port, unsigned tx_layer = 0) const { return snr[path_to_index(rx_port, tx_layer)]; }

  /// Returns the estimated average SNR for a given layer (linear scale).
  float get_layer_average_snr(unsigned tx_layer = 0) const
  {
    float noise_var_all_ports = 0.0F;
    float rsrp_all_ports      = 0.0F;

    // Add the noise and signal power contributions of all Rx ports.
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      noise_var_all_ports += noise_variance[path_to_index(i_rx_port, tx_layer)];
      rsrp_all_ports += rsrp[path_to_index(i_rx_port, tx_layer)];
    }

    if (std::isnormal(noise_var_all_ports)) {
      return rsrp_all_ports / noise_var_all_ports;
    }

    // If noise variance is 0, report and SNR of 60 dB.
    return 1e6;
  }

  /// Returns the estimated SNR for the path between the given Rx port and Tx layer (dB scale).
  float get_snr_dB(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return convert_power_to_dB(get_snr(rx_port, tx_layer));
  }

  /// Returns the estimated time alignment in PHY time units between the given Rx port and Tx layer.
  phy_time_unit get_time_alignment(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return time_alignment[path_to_index(rx_port, tx_layer)];
  }

  /// \brief Returns a read-write view to the RE channel estimates of the path between the given Rx port and Tx layer.
  ///
  /// The view is represented as a vector indexed by i) subcarriers and ii) OFDM symbols.
  span<cf_t> get_path_ch_estimate(unsigned rx_port, unsigned tx_layer = 0)
  {
    return ce.get_view<2>({rx_port, tx_layer});
  }

  /// \brief Returns a read-only view to the RE channel estimates of the path between the given Rx port and Tx layer.
  ///
  /// The view is represented as a vector indexed by i) subcarriers and ii) OFDM symbols.
  span<const cf_t> get_path_ch_estimate(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return ce.get_view<2>({rx_port, tx_layer});
  }

  /// \brief Returns a read-write view to the RE channel estimates for a given OFDM symbol, Rx port and Tx layer.
  ///
  /// The view is represented as a vector indexed by subcarrier.
  span<cf_t> get_symbol_ch_estimate(unsigned i_symbol, unsigned rx_port = 0, unsigned tx_layer = 0)
  {
    return ce.get_view<1>({i_symbol, rx_port, tx_layer});
  }

  /// \brief Returns a read-only view to the RE channel estimates for a given OFDM symbol, Rx port and Tx layer.
  ///
  /// The view is represented as a vector indexed by subcarrier.
  span<const cf_t> get_symbol_ch_estimate(unsigned i_symbol, unsigned rx_port = 0, unsigned tx_layer = 0) const
  {
    return ce.get_view<1>({i_symbol, rx_port, tx_layer});
  }

  /// \brief Gets the general Channel State Information.
  ///
  /// param[in] csi Channel State Information object where the CSI parameters are stored.
  void get_channel_state_information(channel_state_information& csi) const
  {
    // EPRE, RSRP and time alignment are reported as a linear average of the results for all Rx ports.
    float    epre_lin      = 0.0F;
    float    rsrp_lin      = 0.0F;
    unsigned best_rx_port  = 0;
    float    best_path_snr = 0.0F;
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      // Accumulate EPRE and RSRP values.
      epre_lin += get_epre(i_rx_port, 0);
      rsrp_lin += get_rsrp(i_rx_port, 0);

      // Determine the Rx port with better SNR.
      float port_snr = get_snr(i_rx_port, 0);
      if (port_snr > best_path_snr) {
        best_path_snr = port_snr;
        best_rx_port  = i_rx_port;
      }
    }

    epre_lin /= static_cast<float>(nof_rx_ports);
    rsrp_lin /= static_cast<float>(nof_rx_ports);

    csi.set_epre(convert_power_to_dB(epre_lin));
    csi.set_rsrp(convert_power_to_dB(rsrp_lin));

    // Use the time alignment of the channel path with better SNR.
    csi.set_time_alignment(get_time_alignment(best_rx_port, 0));

    // SINR is reported by averaging the signal and noise power contributions of all Rx ports.
    csi.set_sinr_dB(channel_state_information::sinr_type::channel_estimator,
                    convert_power_to_dB(get_layer_average_snr(0)));
  }

  ///@}

  /// \name Setters.
  ///@{
  /// Sets the estimated noise variance for the path between the given Rx port and Tx layer (linear scale).
  void set_noise_variance(float var_val, unsigned rx_port, unsigned tx_layer = 0)
  {
    noise_variance[path_to_index(rx_port, tx_layer)] = var_val;
  }

  /// Sets the estimated RSRP for the path between the given Rx port and Tx layer (linear scale).
  void set_rsrp(float rsrp_val, unsigned rx_port, unsigned tx_layer = 0)
  {
    rsrp[path_to_index(rx_port, tx_layer)] = rsrp_val;
  }

  /// Sets the average EPRE for the path between the given Rx port and Tx layer (linear scale).
  void set_epre(float epre_val, unsigned rx_port, unsigned tx_layer = 0)
  {
    epre[path_to_index(rx_port, tx_layer)] = epre_val;
  }

  /// Sets the estimated SNR for the path between the given Rx port and Tx layer (linear scale).
  void set_snr(float snr_val, unsigned rx_port, unsigned tx_layer = 0)
  {
    snr[path_to_index(rx_port, tx_layer)] = snr_val;
  }

  /// Sets the estimated time alignment in PHY units of time for the path between the given Rx port and Tx layer.
  void set_time_alignment(phy_time_unit ta, unsigned rx_port, unsigned tx_layer = 0)
  {
    time_alignment[path_to_index(rx_port, tx_layer)] = ta;
  }

  /// Sets the channel estimate for the resource element at the given coordinates.
  void set_ch_estimate(cf_t ce_val, unsigned subcarrier, unsigned symbol, unsigned rx_port = 0, unsigned tx_layer = 0)
  {
    ce[{subcarrier, symbol, rx_port, tx_layer}] = ce_val;
  }
  ///@}

  /// Resizes internal buffers.
  void resize(const channel_estimate_dimensions& dims)
  {
    nof_subcarriers = dims.nof_prb * NRE;
    nof_symbols     = dims.nof_symbols;
    nof_rx_ports    = dims.nof_rx_ports;
    nof_tx_layers   = dims.nof_tx_layers;

    unsigned nof_paths = nof_rx_ports * nof_tx_layers;
    srsran_assert(nof_paths <= MAX_TX_RX_PATHS,
                  "Total requested paths ({}) exceed maximum available ({}).",
                  nof_paths,
                  static_cast<unsigned>(MAX_TX_RX_PATHS));
    noise_variance.resize(nof_paths);
    epre.resize(nof_paths);
    rsrp.resize(nof_paths);
    snr.resize(nof_paths);

    unsigned nof_res = nof_paths * nof_subcarriers * nof_symbols;
    srsran_assert(nof_res <= MAX_BUFFER_SIZE,
                  "Total requested REs ({}) exceed maximum available ({}).",
                  nof_res,
                  static_cast<unsigned>(MAX_BUFFER_SIZE));
    ce.resize({NRE * dims.nof_prb, dims.nof_symbols, dims.nof_rx_ports, dims.nof_tx_layers});
  }

  /// Returns channel estimate dimensions.
  channel_estimate_dimensions size() const
  {
    channel_estimate_dimensions tmp;
    tmp.nof_prb       = nof_subcarriers / NRE;
    tmp.nof_symbols   = nof_symbols;
    tmp.nof_rx_ports  = nof_rx_ports;
    tmp.nof_tx_layers = nof_tx_layers;
    return tmp;
  }

private:
  /// Number of subcarriers considered for channel estimation.
  unsigned nof_subcarriers;

  /// Number of OFDM symbols considered for channel estimation.
  unsigned nof_symbols;

  /// Number of receive antenna ports.
  unsigned nof_rx_ports;

  /// Number of transmission layers.
  unsigned nof_tx_layers;

  /// \name Containers for channel statistics.
  /// The statistics are stored in one-dimensional vectors. Indices vary, first, with the Rx port and, second, with the
  /// Tx layer (e.g., the index corresponding to the second Rx port and to Tx layer 4 is \f$(2-1) + (4-1) *
  /// \mathtt{nof\_rx\_ports}\f$).
  ///@{
  /// Estimated noise variances (linear scale).
  std::vector<float> noise_variance;

  /// Estimated reference signal received powers (linear scale).
  std::vector<float> rsrp;

  /// Average energies per resource element (linear scale).
  std::vector<float> epre;

  /// Estimated signal-to-noise ratios (linear scale).
  std::vector<float> snr;

  /// Estimated time alignment.
  std::vector<phy_time_unit> time_alignment;
  ///@}

  /// \brief Container for channel estimates.
  ///
  /// The channel estimate should be thought as four-dimensional tensor with dimensions representing, in order,
  /// subcarriers, OFDM symbols, receive ports and, finally, transmit layers. However, it is represented as a single
  /// vector, indexed in the same order: i) subcarriers, ii) OFDM symbols, iii) Rx ports, and iv) Tx layers.
  dynamic_tensor<4, cf_t> ce;

  /// Transforms a port-layer pair into a linear index.
  unsigned path_to_index(unsigned rx_port, unsigned tx_layer = 0) const
  {
    srsran_assert(
        rx_port < nof_rx_ports, "Requested Rx antenna port {} is out of bound (max. {})", rx_port, nof_rx_ports);
    srsran_assert(
        tx_layer < nof_tx_layers, "Requested Rx antenna port {} is out of bound (max. {})", rx_port, nof_tx_layers);
    return rx_port + nof_rx_ports * tx_layer;
  }

  /// Transforms a four-dimensional coordinate into a linear index.
  unsigned coords_to_index(unsigned subcarrier, unsigned symbol, unsigned rx_port = 0, unsigned tx_layer = 0) const
  {
    srsran_assert(
        subcarrier < nof_subcarriers, "Requested subcarrier {} is out of bound (max. {})", subcarrier, nof_subcarriers);
    srsran_assert(symbol < nof_symbols, "Requested OFDM symbol {} is out of bound (max. {})", symbol, nof_symbols);
    srsran_assert(
        rx_port < nof_rx_ports, "Requested Rx antenna port {} is out of bound (max. {})", rx_port, nof_rx_ports);
    srsran_assert(tx_layer < nof_tx_layers, "Requested Tx layer {} is out of bound (max. {})", tx_layer, nof_tx_layers);
    return subcarrier + nof_subcarriers * (symbol + nof_symbols * (rx_port + nof_rx_ports * tx_layer));
  }
};

} // namespace srsran
