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

#include "srsran/ran/pusch/pusch_tpmi_select.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/precoding/precoding_weight_matrix.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/srsvec/mean.h"
#include "srsran/support/math/math_utils.h"
#include <array>
#include <cmath>
#include <limits>

using namespace srsran;

static constexpr cf_t sqrt1_2(M_SQRT1_2, 0.0F);
static constexpr cf_t sqrt1_2j(0.0F, M_SQRT1_2);
static constexpr cf_t sqrt1_8(M_SQRT1_2 / 2, 0.0F);
static constexpr cf_t sqrt1_8j(0.0F, M_SQRT1_2 / 2);
static constexpr cf_t sqrt1_12(0.2886751345948129F, 0.0F);
static constexpr cf_t sqrt1_12j(0.0F, 0.2886751345948129F);
static constexpr cf_t dot5(0.5F, 0.0F);
static constexpr cf_t dot5j(0.0F, 0.5F);
static constexpr cf_t dot25(0.25F, 0.0F);
static constexpr cf_t dot25j(0.0F, 0.25F);
static constexpr cf_t zero;

// TS38.211 Table 6.3.1.5-1
static const std::array<precoding_weight_matrix, 6> codebook_1layer_2port = {
    {precoding_weight_matrix({sqrt1_2, zero}, 1, 2),
     precoding_weight_matrix({zero, sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, -sqrt1_2}, 1, 2),
     precoding_weight_matrix({sqrt1_2, sqrt1_2j}, 1, 2),
     precoding_weight_matrix({sqrt1_2, -sqrt1_2j}, 1, 2)}};

// TS38.211 Table 6.3.1.5-3
static const std::array<precoding_weight_matrix, 28> codebook_1layer_4port = {
    {// TPMI Index 0-7
     precoding_weight_matrix({dot5, zero, zero, zero}, 1, 4),
     precoding_weight_matrix({zero, dot5, zero, zero}, 1, 4),
     precoding_weight_matrix({zero, zero, dot5, zero}, 1, 4),
     precoding_weight_matrix({zero, zero, zero, dot5}, 1, 4),
     precoding_weight_matrix({dot5, zero, dot5, zero}, 1, 4),
     precoding_weight_matrix({dot5, zero, -dot5, zero}, 1, 4),
     precoding_weight_matrix({dot5, zero, dot5j, zero}, 1, 4),
     precoding_weight_matrix({dot5, zero, -dot5j, zero}, 1, 4),
     // TPMI Index 8-15
     precoding_weight_matrix({zero, dot5, zero, dot5}, 1, 4),
     precoding_weight_matrix({zero, dot5, zero, -dot5}, 1, 4),
     precoding_weight_matrix({zero, dot5, zero, dot5j}, 1, 4),
     precoding_weight_matrix({zero, dot5, zero, -dot5j}, 1, 4),
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
     precoding_weight_matrix({dot5, -dot5j, -dot5j, -dot5}, 1, 4)}};

// TS38.211 Table 6.3.1.5-4
static const std::array<precoding_weight_matrix, 3> codebook_2layer_2port = {
    {precoding_weight_matrix({sqrt1_2, zero, zero, sqrt1_2}, 2, 2),
     precoding_weight_matrix({dot5, dot5, dot5, -dot5}, 2, 2),
     precoding_weight_matrix({dot5, dot5, dot5j, -dot5j}, 2, 2)}};

