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

#include "srsran/ran/srs/srs_information.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"
#include "srsran/ran/srs/srs_resource_configuration.h"
#include "srsran/support/srsran_assert.h"
#include <cmath>

using namespace srsran;

static constexpr unsigned N_RB_SC = 12;

static constexpr unsigned get_sequence_length(unsigned m_srs_b, srs_resource_configuration::comb_size_enum comb_size)
{
  return (m_srs_b * N_RB_SC) / static_cast<unsigned>(comb_size);
}

srs_information
srsran::get_srs_information(const srs_resource_configuration& resource, unsigned i_antenna_port, unsigned i_symbol)
{
  // Select BW configuration.
  optional<srs_configuration> srs_bw_config =
      srs_configuration_get(resource.configuration_index.value(), resource.bandwidth_index.value());
  srsran_assert(srs_bw_config.has_value(),
                "Invalid combination of c-SRS (i.e., {}) and b-SRS (i.e., {})",
                resource.configuration_index,
                resource.bandwidth_index);

  // Assert configuration parameters.
  srsran_assert(resource.hopping == srs_resource_configuration::group_or_sequence_hopping_enum::neither,
                "No sequence nor group hopping supported");
  srsran_assert(resource.freq_hopping >= resource.bandwidth_index, "Frequency hopping is not supported.");

  // Calculate sequence length.
  unsigned sequence_length = get_sequence_length(srs_bw_config->m_srs, resource.comb_size);

  // Extract comb size.
  unsigned comb_size = static_cast<unsigned>(resource.comb_size);

  // Calculate sequence group.
  unsigned f_gh = 0;
  unsigned u    = (f_gh + resource.sequence_id.value()) % 30;

  // Calculate sequence number.
  unsigned v = 0;

  // Maximum number of cyclic shifts depending on the comb size.
  unsigned n_cs_max = (resource.comb_size == srs_resource_configuration::comb_size_enum::four) ? 12 : 8;

  // Calculate cyclic shift. Note that n_cs_max is always multiple of the number of antenna ports (one, two or four).
  unsigned cyclic_shift_port = (resource.cyclic_shift.value() +
                                (n_cs_max * i_antenna_port) / static_cast<unsigned>(resource.nof_antenna_ports)) %
                               n_cs_max;

  // Calculate alpha.
  float alpha = 2 * M_PI * static_cast<float>(cyclic_shift_port) / static_cast<float>(n_cs_max);

  // Calculate initial subcarrier index.
  unsigned k_tc = resource.comb_offset.value();
  if ((resource.cyclic_shift >= n_cs_max / 2) && (resource.cyclic_shift < n_cs_max) &&
      (resource.nof_antenna_ports == srs_resource_configuration::one_two_four_enum::four) &&
      ((i_antenna_port == 1) || (i_antenna_port == 3))) {
    k_tc = (k_tc + comb_size / 2) % comb_size;
  }
  unsigned k0_bar = resource.freq_shift.value() * N_RB_SC + k_tc;
  unsigned sum    = 0;
  for (unsigned b = 0; b <= resource.bandwidth_index; ++b) {
    optional<srs_configuration> bw_config = srs_configuration_get(resource.configuration_index.value(), b);
    srsran_assert(bw_config, "Invalid configuration.");

    unsigned M_srs = get_sequence_length(bw_config->m_srs, resource.comb_size);
    unsigned n_b   = ((4 * resource.freq_position.value()) / bw_config->m_srs) % bw_config->N;
    sum += comb_size * M_srs * n_b;
  }
  unsigned initial_subcarrier = k0_bar + sum;

  // Fill derived parameters.
  srs_information info;
  info.sequence_length            = sequence_length;
  info.sequence_group             = u;
  info.sequence_number            = v;
  info.alpha                      = alpha;
  info.mapping_initial_subcarrier = initial_subcarrier;
  info.comb_size                  = comb_size;
  return info;
}