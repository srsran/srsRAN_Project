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

#include "pucch_detector_format0.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/dot_prod.h"
#include <algorithm>
#include <complex>

using namespace srsran;

namespace {

/// \brief PUCCH Format 0 dictionary entry.
///
/// Pairs a cyclic shift with a corresponding UCI message.
struct pucch_detector_format0_entry {
  /// Cyclic shift, parameter \f$m_{cs}\f$.
  unsigned m_cs;
  /// Message corresponding to the received cyclic shift.
  pucch_uci_message message;
};

} // namespace

/// Table for positive SR only. Defined in TS38.213 Section 9.2.4.
static const std::array<pucch_detector_format0_entry, 1> pucch_detector_format0_noharq_sr = {{{0, {{1}, {}, {}, {}}}}};
/// Table for one HARQ-ACK feedback bit. Defined in TS38.213 Section 9.2.4 Table 9.2.3-3.
static const std::array<pucch_detector_format0_entry, 2> pucch_detector_format0_oneharq_nosr = {
    {{0, {{}, {0}, {}, {}}}, {6, {{}, {1}, {}, {}}}}};
/// Table for two HARQ-ACK feedback bit. Defined in TS38.213 Section 9.2.4 Table 9.2.3-4.
static const std::array<pucch_detector_format0_entry, 4> pucch_detector_format0_twoharq_nosr = {
    {{0, {{}, {0, 0}, {}, {}}}, {3, {{}, {0, 1}, {}, {}}}, {6, {{}, {1, 1}, {}, {}}}, {9, {{}, {1, 0}, {}, {}}}}};
/// Table for one HARQ-ACK feedback bit and one SR bit. Defined in TS38.213 Table 9.2.3-3 for negative SR and Table
/// 9.2.5-1 for positive SR.
static const std::array<pucch_detector_format0_entry, 4> pucch_detector_format0_oneharq_onesr = {
    {{0, {{0}, {0}, {}, {}}}, {6, {{0}, {1}, {}, {}}}, {3, {{1}, {0}, {}, {}}}, {9, {{1}, {1}, {}, {}}}}};
/// Table for two HARQ-ACK feedback bit and one SR bit. Defined in TS38.213 Table 9.2.3-4 for negative SR and Table
/// 9.2.5-2 for positive SR.
static const std::array<pucch_detector_format0_entry, 8> pucch_detector_format0_twoharq_onesr = {
    {{0, {{0}, {0, 0}, {}, {}}},
     {3, {{0}, {0, 1}, {}, {}}},
     {6, {{0}, {1, 1}, {}, {}}},
     {9, {{0}, {1, 0}, {}, {}}},
     {1, {{1}, {0, 0}, {}, {}}},
     {4, {{1}, {0, 1}, {}, {}}},
     {7, {{1}, {1, 1}, {}, {}}},
     {10, {{1}, {1, 0}, {}, {}}}}};

static float pick_threshold(unsigned nof_ports, unsigned nof_symbols, unsigned nof_sequences)
{
  using threshold_index = std::pair<unsigned, unsigned>;
  using threshold_entry = std::pair<threshold_index, float>;

  // Detection thresholds.
  //
  // Thresholds depend on the number of degrees of freedom (i.e., the number of Rx antenna ports times the number of
  // symbols carrying PUCCH) and on the number of sequences that have to be evaluated (1, 2, 4 or 8 depending on the ACK
  // and SR bits in the PUCCH).
  //
  // Thresholds marked as TS38.104 correspond to the cases evaluated in the 3GPP conformance tests.
  // Thresholds marked as TBR (to be refined) haven't been tested yet.
  static constexpr std::array<threshold_entry, 16> pucch_detector_format0_thresholds = {{{{1, 1}, 0.5373},
                                                                                         {{1, 2}, 0.6460},
                                                                                         {{1, 4}, 0.7556},
                                                                                         {{1, 8}, 1.6818},   // TBR
                                                                                         {{2, 1}, 0.5273},   // TBR
                                                                                         {{2, 2}, 0.4038},   // TS38.104
                                                                                         {{2, 4}, 0.7273},   // TBR
                                                                                         {{2, 8}, 0.8364},   // TBR
                                                                                         {{4, 1}, 0.3455},   // TBR
                                                                                         {{4, 2}, 0.2800},   // TS38.104
                                                                                         {{4, 4}, 0.4455},   // TBR
                                                                                         {{4, 8}, 0.5000},   // TBR
                                                                                         {{8, 1}, 0.2545},   // TBR
                                                                                         {{8, 2}, 0.2083},   // TS38.104
                                                                                         {{8, 4}, 0.3000},   // TBR
                                                                                         {{8, 8}, 0.3273}}}; // TBR
  // Number of degrees of freedom.
  unsigned nof_degrees = nof_ports * nof_symbols;

  auto* it = std::lower_bound(pucch_detector_format0_thresholds.begin(),
                              pucch_detector_format0_thresholds.end(),
                              threshold_index(nof_degrees, nof_sequences),
                              [](const threshold_entry& a, const threshold_index& b) { return (a.first < b); });

  srsran_assert(it != pucch_detector_format0_thresholds.end(),
                "Requested configuration ({} antenna ports, {} OFDM symbols, {} sequences) not supported.",
                nof_ports,
                nof_symbols,
                nof_sequences);
  return it->second;
}