// TS38.211 Table 6.3.1.5-5
static const std::array<precoding_weight_matrix, 22> codebook_2layer_4port = {{
    // TPMI Index 0-3
    precoding_weight_matrix({dot5, zero, zero, dot5, zero, zero, zero, zero}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, zero, zero, dot5, zero, zero}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, zero, zero, zero, zero, dot5}, 2, 4),
    precoding_weight_matrix({zero, zero, dot5, zero, zero, dot5, zero, zero}, 2, 4),
    // TPMI Index 4-7
    precoding_weight_matrix({zero, zero, dot5, zero, zero, zero, zero, dot5}, 2, 4),
    precoding_weight_matrix({zero, zero, zero, zero, dot5, zero, zero, dot5}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, dot5, dot5, zero, zero, -dot5j}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, dot5, dot5, zero, zero, dot5j}, 2, 4),
    // TPMI Index 8-11
    precoding_weight_matrix({dot5, zero, zero, dot5, -dot5j, zero, zero, dot5}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, dot5, -dot5j, zero, zero, -dot5}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, dot5, -dot5, zero, zero, -dot5j}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, dot5, -dot5, zero, zero, dot5j}, 2, 4),
    // TPMI Index 12-15
    precoding_weight_matrix({dot5, zero, zero, dot5, dot5j, zero, zero, dot5}, 2, 4),
    precoding_weight_matrix({dot5, zero, zero, dot5, dot5j, zero, zero, -dot5}, 2, 4),
    precoding_weight_matrix({sqrt1_8, sqrt1_8, sqrt1_8, sqrt1_8, sqrt1_8, -sqrt1_8, sqrt1_8, -sqrt1_8}, 2, 4),
    precoding_weight_matrix({sqrt1_8, sqrt1_8, sqrt1_8, sqrt1_8, sqrt1_8j, -sqrt1_8j, sqrt1_8j, -sqrt1_8j}, 2, 4),
    // TPMI Index 16-19
    precoding_weight_matrix({sqrt1_8, sqrt1_8, sqrt1_8j, sqrt1_8j, sqrt1_8, -sqrt1_8, sqrt1_8j, -sqrt1_8j}, 2, 4),
    precoding_weight_matrix({sqrt1_8, sqrt1_8, sqrt1_8j, sqrt1_8j, sqrt1_8j, -sqrt1_8j, -sqrt1_8, sqrt1_8}, 2, 4),
    precoding_weight_matrix({sqrt1_8, sqrt1_8, -sqrt1_8, -sqrt1_8, sqrt1_8, -sqrt1_8, -sqrt1_8, sqrt1_8}, 2, 4),
    precoding_weight_matrix({sqrt1_8, sqrt1_8, -sqrt1_8, -sqrt1_8, sqrt1_8j, -sqrt1_8j, -sqrt1_8j, sqrt1_8j}, 2, 4),
    // TPMI Index 20-21
    precoding_weight_matrix({sqrt1_8, sqrt1_8, -sqrt1_8j, -sqrt1_8j, sqrt1_8, -sqrt1_8, -sqrt1_8j, sqrt1_8j}, 2, 4),
    precoding_weight_matrix({sqrt1_8, sqrt1_8, -sqrt1_8j, -sqrt1_8j, sqrt1_8j, -sqrt1_8j, sqrt1_8, -sqrt1_8}, 2, 4),
}};

// TS38.211 Table 6.3.1.5-6
static const std::array<precoding_weight_matrix, 7> codebook_3layer_4port = {{
    // TPMI Index 0-3
    precoding_weight_matrix({dot5, zero, zero, zero, dot5, zero, zero, zero, dot5, zero, zero, zero}, 3, 4),
    precoding_weight_matrix({dot5, zero, zero, zero, dot5, zero, dot5, zero, zero, zero, zero, dot5}, 3, 4),
    precoding_weight_matrix({dot5, zero, zero, zero, dot5, zero, -dot5, zero, zero, zero, zero, dot5}, 3, 4),
    precoding_weight_matrix({sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             -sqrt1_12},
                            3,
                            4),
    // TPMI Index 4-6
    precoding_weight_matrix({sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12,
                             sqrt1_12j,
                             sqrt1_12j,
                             -sqrt1_12j,
                             sqrt1_12j,
                             -sqrt1_12j,
                             -sqrt1_12j},
                            3,
                            4),
    precoding_weight_matrix({sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12,
                             sqrt1_12},
                            3,
                            4),
    precoding_weight_matrix({sqrt1_12,
                             sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12,
                             -sqrt1_12,
                             sqrt1_12j,
                             sqrt1_12j,
                             -sqrt1_12j,
                             -sqrt1_12j,
                             sqrt1_12j,
                             sqrt1_12j},
                            3,
                            4),
}};

