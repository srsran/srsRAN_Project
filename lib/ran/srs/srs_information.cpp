/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

static constexpr unsigned get_sequence_length(unsigned m_srs_b, unsigned comb_size)
{
  srsran_assert((comb_size == 2) || (comb_size == 4), "Invalid comb size (i.e., )", comb_size);
  return (m_srs_b * N_RB_SC) / comb_size;
}

srs_information
srsran::get_srs_information(const srs_resource_configuration& resource, unsigned i_antenna_port, unsigned i_symbol)
{
  // Select BW configuration.
  optional<srs_configuration> srs_bw_config =
      srs_configuration_get(resource.configuration_index, resource.bandwidth_index);
  srsran_assert(srs_bw_config.has_value(),
                "Invalid combination of c-SRS (i.e., {}) and b-SRS (i.e., {})",
                resource.configuration_index,
                resource.bandwidth_index);

  // Assert configuration parameters.
  srsran_assert(
      (resource.comb_size == 2) || (resource.comb_size == 4), "Invalid comb size (i.e., {})", resource.comb_size);
  srsran_assert(resource.hopping == srs_resource_configuration::group_or_sequence_hopping_enum::neither,
                "No sequence nor group hopping supported");
  srsran_assert(resource.freq_hopping >= resource.bandwidth_index, "Frequency hopping is not supported.");

  // Calculate sequence length.
  unsigned sequence_length = get_sequence_length(srs_bw_config->m_srs, resource.comb_size);

  // Calculate sequence group.
  unsigned f_gh = 0;
  unsigned u    = (f_gh + resource.sequence_id) % 30;

  // Calculate sequence number.
  unsigned v = 0;

  // Maximum number of cyclic shifts depending on the comb size.
  unsigned n_cs_max = (resource.comb_size == 4) ? 12 : 8;

  // Calculate cyclic shift. Note that n_cs_max is always multiple of the number of antenna ports (one, two or four).
  unsigned cyclic_shift_port =
      (resource.cyclic_shift + (n_cs_max * i_antenna_port) / static_cast<unsigned>(resource.nof_antenna_ports)) %
      n_cs_max;

  // Calculate alpha.
  float alpha = 2 * M_PI * static_cast<float>(cyclic_shift_port) / static_cast<float>(n_cs_max);

  // Calculate initial subcarrier index.
  unsigned k_tc = resource.comb_offset;
  if ((resource.cyclic_shift >= n_cs_max / 2) && (resource.cyclic_shift < n_cs_max) &&
      (resource.nof_antenna_ports == srs_resource_configuration::one_two_four_enum::four) &&
      ((i_antenna_port == 1) || (i_antenna_port == 3))) {
    k_tc = (resource.comb_offset + resource.comb_size / 2) % resource.comb_size;
  }
  unsigned k0_bar = resource.freq_shift * N_RB_SC + k_tc;
  unsigned sum    = 0;
  for (unsigned b = 0; b <= resource.bandwidth_index; ++b) {
    optional<srs_configuration> bw_config = srs_configuration_get(resource.configuration_index, b);
    srsran_assert(bw_config, "Invalid configuration.");

    unsigned M_srs = get_sequence_length(bw_config->m_srs, resource.comb_size);
    unsigned n_b   = ((4 * resource.freq_position) / bw_config->m_srs) % bw_config->N;
    sum += resource.comb_size * M_srs * n_b;
  }
  unsigned initial_subcarrier = k0_bar + sum;

  // Fill derived parameters.
  srs_information info;
  info.sequence_length            = sequence_length;
  info.sequence_group             = u;
  info.sequence_number            = v;
  info.alpha                      = alpha;
  info.mapping_initial_subcarrier = initial_subcarrier;
  info.comb_size                  = resource.comb_size;
  return info;
}