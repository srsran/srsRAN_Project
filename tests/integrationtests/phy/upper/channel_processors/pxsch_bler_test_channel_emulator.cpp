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

#include "pxsch_bler_test_channel_emulator.h"
#include "srsran/adt/span.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <thread>

using namespace srsran;

unsigned channel_emulator::concurrent_channel_emulator::seed = 0;

/// \brief TDP channel tap.
///
/// The first element is the tap delay in nanoseconds, and the second element is the tap amplitude in dB.
using tdl_tap = std::pair<unsigned, float>;

/// Single-tap profile.
static constexpr std::array<tdl_tap, 1> taps_single = {{{0, 0.0}}};

/// TDLA fading profile.
static constexpr std::array<tdl_tap, 12> taps_tdla = {{{0, -15.5},
                                                       {10, 0.0},
                                                       {15, -5.1},
                                                       {20, -5.1},
                                                       {25, -9.6},
                                                       {50, -8.2},
                                                       {65, -13.1},
                                                       {75, -11.5},
                                                       {105, -11.0},
                                                       {135, -16.2},
                                                       {150, -16.6},
                                                       {290, -26.2}}};

/// TDLB fading profile.
static constexpr std::array<tdl_tap, 12> taps_tdlb = {{{0, 0},
                                                       {10, -2.2},
                                                       {20, -0.6},
                                                       {30, -0.6},
                                                       {35, -0.3},
                                                       {45, -1.2},
                                                       {55, -5.9},
                                                       {120, -2.2},
                                                       {170, -0.8},
                                                       {245, -6.3},
                                                       {330, -7.5},
                                                       {480, -7.1}}};

/// TDLC fading profile.
static constexpr std::array<tdl_tap, 12> taps_tdlc = {{{0, -6.9},
                                                       {65, 0},
                                                       {70, -7.7},
                                                       {190, -2.5},
                                                       {195, -2.4},
                                                       {200, -9.9},
                                                       {240, -8.0},
                                                       {325, -6.6},
                                                       {520, -7.1},
                                                       {1045, -13.0},
                                                       {1510, -14.2},
                                                       {2595, -16.0}}};

channel_emulator::channel_emulator(std::string        delay_profile,
                                   std::string        fading_distribution_,
                                   float              sinr_dB,
                                   float              cfo_Hz,
                                   unsigned           nof_corrupted_re_per_symbol,
                                   unsigned           nof_tx_ports,
                                   unsigned           nof_rx_ports,
                                   unsigned           nof_subc,
                                   unsigned           nof_symbols,
                                   unsigned           max_nof_threads,
                                   subcarrier_spacing scs,
                                   task_executor&     executor_) :
  nof_ofdm_symbols(nof_symbols),
  freq_domain_channel({nof_subc, nof_rx_ports, nof_tx_ports}),
  temp_channel(nof_subc),
  emulators(max_nof_threads, sinr_dB, nof_corrupted_re_per_symbol, nof_subc),
  executor(executor_)
{
  // Select fading channel taps.
  span<const tdl_tap> taps;
  if (delay_profile == "single-tap") {
    taps = taps_single;
  } else if (delay_profile == "TDLA") {
    taps = taps_tdla;
  } else if (delay_profile == "TDLB") {
    taps = taps_tdlb;
  } else if (delay_profile == "TDLC") {
    taps = taps_tdlc;
  }
  report_fatal_error_if_not(!taps.empty(),
                            "Invalid delay profile '{}'. Valid delay profiles are single-tap, TDLA, TDLB, and TDLC.",
                            delay_profile);

  if (fading_distribution_ == "rayleigh") {
    fading_distribution = rayleigh;
  } else if (fading_distribution_ == "uniform-phase") {
    fading_distribution = uniform_phase;
  } else if (fading_distribution_ == "butler") {
    fading_distribution = butler;
  }
  report_fatal_error_if_not(
      fading_distribution != invalid_distribution,
      "Invalid fading distribution '{}'. Valid distributions are rayleigh, uniform-phase, and butler.",
      fading_distribution_);

  // Estimate taps power.
  float taps_power = std::accumulate(taps.begin(), taps.end(), 0.0F, [](float acc, const tdl_tap& tap) {
    return acc + convert_dB_to_power(tap.second);
  });

  // Calculate power normalization coefficient.
  float norm_coefficient = 1.0F / std::sqrt(nof_rx_ports * taps_power);

  // Generate frequency response of each tap.
  taps_channel_response.resize({nof_subc, static_cast<unsigned>(taps.size())});
  for (unsigned i_tap = 0; i_tap != taps.size(); ++i_tap) {
    // Extract tap delay in seconds.
    float delay_s = taps[i_tap].first * 1e-9;

    // Extract tap linear average amplitude.
    float amplitude = norm_coefficient * convert_dB_to_amplitude(taps[i_tap].second);

    // Calculate the subcarrier phase shift.
    float subcarrier_phase_shift = delay_s * static_cast<float>(scs_to_khz(scs) * 1000);

    // Generate frequency response for the tap.
    span<cf_t> freq_response_tap = taps_channel_response.get_view({i_tap});
    std::generate(
        freq_response_tap.begin(), freq_response_tap.end(), [n = 0, amplitude, subcarrier_phase_shift]() mutable {
          return std::polar(amplitude, -TWOPI * static_cast<float>(++n) * subcarrier_phase_shift);
        });
  }

  // Generate CFO coefficients.
  double symbol_duration_s   = 1.0 / (1000.0 * scs_to_khz(scs));
  double symbol_start_time_s = 0.0;
  for (unsigned i_symb = 0; i_symb != MAX_NSYMB_PER_SLOT; ++i_symb) {
    // Calculate cyclic prefix duration and add it to the start time.
    symbol_start_time_s += cyclic_prefix(cyclic_prefix::NORMAL).get_length(i_symb, scs).to_seconds();

    // Calculate CFO coefficient for the OFDM symbol.
    cfo_coeffs[i_symb] = std::polar(1.0, TWOPI * symbol_start_time_s * cfo_Hz);

    // Advance symbol duration.
    symbol_start_time_s += symbol_duration_s;
  }
}