// TS38.211 Table 6.3.1.5-7
static const std::array<precoding_weight_matrix, 5> codebook_4layer_4port = {{
    // TPMI Index 0-3
    precoding_weight_matrix(
        {dot5, zero, zero, zero, zero, dot5, zero, zero, zero, zero, dot5, zero, zero, zero, zero, dot5},
        4,
        4),
    precoding_weight_matrix({sqrt1_8,
                             sqrt1_8,
                             zero,
                             zero,
                             zero,
                             zero,
                             sqrt1_8,
                             sqrt1_8,
                             sqrt1_8,
                             -sqrt1_8,
                             zero,
                             zero,
                             zero,
                             zero,
                             sqrt1_8,
                             -sqrt1_8},
                            4,
                            4),
    precoding_weight_matrix({sqrt1_8,
                             sqrt1_8,
                             zero,
                             zero,
                             zero,
                             zero,
                             sqrt1_8,
                             sqrt1_8,
                             sqrt1_8j,
                             -sqrt1_8j,
                             zero,
                             zero,
                             zero,
                             zero,
                             sqrt1_8j,
                             -sqrt1_8j},
                            4,
                            4),
    precoding_weight_matrix({dot25,
                             dot25,
                             dot25,
                             dot25,
                             dot25,
                             -dot25,
                             dot25,
                             -dot25,
                             dot25,
                             dot25,
                             -dot25,
                             -dot25,
                             dot25,
                             -dot25,
                             -dot25,
                             dot25},
                            4,
                            4),
    precoding_weight_matrix({dot25,
                             dot25,
                             dot25,
                             dot25,
                             dot25,
                             -dot25,
                             dot25,
                             -dot25,
                             dot25j,
                             dot25j,
                             -dot25j,
                             -dot25j,
                             dot25j,
                             -dot25j,
                             -dot25j,
                             dot25j},
                            4,
                            4),
}};

// Calculates the product of a channel matrix and a precoding weight matrix. The number of transmit ports of each matrix
// must be equal.
static precoding_weight_matrix product_channel_weight(const srs_channel_matrix& H, const precoding_weight_matrix& W)
{
  srsran_assert(H.get_nof_tx_ports() == W.get_nof_ports(), "Number of Ports is not equal.");
  unsigned nof_tx_ports = W.get_nof_ports();
  unsigned nof_layers   = W.get_nof_layers();
  unsigned nof_rx_ports = H.get_nof_rx_ports();

  precoding_weight_matrix result(nof_layers, nof_rx_ports);
  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      cf_t sum = 0;
      for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
        sum += H.get_coefficient(i_rx_port, i_tx_port) * W.get_coefficient(i_layer, i_tx_port);
      }
      result.set_coefficient(sum, i_layer, i_rx_port);
    }
  }

  return result;
}

// Calculates the gram matrix.
template <unsigned N>
static void gram_channel_matrix(cf_t out[N][N], const precoding_weight_matrix& H)
{
  unsigned nof_layers   = H.get_nof_layers();
  unsigned nof_rx_ports = H.get_nof_ports();

  precoding_weight_matrix result(nof_layers, nof_layers);
  for (unsigned i = 0; i != nof_layers; ++i) {
    for (unsigned j = 0; j != nof_layers; ++j) {
      for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
        cf_t sum;
        for (unsigned k = 0; k != nof_rx_ports; ++k) {
          sum += H.get_coefficient(i, k) * std::conj(H.get_coefficient(j, k));
        }
        out[j][i] = sum;
      }
    }
  }
}

// Select the sub elements of a matrix eliminating a given row and column.
template <unsigned N>
static void subchannel_matrix(cf_t out[N - 1][N - 1], const cf_t in[N][N], unsigned i_skip_row, unsigned i_skip_col)
{
  unsigned i_row_count = 0;
  for (unsigned i_row = 0; i_row != N; ++i_row) {
    if (i_row == i_skip_row) {
      continue;
    }
    unsigned i_col_count = 0;
    for (unsigned i_col = 0; i_col != N; ++i_col) {
      if (i_col == i_skip_col) {
        continue;
      }
      out[i_col_count++][i_row_count] = in[i_col][i_row];
    }
    ++i_row_count;
  }
}

// Calculate the determinant of a matrix of size N.
template <unsigned N>
cf_t det_channel_matrix(const cf_t in[N][N])
{
  static constexpr unsigned subN = N - 1;
  cf_t                      sub[subN][subN];
  cf_t                      sum;
  float                     sign = 1;
  for (unsigned i = 0; i != N; ++i) {
    subchannel_matrix(sub, in, i, 0);
    cf_t subdet = det_channel_matrix<N - 1>(sub);
    sum += sign * in[0][i] * subdet;
    sign *= -1;
  }
  return sum;
}

// Calculate the determinant of a matrix of size 2.
template <>
cf_t det_channel_matrix<2>(const cf_t in[2][2])
{
  return in[0][0] * in[1][1] - in[0][1] * in[1][0];
}

