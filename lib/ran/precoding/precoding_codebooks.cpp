/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/adt/interval.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

precoding_configuration srsran::make_single_port()
{
  return make_wideband_one_layer_one_port(1, 0);
}

precoding_configuration srsran::make_wideband_one_layer_one_port(unsigned nof_ports, unsigned selected_i_port)
{
  interval<unsigned, false> selected_i_port_range(0, nof_ports);
  srsran_assert(selected_i_port_range.contains(selected_i_port),
                "The given port identifier (i.e., {}) is out of the valid range {}",
                selected_i_port,
                selected_i_port_range);

  precoding_configuration result;
  result.resize(1, nof_ports, 1, MAX_NOF_PRBS);

  // Set weights per port.
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    cf_t port_weight = (i_port == selected_i_port) ? 1.0F : 0.0F;
    result.set_coefficient(port_weight, 0, i_port, 0);
  }

  return result;
}

precoding_configuration srsran::make_wideband_one_layer_all_ports(unsigned nof_ports)
{
  interval<unsigned, true> nof_ports_range(1, precoding_constants::MAX_NOF_PORTS);
  srsran_assert(nof_ports_range.contains(nof_ports),
                "The number of ports (i.e., {}) is out of the valid range {}.",
                nof_ports,
                nof_ports_range);

  precoding_configuration result;
  result.resize(1, nof_ports, 1, MAX_NOF_PRBS);

  // Set normalized weights per port.
  cf_t weight = {1.0F / std::sqrt(static_cast<float>(nof_ports)), 0.0F};
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    result.set_coefficient(weight, 0, i_port, 0);
  }

  return result;
}

precoding_configuration srsran::make_wideband_identity(unsigned nof_streams)
{
  static constexpr interval<unsigned, true> nof_streams_range(1, precoding_constants::MAX_NOF_LAYERS);

  srsran_assert(nof_streams_range.contains(nof_streams),
                "The number of streams (i.e., {}) is out of the valid range {}.",
                nof_streams,
                nof_streams_range);

  precoding_configuration result;
  result.resize(nof_streams, nof_streams, 1, MAX_NOF_PRBS);

  cf_t normalised_weight = 1.0F / std::sqrt(static_cast<float>(nof_streams));

  // Set weights per port.
  for (unsigned i_layer = 0; i_layer != nof_streams; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_streams; ++i_port) {
      cf_t weight = (i_layer == i_port) ? normalised_weight : 0.0F;
      result.set_coefficient(weight, i_layer, i_port, 0);
    }
  }
  return result;
}

precoding_configuration srsran::make_wideband_one_layer_two_ports(unsigned i_codebook)
{
  static constexpr interval<unsigned, true>           i_codebook_range(0, 3);
  static constexpr cf_t                               sqrt_1_2         = {M_SQRT1_2, 0};
  static constexpr cf_t                               j_sqrt_1_2       = {0, M_SQRT1_2};
  static constexpr cf_t                               minus_sqrt_1_2   = {-M_SQRT1_2, 0};
  static constexpr cf_t                               minus_j_sqrt_1_2 = {0, -M_SQRT1_2};
  static constexpr std::array<std::array<cf_t, 2>, 4> codebooks        = {
             {{sqrt_1_2, sqrt_1_2}, {sqrt_1_2, j_sqrt_1_2}, {sqrt_1_2, minus_sqrt_1_2}, {sqrt_1_2, minus_j_sqrt_1_2}}};

  srsran_assert(i_codebook_range.contains(i_codebook),
                "The given codebook identifier (i.e., {}) is out of the range {}",
                i_codebook,
                i_codebook_range);

  precoding_configuration result;
  result.resize(1, 2, 1, MAX_NOF_PRBS);

  // Select codebook.
  span<const cf_t> codebook = codebooks[i_codebook];

  // Set weights per port.
  for (unsigned i_port = 0; i_port != 2; ++i_port) {
    result.set_coefficient(codebook[i_port], 0, i_port, 0);
  }

  return result;
}

precoding_configuration srsran::make_wideband_two_layer_two_ports(unsigned i_codebook)
{
  static constexpr interval<unsigned, true>           i_codebook_range(0, 1);
  static constexpr cf_t                               dot_five         = {0.5F, 0.0F};
  static constexpr cf_t                               minus_dot_five   = {-0.5F, 0.0F};
  static constexpr cf_t                               j_dot_five       = {0.0F, 0.5F};
  static constexpr cf_t                               minus_j_dot_five = {0.0F, -0.5F};
  static constexpr std::array<std::array<cf_t, 2>, 2> codebook0 = {{{dot_five, dot_five}, {dot_five, minus_dot_five}}};
  static constexpr std::array<std::array<cf_t, 2>, 2> codebook1 = {
      {{dot_five, dot_five}, {j_dot_five, minus_j_dot_five}}};

  srsran_assert(i_codebook_range.contains(i_codebook),
                "The given codebook identifier (i.e., {}) is out of the range {}",
                i_codebook,
                i_codebook_range);

  precoding_configuration result;
  result.resize(2, 2, 1, MAX_NOF_PRBS);

  // Select codebook.
  const std::array<std::array<cf_t, 2>, 2>& codebook = (i_codebook == 0) ? codebook0 : codebook1;

  // Set weights per port.
  for (unsigned i_layer = 0; i_layer != 2; ++i_layer) {
    span<const cf_t> codebook_layer = codebook[i_layer];
    for (unsigned i_port = 0; i_port != 2; ++i_port) {
      result.set_coefficient(codebook_layer[i_port], i_layer, i_port, 0);
    }
  }

  return result;
}