void channel_emulator::run(resource_grid_writer& rx_grid, const resource_grid_reader& tx_grid)
{
  unsigned nof_rx_ports = freq_domain_channel.get_dimension_size(1);
  unsigned nof_tx_ports = freq_domain_channel.get_dimension_size(2);
  unsigned nof_taps     = taps_channel_response.get_dimension_size(1);

  // Channel emulator.
  std::atomic<unsigned> count = {0}, completed = {0};
  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    // Generate frequency domain channel response for each transmit port.
    for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
      // Get view of the frequency domain response for this port.
      span<cf_t> chan_freq_respone = freq_domain_channel.get_view({i_rx_port, i_tx_port});

      // Generate frequency domain response for the entire slot.
      for (unsigned i_tap = 0; i_tap != nof_taps; ++i_tap) {
        // Select intermediate tap frequency domain buffer. Use final buffer for the first tap and skip accumulation.
        span<cf_t> tap_channel = temp_channel;
        if (i_tap == 0) {
          tap_channel = chan_freq_respone;
        }

        // Select tap from the fading distribution.
        cf_t tap;
        switch (fading_distribution) {
          case rayleigh:
            tap = dist_rayleigh(rgen);
            break;
          case uniform_phase:
            tap = std::polar(1.0F, dist_uniform_phase(rgen));
            break;
          case butler:
            tap = std::polar(1.0F, static_cast<float>(2.0F * M_PI * i_rx_port * i_tx_port / nof_tx_ports));
            break;
          case invalid_distribution:
            tap = std::numeric_limits<float>::quiet_NaN();
            break;
        }

        // Multiply tap frequency response by a fading distribution tap.
        srsvec::sc_prod(tap_channel, taps_channel_response.get_view({i_tap}), tap);

        // Accumulate tap frequency response. Bypass accumulation for the first tap.
        if (i_tap != 0) {
          srsvec::add(chan_freq_respone, tap_channel, chan_freq_respone);
        }
      }
    }

    // Run channel for each symbol with the same frequency response.
    for (unsigned i_symbol = 0; i_symbol != nof_ofdm_symbols; ++i_symbol) {
      bool success = executor.execute([this, &rx_grid, &tx_grid, i_rx_port, i_symbol, &completed]() {
        auto emulator = emulators.get();
        report_fatal_error_if_not(emulator, "Failed to retrieve channel emulator.");
        emulator->run(rx_grid, tx_grid, freq_domain_channel, cfo_coeffs[i_symbol], i_rx_port, i_symbol);
        ++completed;
      });
      report_fatal_error_if_not(success, "Failed to enqueue concurrent channel emulate.");
      ++count;
    }
  }

  // Wait for channel emulator to finish.
  while (completed != count) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void channel_emulator::concurrent_channel_emulator::run(resource_grid_writer&       rx_grid,
                                                        const resource_grid_reader& tx_grid,
                                                        const tensor<3, cf_t>&      freq_response,
                                                        cf_t                        time_coeff,
                                                        unsigned                    i_rx_port,
                                                        unsigned                    i_symbol)
{
  using namespace std::complex_literals;
  unsigned nof_tx_ports = freq_response.get_dimension_size(2);

  // For each transmit port.
  for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
    // Select temporary buffer.
    span<cf_t> single_ofdm_symbol = temp_single_ofdm_symbol;
    if (i_tx_port == 0) {
      single_ofdm_symbol = temp_ofdm_symbol;
    }

    // Get OFDM symbol.
    tx_grid.get(single_ofdm_symbol, i_tx_port, i_symbol, 0);

    // Apply frequency domain fading channel.
    srsvec::prod(single_ofdm_symbol, freq_response.get_view({i_rx_port, i_tx_port}), single_ofdm_symbol);

    // Skip accumulating for the first transmit port.
    if (i_tx_port != 0) {
      srsvec::add(temp_ofdm_symbol, single_ofdm_symbol, temp_ofdm_symbol);
    }
  }

  // Apply time-domain coefficient.
  srsvec::sc_prod(temp_ofdm_symbol, temp_ofdm_symbol, time_coeff);

  // Apply AWGN.
  std::generate(temp_single_ofdm_symbol.begin(), temp_single_ofdm_symbol.end(), [this]() { return dist_awgn(rgen); });
  srsvec::add(temp_ofdm_symbol, temp_single_ofdm_symbol, temp_ofdm_symbol);

  // Corrupt REs.
  std::set<unsigned> corrupted_i_subc;
  for (unsigned i = 0; i != nof_corrupted_re; ++i) {
    // Select a subcarrier that has not been corruped before.
    unsigned i_subc;
    do {
      i_subc = dist_corrupted_re(rgen);
    } while (corrupted_i_subc.count(i_subc) != 0);
    corrupted_i_subc.emplace(i_subc);

    temp_ofdm_symbol[i_subc] *= 1i;
  }

  // Write the OFDM symbol back to the grid.
  rx_grid.put(i_rx_port, i_symbol, 0, temp_ofdm_symbol);
}
