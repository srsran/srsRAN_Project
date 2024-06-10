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
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/complex_normal_random.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"
#include <random>
#include <string>

namespace srsran {

/// \brief Implements a frequency domain channel emulator.
///
/// This channel emulator works in frequency domain. It applies a fading channel model without doppler dispersion. The
/// fading profile is selected upon creation and the tap coefficients are randomly generated in a uncorrelated fashion
/// upon every call to run().
///
/// The fading channel run() is not thread safe. It deterministically selects the tap coefficients. However, the noise
/// addition is performed asynchronously, which can lead to different results across executions.
///
/// The channel response is only inspired by the 3GPP TDL profiles, not an accurate implementation thereof.
class channel_emulator
{
public:
  /// \brief Creates a channel emulator.
  ///
  /// \param[in] channel         Channel fading profile. The valid profiles are TDLA, TDLB and TDLC.
  /// \param[in] sinr_dB         Signal-to-Interference-plus-Noise Ratio.
  /// \param[in] nof_rx_ports    Number of receive ports.
  /// \param[in] nof_subc        Number of the resource grid subcarriers.
  /// \param[in] nof_symbols     Number of OFDM symbols per slot.
  /// \param[in] scs             Resource grid subcarrier spacing.
  /// \param[in] executor_       Asynchronous task execution.
  channel_emulator(std::string        channel,
                   float              sinr_dB,
                   unsigned           nof_rx_ports,
                   unsigned           nof_subc,
                   unsigned           nof_symbols,
                   subcarrier_spacing scs,
                   task_executor&     executor_);

  /// \brief Runs the channel emulator.
  ///
  /// \param[out] rx_grid Receive resource grid.
  /// \param[in]  tx_grid Transmit resource grid.
  void run(resource_grid_writer& rx_grid, const resource_grid_reader& tx_grid);

private:
  /// Internal concurrent channel emulator.
  class concurrent_channel_emulator
  {
  public:
    concurrent_channel_emulator(float sinr_dB, unsigned nof_subc) :
      rgen(seed++),
      dist_awgn(cf_t(), convert_dB_to_amplitude(-sinr_dB)),
      temp_ofdm_symbol(nof_subc),
      temp_awgn(nof_subc)
    {
    }

    /// \brief Runs the channel emulator on a receive port and OFDM symbol index for the given resource grids.
    /// \param[out] rx_grid       Received resource grid.
    /// \param[in]  tx_grid       Transmit resource grid.
    /// \param[in]  freq_response Frequency domain response.
    /// \param[in]  i_port        Receive port index.
    /// \param[in]  i_symbol      OFDM symbol index within the slot.
    void run(resource_grid_writer&       rx_grid,
             const resource_grid_reader& tx_grid,
             span<const cf_t>            freq_response,
             unsigned                    i_port,
             unsigned                    i_symbol);

  private:
    /// Seed for the random generator (each instance gets a different initialization).
    static unsigned seed;
    /// Random generator.
    std::mt19937 rgen;
    /// Additive white gaussian noise distribution.
    complex_normal_distribution<cf_t> dist_awgn;
    /// Temporary OFDM frequency domain symbol.
    std::vector<cf_t> temp_ofdm_symbol;
    /// Temporary generated noise for adding to an OFDM symbol.
    std::vector<cf_t> temp_awgn;
  };

  /// Number of OFDM symbols per slot.
  unsigned nof_ofdm_symbols;
  /// Random generator for taps phase and power.
  std::mt19937 rgen;
  /// Random distribution for uncorrelated tap weights.
  complex_normal_distribution<cf_t> dist_taps;
  /// Temporary channel sum.
  dynamic_tensor<2, cf_t> freq_domain_channel;
  /// Temporary channel.
  std::vector<cf_t> temp_channel;
  /// Frequency response of each of the channel taps.
  dynamic_tensor<2, cf_t> taps_channel_response;
  /// Concurrent channel emulators.
  concurrent_thread_local_object_pool<concurrent_channel_emulator> emulators;
  /// Asynchronous executor.
  task_executor& executor;
};

} // namespace srsran