// Calculate the determinant of a matrix of size 3.
template <>
cf_t det_channel_matrix<3>(const cf_t in[3][3])
{
  return in[0][0] * (in[1][1] * in[2][2] - in[1][2] * in[2][1]) +
         in[0][1] * (in[1][2] * in[2][0] - in[2][2] * in[1][0]) +
         in[0][2] * (in[1][0] * in[2][1] - in[1][1] * in[2][0]);
}

// Calculate the determinant of a matrix of size 1.
template <>
cf_t det_channel_matrix<1>(const cf_t in[1][1])
{
  return in[0][0];
}

// Calculate the mean layer SINR based on the channel matrix and noise variance.
template <unsigned NofLayers>
std::array<float, NofLayers> calculate_mean_layer_sinr(const precoding_weight_matrix& channel_weights,
                                                       float                          noise_variance)
{
  // Calculate Gram Matrix.
  cf_t gram_matrix[NofLayers][NofLayers];
  gram_channel_matrix(gram_matrix, channel_weights);

  // Add noise variance to the diagonal.
  for (unsigned i = 0; i != NofLayers; ++i) {
    gram_matrix[i][i] += noise_variance;
  }

  // Calculate gram matrix determinant. It must be real.
  float det = det_channel_matrix<NofLayers>(gram_matrix).real();

  // Calculate the diagonal of the inverse, it must be real.
  std::array<float, NofLayers> inv_diagonal;
  cf_t                         temp_sub[NofLayers - 1][NofLayers - 1];
  for (unsigned i = 0; i != NofLayers; ++i) {
    subchannel_matrix(temp_sub, gram_matrix, i, i);
    inv_diagonal[i] = det_channel_matrix<NofLayers - 1>(temp_sub).real() / det;
  }

  // Estimate noise variances.
  std::array<float, NofLayers> layer_sinr;
  for (unsigned i_layer = 0; i_layer != NofLayers; ++i_layer) {
    layer_sinr[i_layer] = 1.0F / (inv_diagonal[i_layer] * noise_variance) - 1.0F;
  }

  return layer_sinr;
}

