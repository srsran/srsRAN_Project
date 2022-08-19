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
#include "srsgnb/adt/tensor.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/channel_state_information.h"
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

  /// Default constructor: reserves internal memory.
  channel_estimate() : nof_subcarriers(0), nof_symbols(0), nof_rx_ports(0), nof_tx_layers(0)
  {
    ce.reserve({MAX_RB * NRE, MAX_NSYMB_PER_SLOT, MAX_RX_PORTS, MAX_TX_LAYERS});
    noise_variance.reserve(MAX_TX_RX_PATHS);
    epre.reserve(MAX_TX_RX_PATHS);
    rsrp.reserve(MAX_TX_RX_PATHS);
    snr.reserve(MAX_TX_RX_PATHS);
  }

  /// Constructor: sets the size of the internal buffers.
  explicit channel_estimate(const channel_estimate_dimensions& dims) :
    nof_subcarriers(dims.nof_prb * NRE),
    nof_symbols(dims.nof_symbols),
    nof_rx_ports(dims.nof_rx_ports),
    nof_tx_layers(dims.nof_tx_layers)
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
    return ce.get_view<2>({rx_port, tx_layer});
  }

  /// \brief Returns a read-only view to the RE channel estimates  of the path between the given Rx port and Tx layer.
  ///
  /// The view is represented as a vector indexed by i) subcarriers and ii) OFDM symbols.
  span<const cf_t> get_path_ch_estimate(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return ce.get_view<2>({rx_port, tx_layer});
  }

  /// Returns the general Channel State Information.
  channel_state_information get_channel_state_information() const
  {
    channel_state_information csi = {};
    return csi;
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
    srsgnb_assert(nof_paths <= MAX_TX_RX_PATHS,
                  "Total requested paths ({}) exceed maximum available ({}).",
                  nof_paths,
                  static_cast<unsigned>(MAX_TX_RX_PATHS));
    noise_variance.resize(nof_paths);
    epre.resize(nof_paths);
    rsrp.resize(nof_paths);
    snr.resize(nof_paths);

    unsigned nof_res = nof_paths * nof_subcarriers * nof_symbols;
    srsgnb_assert(nof_res <= MAX_BUFFER_SIZE,
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
    srsgnb_assert(
        rx_port < nof_rx_ports, "Requested Rx antenna port {} is out of bound (max. {})", rx_port, nof_rx_ports);
    srsgnb_assert(
        tx_layer < nof_tx_layers, "Requested Rx antenna port {} is out of bound (max. {})", rx_port, nof_tx_layers);
    return rx_port + nof_rx_ports * tx_layer;
  }

  /// Transforms a four-dimensional coordinate into a linear index.
  unsigned coords_to_index(unsigned subcarrier, unsigned symbol, unsigned rx_port = 0, unsigned tx_layer = 0) const
  {
    srsgnb_assert(
        subcarrier < nof_subcarriers, "Requested subcarrier {} is out of bound (max. {})", subcarrier, nof_subcarriers);
    srsgnb_assert(symbol < nof_symbols, "Requested OFDM symbol {} is out of bound (max. {})", symbol, nof_symbols);
    srsgnb_assert(
        rx_port < nof_rx_ports, "Requested Rx antenna port {} is out of bound (max. {})", rx_port, nof_rx_ports);
    srsgnb_assert(tx_layer < nof_tx_layers, "Requested Tx layer {} is out of bound (max. {})", tx_layer, nof_tx_layers);
    return subcarrier + nof_subcarriers * (symbol + nof_symbols * (rx_port + nof_rx_ports * tx_layer));
  }
};

