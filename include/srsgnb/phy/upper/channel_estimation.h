/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// Channel estimation-related declarations.

#pragma once

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/pusch/pusch_constants.h"
#include "srsgnb/support/srsgnb_assert.h"

namespace srsgnb {

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

  /// Constructor: sets the size of the internal buffers.
  explicit channel_estimate(const channel_estimate_dimensions& dims) :
    nof_subcarriers(dims.nof_prb * NRE), nof_symbols(dims.nof_symbols), nof_rx_ports(dims.nof_rx_ports)
  {
    srsgnb_assert(dims.nof_prb <= MAX_RB, "Requested {} RBs, but at most {} are allowed.", dims.nof_prb, MAX_RB);
    srsgnb_assert(dims.nof_symbols <= MAX_NSYMB_PER_SLOT,
                  "Requested {} OFDM symbols, but at most {} are allowed.",
                  dims.nof_symbols,
                  MAX_NSYMB_PER_SLOT);
    srsgnb_assert(dims.nof_rx_ports <= MAX_RX_PORTS,
                  "Requested {} receive ports, but at most {} are supported.",
                  dims.nof_rx_ports,
                  static_cast<unsigned>(MAX_RX_PORTS));
    srsgnb_assert(dims.nof_tx_layers <= MAX_TX_LAYERS,
                  "Requested {} transmission layers, but at most {} are supported.",
                  dims.nof_tx_layers,
                  static_cast<unsigned>(MAX_TX_LAYERS));

    unsigned slot_len_re = dims.nof_prb * NRE * dims.nof_symbols;
    unsigned nof_paths   = dims.nof_tx_layers * dims.nof_rx_ports;

    // Reserve memory for channel estimates and initialize with 1.0.
    ce.reserve(MAX_BUFFER_SIZE);
    ce.resize(slot_len_re * nof_paths, 1.0F);

    // Reserve memory for the rest of channel statistics.
    noise_variance.reserve(MAX_TX_RX_PATHS);
    noise_variance.resize(nof_paths);
    epre.reserve(MAX_TX_RX_PATHS);
    epre.resize(nof_paths);
    rsrp.reserve(MAX_TX_RX_PATHS);
    rsrp.resize(nof_paths);
    snr.reserve(MAX_TX_RX_PATHS);
    snr.resize(nof_paths);
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

  /// Returns the average EPRE for the path between the given Rx port and Tx layer (linear scale).
  float get_epre(unsigned rx_port, unsigned tx_layer = 0) const { return epre[path_to_index(rx_port, tx_layer)]; }

  /// Returns the average EPRE for the path between the given Rx port and Tx layer (dB scale).
  float get_epre_dB(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return convert_power_to_dB(get_epre(rx_port, tx_layer));
  }

  /// Returns the estimated SNR for the path between the given Rx port and Tx layer (linear scale).
  float get_snr(unsigned rx_port, unsigned tx_layer = 0) const { return snr[path_to_index(rx_port, tx_layer)]; }

  /// Returns the estimated SNR for the path between the given Rx port and Tx layer (dB scale).
  float get_snr_dB(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return convert_power_to_dB(get_snr(rx_port, tx_layer));
  }

  /// \brief Returns a read-write view to the RE channel estimates of the path between the given Rx port and Tx layer.
  ///
  /// The view is represented as a vector indexed by i) subcarriers and ii) OFDM symbols.
  span<cf_t> get_path_ch_estimate(unsigned rx_port, unsigned tx_layer = 0)
  {
    unsigned view_size  = nof_subcarriers * nof_symbols;
    unsigned view_start = view_size * (rx_port + nof_rx_ports * tx_layer);
    return span<cf_t>(ce).subspan(view_start, view_size);
  }

  /// \brief Returns a read-only view to the RE channel estimates  of the path between the given Rx port and Tx layer.
  ///
  /// The view is represented as a vector indexed by i) subcarriers and ii) OFDM symbols.
  span<const cf_t> get_path_ch_estimate(unsigned rx_port, unsigned tx_layer = 0) const
  {
    unsigned view_size  = nof_subcarriers * nof_symbols;
    unsigned view_start = view_size * (rx_port + nof_rx_ports * tx_layer);
    return span<const cf_t>(ce).subspan(view_start, view_size);
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

  /// Sets the channel estimate for the resource element at the given coordinates.
  void set_ch_estimate(float ce_val, unsigned subcarrier, unsigned symbol, unsigned rx_port = 0, unsigned tx_layer = 0)
  {
    unsigned index = coords_to_index(subcarrier, symbol, rx_port, tx_layer);
    ce[index]      = ce_val;
  }
  ///@}

  void resize(const channel_estimate_dimensions& dims)
  {
    nof_subcarriers = dims.nof_prb * NRE;
    nof_symbols     = dims.nof_symbols;
    nof_rx_ports    = dims.nof_rx_ports;

    unsigned nof_paths = dims.nof_rx_ports * dims.nof_tx_layers;
    srsgnb_assert(nof_paths <= MAX_TX_RX_PATHS,
                  "Total requested paths ({}) exceed maximum available ({}).",
                  nof_paths,
                  MAX_TX_RX_PATHS);
    noise_variance.resize(nof_paths);
    epre.resize(nof_paths);
    rsrp.resize(nof_paths);
    snr.resize(nof_paths);

    unsigned nof_res = nof_paths * nof_subcarriers * nof_symbols;
    srsgnb_assert(nof_res <= MAX_BUFFER_SIZE,
                  "Total requested REs ({}) exceed maximum available ({}).",
                  nof_res,
                  MAX_BUFFER_SIZE);
    ce.resize(nof_res);
  }

private:
  /// Number of subcarriers considered for channel estimation.
  unsigned nof_subcarriers;

  /// Number of OFDM symbols considered for channel estimation.
  unsigned nof_symbols;

  /// Number of receive ports.
  unsigned nof_rx_ports;

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
  ///@}

  /// \brief Container for channel estimates.
  ///
  /// The channel estimate should be thought as four-dimensional tensor with dimensions representing, in order,
  /// subcarriers, OFDM symbols, receive ports and, finally, transmit layers. However, it is represented as a single
  /// vector, indexed in the same order: i) subcarriers, ii) OFDM symbols, iii) Rx ports, and iv) Tx layers.
  std::vector<cf_t> ce;

  /// Transforms a port-layer pair into a linear index.
  unsigned path_to_index(unsigned rx_port, unsigned tx_layer = 0) const { return rx_port + nof_rx_ports * tx_layer; }

  /// Transforms a four-dimensional coordinate into a linear index.
  unsigned coords_to_index(unsigned subcarrier, unsigned symbol, unsigned rx_port = 0, unsigned tx_layer = 0) const
  {
    return subcarrier + nof_subcarriers * (symbol + nof_symbols * (rx_port + nof_rx_ports * tx_layer));
  }
};

/// \brief Maximum number of DM-RS symbols for one layer.
///
/// At most half of the total number of subcarriers (i.e., <tt>MAX_RB * NRE / 2</tt>) is assigned a DM-RS symbol.
/// Moreover, the number of OFDM symbols carrying DM-RS in a slot is at most \f$4 \times 2\f$, being 4 the maximum
/// number of positions \f$\bar{l}\f$ and 2 the maximum number of indices \f$l'\f$, as per TS38.211 Section 6.4.1.1.
static constexpr unsigned MAX_NOF_LAYER_DMRS_SYMBOLS = MAX_RB * NRE / 2 * 4 * 2;

/// Container for DM-RS symbols.
using dmrs_symbol_list = static_vector<cf_t, MAX_NOF_LAYER_DMRS_SYMBOLS>;

/// \brief Boolean mask to specify the position of DM-RS symbols in the resource grid.
///
/// DM-RS symbols are transmitted with a specific subcarrier pattern that repeats at specific OFDM symbols within the
/// slot. Therefore, the position of the DM-RS symbols can be univocally characterized by a time mask and a frequency
/// mask: Resource element \f$(k, l)\f$ contains a DM-RS symbol only if element \f$k\f$ of the frequency mask and
/// element \f$l\f$ of the time mask are set to true.
struct dmrs_mask {
  /// Boolean mask to specify the OFDM symbols carrying DM-RS symbols.
  static_vector<bool, MAX_NSYMB_PER_SLOT> symbols;
  /// Boolean mask to specify the subcarriers carrying DM-RS symbols.
  bounded_bitset<MAX_RB * NRE / 2> res_elements;
};
} // namespace srsgnb