static pusch_tpmi_select_info::tpmi_info get_tpmi_select_info_1layer(const srs_channel_matrix& channel,
                                                                     float                     noise_variance,
                                                                     tx_scheme_codebook_subset codebook_subset)
{
  unsigned nof_rx_ports = channel.get_nof_rx_ports();
  unsigned nof_tx_ports = channel.get_nof_tx_ports();

  // Select codebook as a function of the number of transmit ports and restrict the candidates according to its subset.
  span<const precoding_weight_matrix> codebook;
  unsigned                            tpmi_end = codebook.size();
  if (nof_tx_ports == 2) {
    codebook = codebook_1layer_2port;
    switch (codebook_subset) {
      case tx_scheme_codebook_subset::fully_and_partial_and_non_coherent:
      case tx_scheme_codebook_subset::partial_and_non_coherent:
        tpmi_end = codebook.size();
        break;
      case tx_scheme_codebook_subset::non_coherent:
        tpmi_end = 2;
        break;
    }
  } else {
    codebook = codebook_1layer_4port;
    switch (codebook_subset) {
      case tx_scheme_codebook_subset::fully_and_partial_and_non_coherent:
        tpmi_end = codebook.size();
        break;
      case tx_scheme_codebook_subset::partial_and_non_coherent:
        tpmi_end = 12;
        break;
      case tx_scheme_codebook_subset::non_coherent:
        tpmi_end = 4;
        break;
    }
  }

  float    best_sinr = -std::numeric_limits<float>::infinity();
  unsigned best_tpmi = 0;

  // Iterate possible TPMIs.
  for (unsigned tpmi = 0; tpmi != tpmi_end; ++tpmi) {
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

  float sinr_dB = convert_power_to_dB(best_sinr);
  return {.tpmi = best_tpmi, .avg_sinr_dB = sinr_dB, .sinr_dB_layer = {sinr_dB}};
}

static pusch_tpmi_select_info::tpmi_info get_tpmi_select_info_2layer(const srs_channel_matrix& channel,
                                                                     float                     noise_variance,
                                                                     tx_scheme_codebook_subset codebook_subset)
{
  static constexpr unsigned nof_layers   = 2;
  unsigned                  nof_tx_ports = channel.get_nof_tx_ports();

  // Select codebook as a function of the number of transmit ports and restrict the candidates according to its subset.
  span<const precoding_weight_matrix> codebook;
  unsigned                            tpmi_end = codebook.size();
  if (nof_tx_ports == 2) {
    codebook = codebook_2layer_2port;
    switch (codebook_subset) {
      case tx_scheme_codebook_subset::fully_and_partial_and_non_coherent:
      case tx_scheme_codebook_subset::partial_and_non_coherent:
        tpmi_end = codebook.size();
        break;
      case tx_scheme_codebook_subset::non_coherent:
        tpmi_end = 1;
        break;
    }
  } else {
    codebook = codebook_2layer_4port;
    switch (codebook_subset) {
      case tx_scheme_codebook_subset::fully_and_partial_and_non_coherent:
        tpmi_end = codebook.size();
        break;
      case tx_scheme_codebook_subset::partial_and_non_coherent:
        tpmi_end = 14;
        break;
      case tx_scheme_codebook_subset::non_coherent:
        tpmi_end = 6;
        break;
    }
  }

  static_vector<float, pusch_constants::MAX_NOF_LAYERS> best_sinr_layer;
  float                                                 best_sinr = -std::numeric_limits<float>::infinity();
  unsigned                                              best_tpmi = 0;

  // Iterate possible TPMIs.
  for (unsigned tpmi = 0; tpmi != tpmi_end; ++tpmi) {
    // Select precoding matrix.
    const precoding_weight_matrix& weights = codebook[tpmi];

    // Calculate the product of the channel matrix and the precoding weights.
    precoding_weight_matrix channel_weights = product_channel_weight(channel, weights);

    // Calculate the average SINR.
    auto  layer_sinr = calculate_mean_layer_sinr<nof_layers>(channel_weights, noise_variance);
    float avg_sinr   = srsvec::mean(layer_sinr);
    if (avg_sinr > best_sinr) {
      best_sinr_layer = {layer_sinr.begin(), layer_sinr.end()};
      best_sinr       = avg_sinr;
      best_tpmi       = tpmi;
    }
  }

  // Convert SINR to dB.
  std::transform(best_sinr_layer.begin(), best_sinr_layer.end(), best_sinr_layer.begin(), [](float sinr) {
    return convert_power_to_dB(sinr);
  });

  return {.tpmi = best_tpmi, .avg_sinr_dB = convert_power_to_dB(best_sinr), .sinr_dB_layer = best_sinr_layer};
}

static pusch_tpmi_select_info::tpmi_info get_tpmi_select_info_3layer(const srs_channel_matrix& channel,
                                                                     float                     noise_variance,
                                                                     tx_scheme_codebook_subset codebook_subset)
{
  static constexpr unsigned nof_layers = 3;

  // Select codebook as a function of the number of transmit ports and restrict the candidates according to its subset.
  span<const precoding_weight_matrix> codebook;
  unsigned                            tpmi_end = codebook.size();

  codebook = codebook_3layer_4port;
  switch (codebook_subset) {
    case tx_scheme_codebook_subset::fully_and_partial_and_non_coherent:
      tpmi_end = codebook.size();
      break;
    case tx_scheme_codebook_subset::partial_and_non_coherent:
      tpmi_end = 3;
      break;
    case tx_scheme_codebook_subset::non_coherent:
      tpmi_end = 1;
      break;
  }

  static_vector<float, pusch_constants::MAX_NOF_LAYERS> best_sinr_layer;
  float                                                 best_sinr = -std::numeric_limits<float>::infinity();
  unsigned                                              best_tpmi = 0;

  // Iterate possible TPMIs.
  for (unsigned tpmi = 0; tpmi != tpmi_end; ++tpmi) {
    // Select precoding matrix.
    const precoding_weight_matrix& weights = codebook[tpmi];

    // Calculate the product of the channel matrix and the precoding weights.
    precoding_weight_matrix channel_weights = product_channel_weight(channel, weights);

    // Calculate the average SINR.
    auto  layer_sinr = calculate_mean_layer_sinr<nof_layers>(channel_weights, noise_variance);
    float avg_sinr   = srsvec::mean(layer_sinr);
    if (avg_sinr > best_sinr) {
      best_sinr_layer = {layer_sinr.begin(), layer_sinr.end()};
      best_sinr       = avg_sinr;
      best_tpmi       = tpmi;
    }
  }

  // Convert SINR to dB.
  std::transform(best_sinr_layer.begin(), best_sinr_layer.end(), best_sinr_layer.begin(), [](float sinr) {
    return convert_power_to_dB(sinr);
  });

  return {.tpmi = best_tpmi, .avg_sinr_dB = convert_power_to_dB(best_sinr), .sinr_dB_layer = best_sinr_layer};
}

static pusch_tpmi_select_info::tpmi_info get_tpmi_select_info_4layer(const srs_channel_matrix& channel,
                                                                     float                     noise_variance,
                                                                     tx_scheme_codebook_subset codebook_subset)
{
  static constexpr unsigned nof_layers = 4;

  // Select codebook as a function of the number of transmit ports and restrict the candidates according to its subset.
  span<const precoding_weight_matrix> codebook;
  unsigned                            tpmi_end = codebook.size();

  codebook = codebook_4layer_4port;
  switch (codebook_subset) {
    case tx_scheme_codebook_subset::fully_and_partial_and_non_coherent:
      tpmi_end = codebook.size();
      break;
    case tx_scheme_codebook_subset::partial_and_non_coherent:
      tpmi_end = 3;
      break;
    case tx_scheme_codebook_subset::non_coherent:
      tpmi_end = 1;
      break;
  }

  static_vector<float, pusch_constants::MAX_NOF_LAYERS> best_sinr_layer;
  float                                                 best_sinr = -std::numeric_limits<float>::infinity();
  unsigned                                              best_tpmi = 0;

  // Iterate possible TPMIs.
  for (unsigned tpmi = 0; tpmi != tpmi_end; ++tpmi) {
    // Select precoding matrix.
    const precoding_weight_matrix& weights = codebook[tpmi];

    // Calculate the product of the channel matrix and the precoding weights.
    precoding_weight_matrix channel_weights = product_channel_weight(channel, weights);

    // Calculate the average SINR.
    auto  layer_sinr = calculate_mean_layer_sinr<nof_layers>(channel_weights, noise_variance);
    float avg_sinr   = srsvec::mean(layer_sinr);
    if (avg_sinr > best_sinr) {
      best_sinr_layer = {layer_sinr.begin(), layer_sinr.end()};
      best_sinr       = avg_sinr;
      best_tpmi       = tpmi;
    }
  }

  // Convert SINR to dB.
  std::transform(best_sinr_layer.begin(), best_sinr_layer.end(), best_sinr_layer.begin(), [](float sinr) {
    return convert_power_to_dB(sinr);
  });

  return {.tpmi = best_tpmi, .avg_sinr_dB = convert_power_to_dB(best_sinr), .sinr_dB_layer = best_sinr_layer};
}

pusch_tpmi_select_info srsran::get_tpmi_select_info(const srs_channel_matrix& channel,
                                                    float                     noise_variance,
                                                    unsigned                  max_rank,
                                                    tx_scheme_codebook_subset codebook_subset)
{
  unsigned nof_tx_ports   = channel.get_nof_tx_ports();
  unsigned nof_rx_ports   = channel.get_nof_rx_ports();
  unsigned max_nof_layers = std::min(std::min(nof_tx_ports, nof_rx_ports), max_rank);

  static_vector<pusch_tpmi_select_info::tpmi_info, pusch_constants::MAX_NOF_LAYERS> info;

  // Calculate TPMI select information for 1 layer.
  if (max_nof_layers >= 1) {
    info.emplace_back(get_tpmi_select_info_1layer(channel, noise_variance, codebook_subset));
  }

  // Calculate TPMI select information for 2 layer.
  if (max_nof_layers >= 2) {
    info.emplace_back(get_tpmi_select_info_2layer(channel, noise_variance, codebook_subset));
  }

  // Calculate TPMI select information for 3 layer.
  if ((max_nof_layers >= 3) && (nof_tx_ports == 4) && (nof_rx_ports == 4)) {
    info.emplace_back(get_tpmi_select_info_3layer(channel, noise_variance, codebook_subset));
  }

  // Calculate TPMI select information for 4 layer.
  if ((max_nof_layers >= 4) && (nof_tx_ports == 4) && (nof_rx_ports == 4)) {
    info.emplace_back(get_tpmi_select_info_4layer(channel, noise_variance, codebook_subset));
  }

  // Return invalid information.
  return {info};
}
