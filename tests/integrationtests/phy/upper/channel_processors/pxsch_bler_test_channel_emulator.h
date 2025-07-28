/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/math/complex_normal_random.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"
#include <random>
#include <string>

namespace srsran {

/// \brief Implements a frequency domain channel emulator.
///
/// This channel emulator works in the frequency domain. It applies a fading channel model without doppler dispersion.
/// The fading profile is selected upon creation and the tap coefficients are randomly generated in a uncorrelated
/// fashion upon every call to run().
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
  /// \param[in] delay_profile               Delay profile - The valid profiles are TDLA, TDLB, TDLC, and single-tap.
  /// \param[in] fading_distribution         Fading distribution - The valid distributions are rayleigh and
  ///                                        uniform-phase.
  /// \param[in] sinr_dB                     Signal-to-Interference-plus-Noise Ratio.
  /// \param[in] cfo_Hz                      Carrier frequency offset in Herz.
  /// \param[in] nof_corrupted_re_per_symbol Number of corrupted RE per OFDM symbol. Set to zero for no corrupted RE.
  /// \param[in] nof_tx_ports                Number of transmit ports.
  /// \param[in] nof_rx_ports                Number of receive ports.
  /// \param[in] nof_subc                    Number of resource grid subcarriers.
  /// \param[in] nof_symbols                 Number of OFDM symbols per slot.
  /// \param[in] max_nof_threads             Maximum number of threads used by the asynchronous executor.
  /// \param[in] scs                         Resource grid subcarrier spacing.
  /// \param[in] executor_                   Asynchronous task execution.
  channel_emulator(std::string        delay_profile,
                   std::string        fading_distribution,
                   float              sinr_dB,
                   float              cfo_Hz,
                   unsigned           nof_corrupted_re_per_symbol,
                   unsigned           nof_tx_ports,
                   unsigned           nof_rx_ports,
                   unsigned           nof_subc,
                   unsigned           nof_symbols,
                   unsigned           max_nof_threads,
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
    concurrent_channel_emulator(float sinr_dB, unsigned nof_corrupted_re_, unsigned nof_subc) :
      rgen(seed++),
      dist_awgn(cf_t(), convert_dB_to_amplitude(-sinr_dB)),
      dist_corrupted_re(0, nof_subc - 1),
      nof_corrupted_re(nof_corrupted_re_),
      temp_ofdm_symbol(nof_subc),
      temp_single_ofdm_symbol(nof_subc)
    {
    }

    /// \brief Runs the channel emulator on a receive port and OFDM symbol index for the given resource grids.
    /// \param[out] rx_grid       Received resource grid.
    /// \param[in]  tx_grid       Transmit resource grid.
    /// \param[in]  freq_response Frequency domain response.
    /// \param[in]  time_coeff    Time domain coefficient.
    /// \param[in]  i_port        Receive port index.
    /// \param[in]  i_symbol      OFDM symbol index within the slot.
    void run(resource_grid_writer&       rx_grid,
             const resource_grid_reader& tx_grid,
             const tensor<3, cf_t>&      freq_response,
             cf_t                        time_coeff,
             unsigned                    i_port,
             unsigned                    i_symbol);

  private:
    /// Seed for the random generator (each instance gets a different initialization).
    static unsigned seed;
    /// Random generator.
    std::mt19937 rgen;
    /// Additive white gaussian noise distribution.
    complex_normal_distribution<cf_t> dist_awgn;
    /// Distribution for simulating corrupted RE.
    std::uniform_int_distribution<unsigned> dist_corrupted_re;
    /// Number of corrupted RE per OFDM symbol.
    unsigned nof_corrupted_re;
    /// Temporary OFDM frequency domain symbol.
    std::vector<cf_t> temp_ofdm_symbol;
    /// Temporary single OFDM frequency domain symbol.
    std::vector<cf_t> temp_single_ofdm_symbol;
  };

  enum { invalid_distribution, rayleigh, uniform_phase, butler } fading_distribution = invalid_distribution;
  /// Number of OFDM symbols per slot.
  unsigned nof_ofdm_symbols;
  /// Random generator for taps phase and power.
  std::mt19937 rgen;
  /// Random distribution for uncorrelated tap weights.
  complex_normal_distribution<cf_t> dist_rayleigh;
  /// Uniform real distribution for phase.
  std::uniform_real_distribution<float> dist_uniform_phase = std::uniform_real_distribution<float>(-M_PI, M_PI);
  /// CFO coefficients.
  std::array<cf_t, MAX_NSYMB_PER_SLOT> cfo_coeffs;
  /// Temporary channel sum.
  dynamic_tensor<3, cf_t> freq_domain_channel;
  /// Temporary channel.
  std::vector<cf_t> temp_channel;
  /// Frequency response of each of the channel taps.
  dynamic_tensor<2, cf_t> taps_channel_response;
  /// Concurrent channel emulators.
  bounded_object_pool<concurrent_channel_emulator> emulators;
  /// Asynchronous executor.
  task_executor& executor;
};

} // namespace srsran