std::pair<pucch_uci_message, channel_state_information>
pucch_detector_format0::detect(const srsran::resource_grid_reader&          grid,
                               const pucch_detector::format0_configuration& config)
{
  // Select table.
  span<const pucch_detector_format0_entry> m_cs_table;
  if (config.nof_harq_ack == 0) {
    m_cs_table = span<const pucch_detector_format0_entry>(pucch_detector_format0_noharq_sr);
  } else if (config.nof_harq_ack == 1) {
    if (config.sr_opportunity) {
      m_cs_table = span<const pucch_detector_format0_entry>(pucch_detector_format0_oneharq_onesr);
    } else {
      m_cs_table = span<const pucch_detector_format0_entry>(pucch_detector_format0_oneharq_nosr);
    }
  } else if (config.nof_harq_ack == 2) {
    if (config.sr_opportunity) {
      m_cs_table = span<const pucch_detector_format0_entry>(pucch_detector_format0_twoharq_onesr);
    } else {
      m_cs_table = span<const pucch_detector_format0_entry>(pucch_detector_format0_twoharq_nosr);
    }
  }

  unsigned nof_ports   = config.ports.size();
  unsigned nof_symbols = config.nof_symbols;

  // Verify parameters are correct.
  srsran_assert(!m_cs_table.empty(), "Invalid payload combination.");
  srsran_assert(nof_ports * nof_symbols != 0, "The number of ports or symbols is zero.");

  // Prepare the temporary RE storage.
  temp_re.resize({NRE, nof_symbols, nof_ports});

  // Extract resource elements.
  for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
    // Calculate the initial subcarrier position.
    unsigned i_subc = config.starting_prb * NRE;
    if ((i_symbol != 0) && config.second_hop_prb.has_value()) {
      i_subc = *config.second_hop_prb * NRE;
    }

    // Iterate ports.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      unsigned port_id = config.ports[i_port];
      grid.get(temp_re.get_view({i_symbol, i_port}), port_id, config.start_symbol_index + i_symbol, i_subc);
    }
  }

  // Calculate linear EPRE.
  float epre = 0.0F;
  for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      epre += srsvec::average_power(temp_re.get_view({i_symbol, i_port}));
    }
  }
  epre /= static_cast<float>(nof_symbols * nof_ports);

  // Generate default message.
  pucch_uci_message default_message({config.sr_opportunity ? 1U : 0U, config.nof_harq_ack, 0U, 0U});
  default_message.set_status(uci_status::invalid);

  // Correlate each of the cyclic shifts.
  const pucch_uci_message* best_message = &default_message;
  float                    best_metric  = 0.0F;
  float                    best_rsrp    = 0.0F;
  for (const auto& m_cs_entry : m_cs_table) {
    float sum_corr      = 0.0F;
    float sum_noise_var = 0.0F;

    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      // Compute group sequence.
      unsigned u;
      unsigned v;
      std::tie(u, v) = helper.compute_group_sequence(pucch_group_hopping::NEITHER, config.n_id);

      // Calculate cyclic shift.
      unsigned         alpha    = helper.get_alpha_index(config.slot,
                                              config.cp,
                                              config.n_id,
                                              i_symbol + config.start_symbol_index,
                                              config.initial_cyclic_shift,
                                              m_cs_entry.m_cs);
      span<const cf_t> sequence = low_papr->get(u, v, alpha);

      // Process each receive port.
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        // Select received symbols.
        span<const cf_t> rx_symbols = temp_re.get_view({i_symbol, i_port});

        // Received power.
        float rx_power = srsvec::average_power(rx_symbols);

        // Correlation between received symbols and spreading sequence.
        cf_t rx_seq_corr = srsvec::dot_prod(rx_symbols, sequence);

        // Update cumulative values.
        float corr_contribution = std::norm(rx_seq_corr) / static_cast<float>(NRE);
        sum_corr += corr_contribution;
        sum_noise_var += rx_power * static_cast<float>(NRE) - corr_contribution;
      }
    }

    // Calculate detection metric.
    float detection_metric = 0.0F;
    if (std::isnormal(sum_noise_var)) {
      detection_metric = sum_corr / sum_noise_var;
    }

    // Update the best metric and the actual message.
    if (detection_metric > best_metric) {
      best_metric  = detection_metric;
      best_rsrp    = sum_corr;
      best_message = &m_cs_entry.message;
    }
  }

  pucch_uci_message message             = *best_message;
  float             detection_threshold = pick_threshold(nof_ports, nof_symbols, m_cs_table.size());
  if (best_metric > detection_threshold) {
    message.set_status(uci_status::valid);
  } else {
    message.set_status(uci_status::invalid);
  }

  channel_state_information csi(channel_state_information::sinr_type::post_equalization);
  csi.set_sinr_dB(channel_state_information::sinr_type::post_equalization, convert_power_to_dB(best_metric));
  csi.set_rsrp_dB(convert_power_to_dB(best_rsrp));
  csi.set_epre(convert_power_to_dB(epre));

  return std::make_pair(message, csi);
}
