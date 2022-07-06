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

#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/pusch/pusch_constants.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// \brief Describes channel estimation results.
class channel_estimate
{
private:
  /// Maximum supported number of receive antennas.
  static constexpr unsigned MAX_RX_ANTENNAS = 16;

  /// Maximum supported number of transmission layers.
  static constexpr unsigned MAX_TX_LAYERS = pusch_constants::MAX_NOF_LAYERS;

public:
  struct channel_estimate_dimensions {
    unsigned nof_prb         = 0;
    unsigned nof_symbols     = 0;
    unsigned nof_rx_antennas = 0;
    unsigned nof_tx_layers   = 0;
  };

  /// Estimated noise variance (linear scale).
  std::vector<float> noise_variance;
  /// Estimated noise variance (dB scale).
  std::vector<float> noise_variance_dB;

  /// Reference signal received power (linear scale).
  std::vector<float> rsrp;
  /// Reference signal received power (dB scale).
  std::vector<float> rsrp_dB;

  /// Energy per resource element (linear scale).
  std::vector<float> epre;
  /// Energy per resource element (dB scale).
  std::vector<float> epre_dB;

  /// Signal-to-noise ratio (linear scale).
  std::vector<float> snr;
  /// Signal-to-noise ratio (dB scale).
  std::vector<float> snr_dB;

  /// Dynamic array dedicated for storing channel estimates.
  std::vector<cf_t> ce;

  /// Construct given number of RBs used by reference signal.
  explicit channel_estimate(const channel_estimate_dimensions& dims)
  {
    srsran_assert(dims.nof_prb <= MAX_RB, "Requested {} RBs, but at most {} are allowed.", dims.nof_prb, MAX_RB);
    srsran_assert(dims.nof_symbols <= MAX_NSYMB_PER_SLOT,
                  "Requested {} OFDM symbols, but at most {} are allowed.",
                  dims.nof_symbols,
                  MAX_NSYMB_PER_SLOT);
    srsran_assert(dims.nof_rx_antennas <= MAX_RX_ANTENNAS,
                  "Requested {} receive antennas, but at most {} are supported.",
                  dims.nof_rx_antennas,
                  16);
    // todo(david): if I write MAX_RX_ANTENNAS instead of 16 I get a compile error from "fmt/core.h:1885: undefined
    // reference to `srsgnb::channel_estimate::MAX_TX_LAYERS'
    // same thing with 4 in the next instruction.
    srsran_assert(dims.nof_tx_layers <= MAX_TX_LAYERS,
                  "Requested {} transmission layers, but at most {} are supported.",
                  dims.nof_tx_layers,
                  4);

    unsigned slot_len_re = dims.nof_prb * NRE * dims.nof_symbols;
    unsigned nof_paths   = dims.nof_tx_layers * dims.nof_rx_antennas;

    // reserve memory for channel estimates and initialize with 1.0
    ce.resize(slot_len_re * nof_paths, 1.0);

    noise_variance.resize(nof_paths);
    noise_variance_dB.resize(nof_paths);
    rsrp.resize(nof_paths);
    rsrp_dB.resize(nof_paths);
    epre.resize(nof_paths);
    epre_dB.resize(nof_paths);
    snr.resize(nof_paths);
    snr_dB.resize(nof_paths);
  }

  /// Default destructor
  ~channel_estimate() = default;
};

} // namespace srsgnb
