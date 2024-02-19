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
#include "srsran/ran/precoding/precoding_weight_matrix.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/support/math_utils.h"
#include <array>
#include <cmath>

using namespace srsran;

static constexpr cf_t sqrt1_2(M_SQRT1_2, 0);

static constexpr cf_t sqrt1_2j(0, M_SQRT1_2);

static const std::array<precoding_weight_matrix, 6> codebook_1layer_2port = {
    {precoding_weight_matrix({sqrt1_2, 0}, 1, 2),
     precoding_weight_matrix({0, sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, -sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, sqrt1_2j}, 1, 2),
     precoding_weight_matrix({sqrt1_2, -sqrt1_2j}, 1, 2)}};

static pusch_tpmi_select_info get_tpmi_select_info_1x2(const srs_channel_matrix& channel, float noise_variance)
{
  float    best_sinr = -std::numeric_limits<float>::infinity();
  unsigned best_tpmi = 0;

  // Iterate possible TPMIs.
  for (unsigned tpmi = 0, tpmi_end = codebook_1layer_2port.size(); tpmi != tpmi_end; ++tpmi) {
    // Select precoding matrix.
    const precoding_weight_matrix& w    = codebook_1layer_2port[tpmi];
    cf_t                           w_p0 = w.get_coefficient(0, 0);
    cf_t                           w_p1 = w.get_coefficient(0, 1);

    cf_t h_00 = channel.get_coefficient(0, 0);
    cf_t h_01 = channel.get_coefficient(0, 1);

    cf_t  wh   = w_p0 * h_00 + w_p1 * h_01;
    float whhw = abs_sq(wh);

    float sinr = whhw / noise_variance;
    if (sinr > best_sinr) {
      best_sinr = sinr;
      best_tpmi = tpmi;
    }
  }

  return {{best_tpmi, convert_power_to_dB(best_sinr)}};
}

pusch_tpmi_select_info srsran::get_tpmi_select_info(const srs_channel_matrix& channel, float noise_variance)
{
  unsigned nof_tx_ports = channel.get_nof_tx_ports();
  unsigned nof_rx_ports = channel.get_nof_rx_ports();

  // Two transmit, one receive ports.
  if ((nof_tx_ports == 2) && (nof_rx_ports == 1)) {
    return get_tpmi_select_info_1x2(channel, noise_variance);
  }

  // Return invalid information.
  return {};
}