/// DM-RS symbols.
class dmrs_symbol_list
{
public:
  /// \brief Resizes the DM-RS symbol list.
  /// \param[in] nof_subc    Number of resource elements per symbol.
  /// \param[in] nof_symbols Number of OFDM symbols.
  /// \param[in] nof_layers  Number of transmission layers.
  void resize(unsigned nof_subc, unsigned nof_symbols, unsigned nof_layers)
  {
    srsgnb_assert(nof_subc <= pusch_constants::MAX_NOF_DMRS_SUBC,
                  "The number of subcarriers {} exceeds the maximum {}.",
                  nof_symbols,
                  pusch_constants::MAX_NOF_DMRS_SUBC);
    srsgnb_assert(nof_symbols <= pusch_constants::MAX_NOF_DMRS_SYMBOLS,
                  "The number of OFDM symbols {} exceeds the maximum {}.",
                  nof_symbols,
                  pusch_constants::MAX_NOF_DMRS_SYMBOLS);
    srsgnb_assert(nof_layers <= pusch_constants::MAX_NOF_LAYERS,
                  "The number of layers {} exceeds the maximum {}.",
                  nof_layers,
                  pusch_constants::MAX_NOF_LAYERS);
    // Resize data.
    data.resize({nof_subc, nof_symbols, nof_layers});
  }

  /// Gets the current number of subcarriers.
  unsigned get_nof_subc() const { return data.get_dimensions_size()[0]; }

  /// Gets the current number of symbols.
  unsigned get_nof_symbols() const { return data.get_dimensions_size()[1]; }

  /// Gets the current number of layers.
  unsigned get_nof_layers() const { return data.get_dimensions_size()[2]; }

  /// Gets a read-only view of the subcarriers for a certain symbol and layer.
  span<const cf_t> get_subc(unsigned symbol, unsigned layer) const { return data.get_view<1>({symbol, layer}); }

  /// Gets a view of the subcarriers for a certain symbol and layer.
  span<cf_t> get_subc(unsigned symbol, unsigned layer) { return data.get_view<1>({symbol, layer}); }

  /// Gets a read-only view of the symbols for a certain layer.
  span<const cf_t> get_symbols(unsigned layer) const { return data.get_view<2>({layer}); }

  /// Gets a view of the symbols for a certain layer.
  span<cf_t> get_symbols(unsigned layer) { return data.get_view<2>({layer}); }

private:
  /// Maximum number of DM-RS symbols per slot.
  static constexpr unsigned MAX_NOF_DMRS_PER_SLOT =
      pusch_constants::MAX_NOF_DMRS_SUBC * pusch_constants::MAX_NOF_DMRS_SYMBOLS * pusch_constants::MAX_NOF_LAYERS;

  /// \brief Container for symbols.
  ///
  /// The symbols should be thought as three-dimensional tensor with dimensions representing, in order,
  /// subcarriers, OFDM symbols, and, finally, transmit layers. However, it is represented as a single
  /// vector, indexed in the same order: i) subcarriers, ii) OFDM symbols, iii) Tx layers.
  static_tensor<3, cf_t, MAX_NOF_DMRS_PER_SLOT> data;
};

/// \brief DM-RS pattern to specify the position of DM-RS symbols in the resource grid.
///
/// DM-RS symbols are transmitted with a specific subcarrier pattern that repeats at specific OFDM symbols within the
/// slot. Therefore, the position of the DM-RS symbols can be univocally characterized by a time mask and a frequency
/// mask: Resource element \f$(k, l)\f$ contains a DM-RS symbol only if element \f$k\f$ of the frequency mask and
/// element \f$l\f$ of the time mask are set to true.
struct dmrs_pattern {
  /// Boolean mask to specify the OFDM symbols carrying DM-RS symbols.
  std::array<bool, MAX_NSYMB_PER_SLOT> symbols;
  /// Boolean mask to specify the resource blocks carrying DM-RS symbols.
  bounded_bitset<MAX_RB> rb_mask;
  /// Boolean mask to specify the resource elements within the resource blocks carrying DM-RS symbols.
  std::array<bool, NRE> re_pattern;
};
} // namespace srsgnb
