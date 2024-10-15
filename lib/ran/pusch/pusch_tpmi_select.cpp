/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include "srsran/support/math_utils.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

using namespace srsran;

static constexpr cf_t sqrt1_2(M_SQRT1_2, 0.0F);
static constexpr cf_t sqrt1_2j(0.0F, M_SQRT1_2);
static constexpr cf_t sqrt1_8(M_SQRT1_2 / 2, 0.0F);
static constexpr cf_t sqrt1_8j(0.0F, M_SQRT1_2 / 2);
static constexpr cf_t dot5(0.5F, 0.0F);
static constexpr cf_t dot5j(0.0F, 0.5F);
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

static pusch_tpmi_select_info::tpmi_info get_tpmi_select_info_1layer(const srs_channel_matrix& channel,
                                                                     float                     noise_variance,
                                                                     tx_scheme_codebook_subset codebook_subset)
{
  unsigned nof_rx_ports = channel.get_nof_rx_ports();
  unsigned nof_tx_ports = channel.get_nof_tx_ports();

  // Select codebook as a function of the number of transmit ports and restrict the candidates according to its subset.
  span<const precoding_weight_matrix> codebook;
  unsigned                            tpmi_end;
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

  return {best_tpmi, convert_power_to_dB(best_sinr)};
}

static pusch_tpmi_select_info::tpmi_info get_tpmi_select_info_2layer(const srs_channel_matrix& channel,
                                                                     float                     noise_variance,
                                                                     tx_scheme_codebook_subset codebook_subset)
{
  static constexpr unsigned nof_layers   = 2;
  unsigned                  nof_rx_ports = channel.get_nof_rx_ports();
  unsigned                  nof_tx_ports = channel.get_nof_tx_ports();

  // Select codebook as a function of the number of transmit ports and restrict the candidates according to its subset.
  span<const precoding_weight_matrix> codebook;
  unsigned                            tpmi_end;
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

  float    best_sinr = -std::numeric_limits<float>::infinity();
  unsigned best_tpmi = 0;

  // Iterate possible TPMIs.
  for (unsigned tpmi = 0; tpmi != tpmi_end; ++tpmi) {
    // Select precoding matrix.
    const precoding_weight_matrix& weights = codebook[tpmi];

    // Calculate the product of the channel matrix and the precoding weights.
    precoding_weight_matrix channel_weights = product_channel_weight(channel, weights);

    // Calculate the product of the channel matrix (recall, it's an Nx2 matrix) and its hermitian transpose. Also, add
    // the noise variance. The diagonal coefficients are the squared norms of the channel matrix column vectors.
    std::array<float, nof_layers> norm_sq_ch;
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      float sum = noise_variance;
      for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
        sum += std::norm(channel_weights.get_coefficient(i_layer, i_port));
      }
      norm_sq_ch[i_layer] = sum;
    }

    // Calculate the anti-diagonal coefficients, which are xi and xi_conj.
    cf_t xi = cf_t();
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      xi += std::conj(channel_weights.get_coefficient(0, i_port)) * channel_weights.get_coefficient(1, i_port);
    }
    float xi_mod_sq = std::norm(xi);

    // Calculate the denominators.
    float det = ((norm_sq_ch[0] * norm_sq_ch[1]) - xi_mod_sq);

    // Estimate noise variances.
    std::array<float, nof_layers> layer_sinr;
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      layer_sinr[i_layer] = det / (norm_sq_ch[i_layer] * noise_variance) - 1.0F;
    }

    float sinr = srsvec::mean(layer_sinr);
    if (sinr > best_sinr) {
      best_sinr = sinr;
      best_tpmi = tpmi;
    }
  }

  return {best_tpmi, convert_power_to_dB(best_sinr)};
}

pusch_tpmi_select_info srsran::get_tpmi_select_info(const srs_channel_matrix& channel,
                                                    float                     noise_variance,
                                                    tx_scheme_codebook_subset codebook_subset)
{
  unsigned nof_tx_ports   = channel.get_nof_tx_ports();
  unsigned nof_rx_ports   = channel.get_nof_rx_ports();
  unsigned max_nof_layers = std::min(nof_tx_ports, nof_rx_ports);

  static_vector<pusch_tpmi_select_info::tpmi_info, pusch_constants::MAX_NOF_LAYERS> info;

  // Calculate TPMI select information for 1 layer.
  if (max_nof_layers >= 1) {
    info.emplace_back(get_tpmi_select_info_1layer(channel, noise_variance, codebook_subset));
  }

  // Calculate TPMI select information for 2 layer.
  if (max_nof_layers >= 2) {
    info.emplace_back(get_tpmi_select_info_2layer(channel, noise_variance, codebook_subset));
  }

  // Return invalid information.
  return {info};
}
