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

#include "srsran/ran/csi_rs/csi_rs_pattern.h"
#include "srsran/adt/span.h"
#include "srsran/ran/csi_rs/csi_rs_config_helpers.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

static constexpr unsigned NRE = 12;

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row one.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_1(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of frequency references.
  constexpr unsigned NOF_K_REF = 1;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 1: 1 k_ref is needed");
  srsran_assert(k_ref[0] <= 3, "CSI-RS Mapping Row 1: k_0 must be in the range 0..3 for row 1 mapping");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::three, "CSI-RS Mapping Row 1: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::no_CDM, "CSI-RS Mapping Row 1: invalid CDM type");

  // Define parameters for the first port.
  k_bar[0] = k_ref[0];
  l_bar[0] = l_0;
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row two.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_2(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of frequency references.
  constexpr unsigned NOF_K_REF = 1;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 2: 1 k_ref is needed");
  srsran_assert(k_ref[0] < NRE, "CSI-RS Mapping Row 2: k_0 outside the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one ||
                    config.freq_density == csi_rs_freq_density_type::dot5_even_RB ||
                    config.freq_density == csi_rs_freq_density_type::dot5_odd_RB,
                "CSI-RS Mapping Row 2: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::no_CDM, "CSI-RS Mapping Row 2: invalid CDM type");

  // Define parameters for the first port.
  k_bar[0] = k_ref[0];
  l_bar[0] = l_0;
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row three.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_3(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of frequency references.
  constexpr unsigned NOF_K_REF = 1;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 3: one k_ref is needed");
  srsran_assert(k_ref[0] < NRE - 1, "CSI-RS Mapping Row 3: k_0 outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one ||
                    config.freq_density == csi_rs_freq_density_type::dot5_even_RB ||
                    config.freq_density == csi_rs_freq_density_type::dot5_odd_RB,
                "CSI-RS Mapping Row 3: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::fd_CDM2, "CSI-RS Mapping Row 3: invalid CDM type");

  // Define parameters for the first port.
  k_bar[0] = k_ref[0];
  l_bar[0] = l_0;

  // Define parameters for the second port.
  k_bar[1] = k_ref[0];
  l_bar[1] = l_0;
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row four.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_4(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 4;
  constexpr unsigned NOF_K_REF = 1;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 4: 1 k_ref is needed");
  srsran_assert(k_ref[0] < NRE - 3, "CSI-RS Mapping Row 4: k_0 outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one, "CSI-RS Mapping Row 4: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::fd_CDM2, "CSI-RS Mapping Row 4: invalid CDM type ");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::fd_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[0] + 2 * (port / CDM_SIZE);
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row five.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_5(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 4;
  constexpr unsigned NOF_K_REF = 1;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 5: 1 k_ref is needed");
  srsran_assert(k_ref[0] < NRE - 1, "CSI-RS Mapping Row 5: k_0 outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one, "CSI-RS Mapping Row 5: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::fd_CDM2, "CSI-RS Mapping Row 5: invalid CDM type ");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::fd_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0 + port / CDM_SIZE;
    k_bar[port] = k_ref[0];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row six.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_6(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 8;
  constexpr unsigned NOF_K_REF = 4;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 6: 4 k_ref are needed");
  srsran_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 6: k references outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one, "CSI-RS Mapping Row 6: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::fd_CDM2, "CSI-RS Mapping Row 6: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::fd_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row seven.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_7(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 8;
  constexpr unsigned NOF_K_REF = 2;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 7: 2 k_ref are needed");
  srsran_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 7: k references outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one, "CSI-RS Mapping Row 7: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::fd_CDM2, "CSI-RS Mapping Row 7: invalid CDM type");

  // CDM group size and number of CDM groups.
  constexpr unsigned CDM_SIZE       = get_cdm_group_size(csi_rs_cdm_type::fd_CDM2);
  constexpr unsigned NOF_CDM_GROUPS = NOF_PORTS / CDM_SIZE;

  // Define parameters for each port.
  for (unsigned cdm_group = 0, port = 0; cdm_group != NOF_CDM_GROUPS; ++cdm_group) {
    for (unsigned cdm_idx = 0; cdm_idx != CDM_SIZE; ++cdm_idx) {
      l_bar[port] = l_0 + cdm_group / 2;
      k_bar[port] = k_ref[cdm_group % 2];
      ++port;
    }
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row eight.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_8(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 8;
  constexpr unsigned NOF_K_REF = 2;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 8: 2 k_ref are needed");
  srsran_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 8: k references outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one, "CSI-RS Mapping Row 8: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::cdm4_FD2_TD2, "CSI-RS Mapping Row 8: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::cdm4_FD2_TD2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row nine.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_9(span<unsigned>                      k_bar,
                          span<unsigned>                      l_bar,
                          span<const unsigned>                k_ref,
                          const unsigned                      l_0,
                          const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 12;
  constexpr unsigned NOF_K_REF = 6;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 9: 6 k_ref are needed");
  srsran_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 9: k references outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one, "CSI-RS Mapping Row 9: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::fd_CDM2, "CSI-RS Mapping Row 9: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::fd_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row ten.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_10(span<unsigned>                      k_bar,
                           span<unsigned>                      l_bar,
                           span<const unsigned>                k_ref,
                           const unsigned                      l_0,
                           const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 12;
  constexpr unsigned NOF_K_REF = 3;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 10: 3 k_ref are needed");
  srsran_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 10: k references outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one, "CSI-RS Mapping Row 10: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::cdm4_FD2_TD2, "CSI-RS Mapping Row 10: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::cdm4_FD2_TD2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row eleven.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_11(span<unsigned>                      k_bar,
                           span<unsigned>                      l_bar,
                           span<const unsigned>                k_ref,
                           const unsigned                      l_0,
                           const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 16;
  constexpr unsigned NOF_K_REF = 4;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 11: 4 k_ref are needed");
  srsran_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 11: k references outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one ||
                    config.freq_density == csi_rs_freq_density_type::dot5_even_RB ||
                    config.freq_density == csi_rs_freq_density_type::dot5_odd_RB,
                "CSI-RS Mapping Row 11: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::fd_CDM2, "CSI-RS Mapping Row 11: invalid CDM type");

  // CDM group size and number of CDM groups.
  constexpr unsigned CDM_SIZE       = get_cdm_group_size(csi_rs_cdm_type::fd_CDM2);
  constexpr unsigned NOF_CDM_GROUPS = NOF_PORTS / CDM_SIZE;

  // Define parameters for each port.
  for (unsigned cdm_group = 0, port = 0; cdm_group != NOF_CDM_GROUPS; ++cdm_group) {
    for (unsigned cdm_idx = 0; cdm_idx != CDM_SIZE; ++cdm_idx) {
      l_bar[port] = l_0 + cdm_group / 4;
      k_bar[port] = k_ref[cdm_group % 4];
      ++port;
    }
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row twelve.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_12(span<unsigned>                      k_bar,
                           span<unsigned>                      l_bar,
                           span<const unsigned>                k_ref,
                           const unsigned                      l_0,
                           const csi_rs_pattern_configuration& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 16;
  constexpr unsigned NOF_K_REF = 4;

  srsran_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 12: 4 k_ref are needed");
  srsran_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 12: k references outside of the valid range");
  srsran_assert(config.freq_density == csi_rs_freq_density_type::one ||
                    config.freq_density == csi_rs_freq_density_type::dot5_even_RB ||
                    config.freq_density == csi_rs_freq_density_type::dot5_odd_RB,
                "CSI-RS Mapping Row 12: invalid density");
  srsran_assert(config.cdm == csi_rs_cdm_type::cdm4_FD2_TD2, "CSI-RS Mapping Row 12: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::cdm4_FD2_TD2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the RE patterns for each antenna port, provided a CSI-RS configuration
// and the required k_bar and l_bar location references.
static csi_rs_pattern build_re_patterns(span<const unsigned>                k_bar,
                                        span<const unsigned>                l_bar,
                                        const unsigned                      nof_ports,
                                        const csi_rs_pattern_configuration& config)
{
  csi_rs_pattern pattern;

  // Define the PRB range.
  pattern.rb_begin = config.start_rb;
  pattern.rb_end   = config.start_rb + config.nof_rb;

  // Skip one of every two PRB when the density is 0.5.
  if (config.freq_density == csi_rs_freq_density_type::dot5_even_RB ||
      config.freq_density == csi_rs_freq_density_type::dot5_odd_RB) {
    pattern.rb_stride = 2;

    // Offset the start of the pattern by 1 PRB if necessary.
    if ((config.start_rb % 2 != 0 && config.freq_density == csi_rs_freq_density_type::dot5_even_RB) ||
        (config.start_rb % 2 == 0 && config.freq_density == csi_rs_freq_density_type::dot5_odd_RB)) {
      ++pattern.rb_begin;
    }
  } else {
    pattern.rb_stride = 1;
  }

  for (unsigned port = 0; port != nof_ports; ++port) {
    csi_rs_pattern_port port_pattern = {bounded_bitset<12>(12), bounded_bitset<14>(14)};

    // Fill the RE mapping pattern, according to the previously calculated
    // k_bar and l_bar values and the CDM type.
    if (config.csi_rs_mapping_table_row == 1) {
      // Define the RE mapping pattern for row 1.
      port_pattern.symbol_mask.set(l_bar[port]);
      port_pattern.re_mask.set(k_bar[port]);
      port_pattern.re_mask.set(k_bar[port] + 4);
      port_pattern.re_mask.set(k_bar[port] + 8);
    } else if (config.cdm == csi_rs_cdm_type::no_CDM) {
      // Define the RE mapping for other rows with no CDM.
      port_pattern.symbol_mask.set(l_bar[port]);
      port_pattern.re_mask.set(k_bar[port]);
    } else if (config.cdm == csi_rs_cdm_type::fd_CDM2) {
      // Define the RE mapping for other rows with FD-CDM2.
      port_pattern.symbol_mask.set(l_bar[port]);
      port_pattern.re_mask.set(k_bar[port]);
      port_pattern.re_mask.set(k_bar[port] + 1);
    } else if (config.cdm == csi_rs_cdm_type::cdm4_FD2_TD2) {
      // Define the RE mapping for other rows with CDM4-FD2-TD2.
      port_pattern.symbol_mask.set(l_bar[port]);
      port_pattern.symbol_mask.set(l_bar[port] + 1);
      port_pattern.re_mask.set(k_bar[port]);
      port_pattern.re_mask.set(k_bar[port] + 1);
    } else {
      // Define the RE mapping for other rows with CDM8-FD2-TD4.
      port_pattern.symbol_mask.set(l_bar[port]);
      port_pattern.symbol_mask.set(l_bar[port] + 1);
      port_pattern.symbol_mask.set(l_bar[port] + 2);
      port_pattern.symbol_mask.set(l_bar[port] + 3);
      port_pattern.re_mask.set(k_bar[port]);
      port_pattern.re_mask.set(k_bar[port] + 1);
    }

    pattern.prb_patterns.emplace_back(port_pattern);
  }

  return pattern;
}

csi_rs_pattern srsran::get_csi_rs_pattern(const csi_rs_pattern_configuration& config)
{
  const unsigned row       = config.csi_rs_mapping_table_row;
  const unsigned nof_ports = csi_rs::get_nof_csi_rs_ports(row);

  // Arrays to store the k_bar and l_bar symbol and RE positions for each port.
  static_vector<unsigned, CSI_RS_MAX_NOF_PORTS> k_bar(nof_ports);
  static_vector<unsigned, CSI_RS_MAX_NOF_PORTS> l_bar(nof_ports);

  // Assert that the mapping table row is valid and currently supported.
  srsran_assert(row <= 18 && row > 0, "Invalid mapping table row.");
  srsran_assert(row <= 12, "Unsupported mapping table row");

  // Frequency and symbol allocation references from the config.
  span<const unsigned> k_ref(config.freq_allocation_ref_idx.begin(), config.freq_allocation_ref_idx.end());
  const unsigned       l_0 = config.symbol_l0;

  // Calculate the RE mapping positions for each port.
  switch (row) {
    case 1:
      mapping_row_1(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 2:
      mapping_row_2(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 3:
      mapping_row_3(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 4:
      mapping_row_4(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 5:
      mapping_row_5(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 6:
      mapping_row_6(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 7:
      mapping_row_7(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 8:
      mapping_row_8(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 9:
      mapping_row_9(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 10:
      mapping_row_10(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 11:
      mapping_row_11(k_bar, l_bar, k_ref, l_0, config);
      break;
    case 12:
      mapping_row_12(k_bar, l_bar, k_ref, l_0, config);
      break;
    default:
      break;
  }

  // Generate the RE patterns for each port.
  return build_re_patterns(k_bar, l_bar, nof_ports, config);
}