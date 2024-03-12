/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/pusch/pusch_tpmi_select.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/precoding/precoding_weight_matrix.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/support/math_utils.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

using namespace srsran;

static constexpr cf_t sqrt1_2(M_SQRT1_2, 0);
static constexpr cf_t sqrt1_2j(0, M_SQRT1_2);
static constexpr cf_t dot5(0.5, 0);
static constexpr cf_t dot5j(0, 0.5);

// TS38.211 Table 6.3.1.5-1
static const std::array<precoding_weight_matrix, 6> codebook_1layer_2port = {
    {precoding_weight_matrix({sqrt1_2, 0}, 1, 2),
     precoding_weight_matrix({0, sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, -sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, sqrt1_2j}, 1, 2),
     precoding_weight_matrix({sqrt1_2, -sqrt1_2j}, 1, 2)}};

// TS38.211 Table 6.3.1.5-3
static const std::array<precoding_weight_matrix, 28> codebook_1layer_4port = {{
    // TPMI Index 0-7
    precoding_weight_matrix({dot5, 0, 0, 0}, 1, 4),
    precoding_weight_matrix({0, dot5, 0, 0}, 1, 4),
    precoding_weight_matrix({0, 0, dot5, 0}, 1, 4),
    precoding_weight_matrix({0, 0, 0, dot5}, 1, 4),
    precoding_weight_matrix({dot5, 0, dot5, 0}, 1, 4),
    precoding_weight_matrix({dot5, 0, -dot5, 0}, 1, 4),
    precoding_weight_matrix({dot5, 0, dot5j, 0}, 1, 4),
    precoding_weight_matrix({dot5, 0, -dot5j, 0}, 1, 4),
    // TPMI Index 8-15
    precoding_weight_matrix({0, dot5, 0, dot5}, 1, 4),
    precoding_weight_matrix({0, dot5, 0, -dot5}, 1, 4),
    precoding_weight_matrix({0, dot5, 0, dot5j}, 1, 4),
    precoding_weight_matrix({0, dot5, 0, -dot5j}, 1, 4),
    precoding_weight_matrix({dot5, dot5, dot5, dot5}, 1, 4),
    precoding_weight_matrix({dot5, dot5, dot5j, dot5j}, 1, 4),
    precoding_weight_matrix({dot5, dot5, -dot5, -dot5}, 1, 4),
    precoding_weight_matrix({dot5, dot5, -dot5j, -dot5j}, 1, 4),
    // TPMI Index 16-23
    precoding_weight_matrix({dot5, dot5j, dot5, dot5j}, 1, 4),
    precoding_weight_matrix({dot5, dot5j, dot5j, -dot5}, 1, 4),
    precoding_weight_matrix({dot5, dot5j, -dot5, -dot5j}, 1, 4),
    precoding_weight_matrix({dot5, dot5j, -dot5j, dot5}, 1, 4),
    precoding_weight_matrix({dot5, -dot5, dot5, -dot5}, 1, 4),
    precoding_weight_matrix({dot5, -dot5, dot5j, -dot5j}, 1, 4),
    precoding_weight_matrix({dot5, -dot5, -dot5, dot5}, 1, 4),
    precoding_weight_matrix({dot5, -dot5, -dot5j, dot5j}, 1, 4),
    // TPMI Index 24-27
    precoding_weight_matrix({dot5, -dot5j, dot5, -dot5j}, 1, 4),
    precoding_weight_matrix({dot5, -dot5j, dot5j, dot5}, 1, 4),
    precoding_weight_matrix({dot5, -dot5j, -dot5, dot5j}, 1, 4),
    precoding_weight_matrix({dot5, -dot5j, -dot5j, -dot5}, 1, 4),
}};

static pusch_tpmi_select_info::tpmi_info get_tpmi_select_info_1layer(const srs_channel_matrix& channel,
                                                                     float                     noise_variance)
{
  unsigned nof_rx_ports = channel.get_nof_rx_ports();
  unsigned nof_tx_ports = channel.get_nof_tx_ports();

  // Select codebook in function of the number of transmit ports.
  span<const precoding_weight_matrix> codebook = codebook_1layer_2port;
  if (nof_tx_ports == 4) {
    codebook = codebook_1layer_4port;
  }

  float    best_sinr = -std::numeric_limits<float>::infinity();
  unsigned best_tpmi = 0;

  // Iterate possible TPMIs.
  for (unsigned tpmi = 0, tpmi_end = codebook.size(); tpmi != tpmi_end; ++tpmi) {
    // Select precoding matrix.
    const precoding_weight_matrix& weights = codebook[tpmi];

    float signal_power = 0;

    // Combine channel coefficients with precoding weights.
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      cf_t sum = 0;
      for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
        // Select precoding weight.
        cf_t w = weights.get_coefficient(0, i_tx_port);
        // Select channel coefficient.
        cf_t h = channel.get_coefficient(i_rx_port, i_tx_port);

        // Sum absolute square.
        sum += h * w;
      }

      signal_power += abs_sq(sum);
    }

    float sinr = signal_power / noise_variance;
    if (sinr > best_sinr) {
      best_sinr = sinr;
      best_tpmi = tpmi;
    }
  }

  return {best_tpmi, convert_power_to_dB(best_sinr)};
}

pusch_tpmi_select_info srsran::get_tpmi_select_info(const srs_channel_matrix& channel, float noise_variance)
{
  unsigned nof_tx_ports   = channel.get_nof_tx_ports();
  unsigned nof_rx_ports   = channel.get_nof_rx_ports();
  unsigned max_nof_layers = std::min(nof_tx_ports, nof_rx_ports);

  static_vector<pusch_tpmi_select_info::tpmi_info, pusch_constants::MAX_NOF_LAYERS> info;

  // Calculate TPMI select information for 1 layer.
  if (max_nof_layers >= 1) {
    info.emplace_back(get_tpmi_select_info_1layer(channel, noise_variance));
  }

  // Return invalid information.
  return {info};
}