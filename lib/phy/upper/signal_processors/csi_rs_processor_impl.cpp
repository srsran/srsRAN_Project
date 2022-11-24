/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "csi_rs_processor_impl.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"

using namespace srsgnb;

const std::array<const csi_rs_processor_impl::cdm_sequence, 2> csi_rs_processor_impl::fd_cdm2_table = {{
    {{+1.0f, +1.0f}, {0, 0, 0, 0}},
    {{+1.0f, -1.0f}, {0, 0, 0, 0}},
}};

const std::array<const csi_rs_processor_impl::cdm_sequence, 4> csi_rs_processor_impl::cdm4_fd2_td2_table = {{
    {{+1.0f, +1.0f}, {+1.0f, +1.0f, 0, 0}},
    {{+1.0f, -1.0f}, {+1.0f, +1.0f, 0, 0}},
    {{+1.0f, +1.0f}, {+1.0f, -1.0f, 0, 0}},
    {{+1.0f, -1.0f}, {+1.0f, -1.0f, 0, 0}},
}};

const std::array<const csi_rs_processor_impl::cdm_sequence, 8> csi_rs_processor_impl::cdm8_fd2_td4_table = {{
    {{+1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}},
    {{+1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f, -1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, -1.0f, +1.0f, -1.0f}},
    {{+1.0f, +1.0f}, {+1.0f, +1.0f, -1.0f, -1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, +1.0f, -1.0f, -1.0f}},
    {{+1.0f, +1.0f}, {+1.0f, -1.0f, -1.0f, +1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f, +1.0f}},
}};

// Computes the CDM group size.
static constexpr unsigned get_cdm_group_size(const csi_rs_cdm_type cdm)
{
  // Determine the CDM group size.
  unsigned cdm_group_size = 1;
  if (cdm == csi_rs_cdm_type::FD_CDM2) {
    cdm_group_size = 2;
  } else if (cdm == csi_rs_cdm_type::CDM4_FD2_TD2) {
    cdm_group_size = 4;
  } else if (cdm == csi_rs_cdm_type::CDM8_FD2_TD4) {
    cdm_group_size = 8;
  }
  return cdm_group_size;
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row one.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_1(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 1;
  constexpr unsigned NOF_K_REF = 1;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 1: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 1: 1 k_ref is needed");
  srsgnb_assert(k_ref[0] <= 3, "CSI-RS Mapping Row 1: k_0 must be in the range 0..3 for row 1 mapping");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp), "CSI-RS Mapping Row 1: l_0 outside the valid range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::THREE, "CSI-RS Mapping Row 1: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::NO_CDM, "CSI-RS Mapping Row 1: invalid CDM type");

  // Define parameters for the first port.
  k_bar[0] = k_ref[0];
  l_bar[0] = l_0;
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row two.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_2(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 1;
  constexpr unsigned NOF_K_REF = 1;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 2: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 2: 1 k_ref is needed");
  srsgnb_assert(k_ref[0] < NRE, "CSI-RS Mapping Row 2: k_0 outside the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp), "CSI-RS Mapping Row 2: l_0 outside the valid range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE ||
                    config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB ||
                    config.freq_density == csi_rs_freq_density::DOT5_ODD_RB,
                "CSI-RS Mapping Row 2: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::NO_CDM, "CSI-RS Mapping Row 2: invalid CDM type");

  // Define parameters for the first port.
  k_bar[0] = k_ref[0];
  l_bar[0] = l_0;
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row three.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_3(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 2;
  constexpr unsigned NOF_K_REF = 1;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 3: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 3: one k_ref is needed");
  srsgnb_assert(k_ref[0] < NRE - 1, "CSI-RS Mapping Row 3: k_0 outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp), "CSI-RS Mapping Row 3: l_0 outside the valid range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE ||
                    config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB ||
                    config.freq_density == csi_rs_freq_density::DOT5_ODD_RB,
                "CSI-RS Mapping Row 3: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::FD_CDM2, "CSI-RS Mapping Row 3: invalid CDM type");

  // Define parameters for the first port.
  k_bar[0] = k_ref[0];
  l_bar[0] = l_0;

  // Define parameters for the second port.
  k_bar[1] = k_ref[0];
  l_bar[1] = l_0;
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row four.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_4(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 4;
  constexpr unsigned NOF_K_REF = 1;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 4: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 4: 1 k_ref is needed");
  srsgnb_assert(k_ref[0] < NRE - 3, "CSI-RS Mapping Row 4: k_0 outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp), "CSI-RS Mapping Row 4: l_0 outside the valid range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE, "CSI-RS Mapping Row 4: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::FD_CDM2, "CSI-RS Mapping Row 4: invalid CDM type ");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::FD_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[0] + 2 * (port / CDM_SIZE);
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row five.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_5(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 4;
  constexpr unsigned NOF_K_REF = 1;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 5: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 5: 1 k_ref is needed");
  srsgnb_assert(k_ref[0] < NRE - 1, "CSI-RS Mapping Row 5: k_0 outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp) - 1, "CSI-RS Mapping Row 5: l_0 outside the valid range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE, "CSI-RS Mapping Row 5: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::FD_CDM2, "CSI-RS Mapping Row 5: invalid CDM type ");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::FD_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0 + port / CDM_SIZE;
    k_bar[port] = k_ref[0];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row six.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_6(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 8;
  constexpr unsigned NOF_K_REF = 4;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 6: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 6: 4 k_ref are needed");
  srsgnb_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 6: k references outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp), "CSI-RS Mapping Row 6: l_0 outside the valid range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE, "CSI-RS Mapping Row 6: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::FD_CDM2, "CSI-RS Mapping Row 6: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::FD_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row seven.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_7(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 8;
  constexpr unsigned NOF_K_REF = 2;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 7: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 7: 2 k_ref are needed");
  srsgnb_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 7: k references outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp) - 1, "CSI-RS Mapping Row 7: l_0 outside the allowed range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE, "CSI-RS Mapping Row 7: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::FD_CDM2, "CSI-RS Mapping Row 7: invalid CDM type");

  // CDM group size and number of CDM groups.
  constexpr unsigned CDM_SIZE       = get_cdm_group_size(csi_rs_cdm_type::FD_CDM2);
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
static void mapping_row_8(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 8;
  constexpr unsigned NOF_K_REF = 2;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 8: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 8: 2 k_ref are needed");
  srsgnb_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 8: k references outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp) - 1, "CSI-RS Mapping Row 8: l_0 outside the allowed range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE, "CSI-RS Mapping Row 8: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::CDM4_FD2_TD2, "CSI-RS Mapping Row 8: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::CDM4_FD2_TD2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row nine.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_9(span<unsigned>                    k_bar,
                          span<unsigned>                    l_bar,
                          span<const unsigned>              k_ref,
                          const unsigned                    l_0,
                          const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 12;
  constexpr unsigned NOF_K_REF = 6;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 9: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 9: 6 k_ref are needed");
  srsgnb_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 9: k references outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp), "CSI-RS Mapping Row 9: l_0 outside the allowed range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE, "CSI-RS Mapping Row 9: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::FD_CDM2, "CSI-RS Mapping Row 9: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::FD_CDM2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row ten.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_10(span<unsigned>                    k_bar,
                           span<unsigned>                    l_bar,
                           span<const unsigned>              k_ref,
                           const unsigned                    l_0,
                           const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 12;
  constexpr unsigned NOF_K_REF = 3;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 10: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 10: 3 k_ref are needed");
  srsgnb_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 10: k references outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp) - 1, "CSI-RS Mapping Row 10: l_0 outside the allowed range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE, "CSI-RS Mapping Row 10: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::CDM4_FD2_TD2, "CSI-RS Mapping Row 10: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::CDM4_FD2_TD2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the frequency and time locations for TS 38.211, Table 7.4.1.5.3-1 Row eleven.
// Returns the number of ports used for mapping the CSI-RS signal.
static void mapping_row_11(span<unsigned>                    k_bar,
                           span<unsigned>                    l_bar,
                           span<const unsigned>              k_ref,
                           const unsigned                    l_0,
                           const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 16;
  constexpr unsigned NOF_K_REF = 4;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 11: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 11: 4 k_ref are needed");
  srsgnb_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 11: k references outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp) - 1, "CSI-RS Mapping Row 11: l_0 outside the allowed range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE ||
                    config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB ||
                    config.freq_density == csi_rs_freq_density::DOT5_ODD_RB,
                "CSI-RS Mapping Row 11: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::FD_CDM2, "CSI-RS Mapping Row 11: invalid CDM type");

  // CDM group size and number of CDM groups.
  constexpr unsigned CDM_SIZE       = get_cdm_group_size(csi_rs_cdm_type::FD_CDM2);
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
static void mapping_row_12(span<unsigned>                    k_bar,
                           span<unsigned>                    l_bar,
                           span<const unsigned>              k_ref,
                           const unsigned                    l_0,
                           const csi_rs_processor::config_t& config)
{
  // Expected number of ports and frequency references.
  constexpr unsigned NOF_PORTS = 16;
  constexpr unsigned NOF_K_REF = 4;

  srsgnb_assert(config.ports.size() == NOF_PORTS, "CSI-RS Mapping Row 12: invalid number of ports");
  srsgnb_assert(NOF_K_REF == k_ref.size(), "CSI-RS Mapping Row 12: 4 k_ref are needed");
  srsgnb_assert(*std::max_element(k_ref.begin(), k_ref.end()) < NRE - 1,
                "CSI-RS Mapping Row 12: k references outside of the valid range");
  srsgnb_assert(l_0 < get_nsymb_per_slot(config.cp) - 1, "CSI-RS Mapping Row 12: l_0 outside the allowed range.");
  srsgnb_assert(config.freq_density == csi_rs_freq_density::ONE ||
                    config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB ||
                    config.freq_density == csi_rs_freq_density::DOT5_ODD_RB,
                "CSI-RS Mapping Row 12: invalid density");
  srsgnb_assert(config.cdm == csi_rs_cdm_type::CDM4_FD2_TD2, "CSI-RS Mapping Row 12: invalid CDM type");

  // CDM group size.
  constexpr unsigned CDM_SIZE = get_cdm_group_size(csi_rs_cdm_type::CDM4_FD2_TD2);

  // Define parameters for each port.
  for (unsigned port = 0; port != NOF_PORTS; ++port) {
    l_bar[port] = l_0;
    k_bar[port] = k_ref[port / CDM_SIZE];
  }
}

// Generates the RE patterns for each antenna port, provided a CSI-RS configuration
// and the required k_bar and l_bar location references.
static void build_re_patterns(span<re_pattern>                  pattern_list,
                              span<const unsigned>              k_bar,
                              span<const unsigned>              l_bar,
                              const unsigned                    nof_ports,
                              const csi_rs_processor::config_t& config)
{
  for (unsigned port = 0; port != nof_ports; ++port) {
    // Define the PRB range.
    pattern_list[port].rb_begin = config.start_rb;
    pattern_list[port].rb_end   = config.start_rb + config.nof_rb;
    pattern_list[port].re_mask  = bounded_bitset<NRE>(NRE);

    // Skip one of every two PRB when the density is 0.5.
    if (config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB ||
        config.freq_density == csi_rs_freq_density::DOT5_ODD_RB) {
      pattern_list[port].rb_stride = 2;

      // Offset the start of the pattern by 1 PRB if necessary.
      if ((config.start_rb % 2 != 0 && config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB) ||
          (config.start_rb % 2 == 0 && config.freq_density == csi_rs_freq_density::DOT5_ODD_RB)) {
        ++pattern_list[port].rb_begin;
      }
    } else {
      pattern_list[port].rb_stride = 1;
    }

    // Fill the RE mapping pattern, according to the previously calculated
    // k_bar and l_bar values and the CDM type.
    if (config.csi_rs_mapping_table_row == 1) {
      // Define the RE mapping pattern for row 1.
      pattern_list[port].symbols[l_bar[port]] = true;
      pattern_list[port].re_mask.set(k_bar[port]);
      pattern_list[port].re_mask.set(k_bar[port] + 4);
      pattern_list[port].re_mask.set(k_bar[port] + 8);
    } else if (config.cdm == csi_rs_cdm_type::NO_CDM) {
      // Define the RE mapping for other rows with no CDM.
      pattern_list[port].symbols[l_bar[port]] = true;
      pattern_list[port].re_mask.set(k_bar[port]);
    } else if (config.cdm == csi_rs_cdm_type::FD_CDM2) {
      // Define the RE mapping for other rows with FD-CDM2.
      pattern_list[port].symbols[l_bar[port]] = true;
      pattern_list[port].re_mask.set(k_bar[port]);
      pattern_list[port].re_mask.set(k_bar[port] + 1);
    } else if (config.cdm == csi_rs_cdm_type::CDM4_FD2_TD2) {
      // Define the RE mapping for other rows with CDM4-FD2-TD2.
      pattern_list[port].symbols[l_bar[port]]     = true;
      pattern_list[port].symbols[l_bar[port] + 1] = true;
      pattern_list[port].re_mask.set(k_bar[port]);
      pattern_list[port].re_mask.set(k_bar[port] + 1);
    } else {
      // Define the RE mapping for other rows with CDM8-FD2-TD4.
      pattern_list[port].symbols[l_bar[port]]     = true;
      pattern_list[port].symbols[l_bar[port] + 1] = true;
      pattern_list[port].symbols[l_bar[port] + 2] = true;
      pattern_list[port].symbols[l_bar[port] + 3] = true;
      pattern_list[port].re_mask.set(k_bar[port]);
      pattern_list[port].re_mask.set(k_bar[port] + 1);
    }
  }
}

void srsgnb::csi_rs_processor_impl::build_grid_allocation(span<re_pattern> pattern_list, const config_t& config)
{
  const unsigned nof_ports = config.ports.size();
  const unsigned row       = config.csi_rs_mapping_table_row;

  // Arrays to store the k_bar and l_bar symbol and RE positions for each port.
  static_vector<unsigned, MAX_PORTS> k_bar(config.ports.size());
  static_vector<unsigned, MAX_PORTS> l_bar(config.ports.size());

  // Assert that the mapping table row is valid and currently supported.
  srsgnb_assert(row <= 18 && row > 0, "Invalid mapping table row.");
  srsgnb_assert(row <= 12, "Unsupported mapping table row");

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

  // Assert that the provided pattern list has the correct size.
  srsgnb_assert(pattern_list.size() == nof_ports,
                "CSI-RS: The pattern_list size and the configured number of ports do not match.");

  // Generate the RE patterns for each port.
  build_re_patterns(pattern_list, k_bar, l_bar, nof_ports, config);
}

// Calculates the number of sequence elements that must be discarded from the
// pseudo-random number generator for the CSI-RS sequence generation.
static unsigned get_nof_skipped_elements(const csi_rs_processor::config_t& config)
{
  // Calculate the first occupied PRB.
  unsigned first_prb = 0;
  if (config.freq_density == csi_rs_freq_density::ONE || config.freq_density == csi_rs_freq_density::THREE) {
    first_prb = config.start_rb;
  } else if (config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB) {
    first_prb = config.start_rb + config.start_rb % 2;
  } else if (config.freq_density == csi_rs_freq_density::DOT5_ODD_RB) {
    first_prb = config.start_rb + (1 - config.start_rb % 2);
  }

  // Determine the start of the sequence. The sequence elements that correspond to PRB's
  // below the first occupied PRB are skipped.
  unsigned nof_advance = 0;
  if (config.freq_density == csi_rs_freq_density::THREE) {
    nof_advance = 3 * first_prb;
  } else if (config.freq_density == csi_rs_freq_density::ONE) {
    if (config.csi_rs_mapping_table_row == 2) {
      nof_advance = first_prb;
    } else {
      nof_advance = 2 * first_prb;
    }
  } else if (config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB) {
    if (config.csi_rs_mapping_table_row == 2) {
      nof_advance = first_prb / 2;
    } else {
      nof_advance = first_prb;
    }
  } else if (config.freq_density == csi_rs_freq_density::DOT5_ODD_RB) {
    if (config.csi_rs_mapping_table_row == 2) {
      nof_advance = first_prb / 2;
    } else {
      nof_advance = first_prb;
    }
  }
  return nof_advance;
}

void srsgnb::csi_rs_processor_impl::generate_sequence(span<cf_t>         sequence,
                                                      const unsigned int symbol,
                                                      const config_t&    config) const
{
  // Slot index.
  const unsigned nslot = config.slot.slot_index();
  // Number of OFDM symbols per slot.
  const unsigned nsymb = get_nsymb_per_slot(config.cp);
  // Scrambling ID.
  const unsigned n_id = config.scrambling_id;

  // Calculate initial state for the pseudo-random sequence.
  const unsigned c_init = (pow2(10) * (nsymb * nslot + symbol + 1) * (2 * n_id + 1) + n_id) % pow2(31);

  // Initialise the pseudo-random sequence generator.
  prg->init(c_init);

  // Determine the symbol amplitude.
  const float amplitude = M_SQRT1_2 * config.amplitude;

  // Calculate the number of sequence elements to skip from the start.
  const unsigned nof_advance = get_nof_skipped_elements(config);

  // Skip the undesired first elements of the sequence.
  prg->advance(2 * nof_advance);

  // Generate the sequence from the adequate starting point.
  prg->generate(sequence, amplitude);
}

// Computes the CSI-RS sequence length for a single OFDM symbol.
static unsigned get_seq_len(const csi_rs_processor::config_t& config)
{
  // The CSI-RS sequence length varies according to the density.
  unsigned seq_len = config.nof_rb;
  if (config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB ||
      config.freq_density == csi_rs_freq_density::DOT5_ODD_RB) {
    seq_len /= 2;

    // For Densities of 0.5, and with and odd-numbered PRB range, in some cases the number of occupied PRB needs to be
    // rounded up instead of truncated.
    if (config.nof_rb % 2 != 0) {
      if (config.start_rb % 2 != 0) {
        if (config.freq_density == csi_rs_freq_density::DOT5_ODD_RB) {
          ++seq_len;
        }
      } else {
        if (config.freq_density == csi_rs_freq_density::DOT5_EVEN_RB) {
          ++seq_len;
        }
      }
    }
  } else if (config.freq_density == csi_rs_freq_density::THREE) {
    seq_len *= 3;
  }

  // The CSI-RS sequence increases in length if CDM is used.
  if (config.cdm == csi_rs_cdm_type::FD_CDM2 || config.cdm == csi_rs_cdm_type::CDM4_FD2_TD2 ||
      config.cdm == csi_rs_cdm_type::CDM8_FD2_TD4) {
    seq_len *= 2;
  }
  return seq_len;
}

void srsgnb::csi_rs_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  unsigned nof_ports = config.ports.size();

  // Generate the grid allocations patterns for each port.
  static_vector<re_pattern, MAX_PORTS> pattern_list(nof_ports);
  build_grid_allocation(pattern_list, config);

  // Calculate the CSI-RS sequence length for a signel OFDM symbol.
  const unsigned seq_len = get_seq_len(config);

  // Determine the number of symbols in the CDM group.
  const unsigned cdm_group_size = get_cdm_group_size(config.cdm);

  // Stores the NZP-CSI-RS sequence for a single OFDM symbol.
  static_vector<cf_t, MAX_SEQ_LEN> sequence(seq_len);

  // Precoding not currently supported.
  if (config.pmi != 0) {
    srsgnb_assertion_failure("Precoding is not currently supported\n");
  }

  for (unsigned p = 0; p != nof_ports; ++p) {
    // Use the corresponding RE pattern.
    const re_pattern& pattern = pattern_list[p];

    // Get the symbol utilization mask witin the slot.
    span<const bool> symbol_mask(pattern.symbols);

    // Index within the CDM group for the specified port.
    const unsigned cdm_group_idx = p % cdm_group_size;
    // Symbol index within the CDM table.
    unsigned cdm_l_idx = 0;
    for (unsigned l = 0; l != get_nsymb_per_slot(config.cp); ++l) {
      if (symbol_mask[l]) {
        // Create whole OFDM symbol mask.
        bounded_bitset<MAX_RB * NRE> mask(MAX_RB * NRE);
        pattern.get_inclusion_mask(mask, l);

        // Select from the mask the region of interest.
        bounded_bitset<MAX_RB* NRE> mask_csi =
            mask.slice(config.start_rb * NRE, (config.start_rb + config.nof_rb) * NRE);

        // Generate the generic sequence.
        generate_sequence(sequence, l, config);

        // Calculate the CDM sequence and write into the resource grid.
        apply_cdm(sequence, sequence, config.cdm, cdm_group_idx, cdm_l_idx++);
        grid.put(config.ports[p], l, config.start_rb * NRE, mask_csi, sequence);
      }
    }
  }
}

void csi_rs_processor_impl::apply_cdm(span<cf_t>            seq_out,
                                      span<const cf_t>      seq_in,
                                      const csi_rs_cdm_type cdm,
                                      const unsigned        cdm_idx,
                                      const unsigned        l_idx)
{
  // Maximum CDM RE and symbol index values.
  constexpr unsigned       K_PRIME_MAX     = 0b1;
  constexpr unsigned       L_PRIME_MAX_TD2 = 0b1;
  constexpr unsigned       L_PRIME_MAX_TD4 = 0b11;
  span<const cdm_sequence> table;

  const unsigned seq_in_len = seq_in.size();

  // Apply FD-CDM2.
  if (cdm == csi_rs_cdm_type::FD_CDM2) {
    // Get the CDM table.
    table = span<const cdm_sequence>(fd_cdm2_table);

    // CDM frequency index.
    unsigned k_prime = 0;
    // Iterate through the input sequence.
    for (unsigned k = 0; k != seq_in_len; ++k) {
      // Get the required output symbols and apply the CDM tables.
      seq_out[k] = table[cdm_idx].w_f[k_prime++] * seq_in[k];
      // Wrap k_prime.
      k_prime &= K_PRIME_MAX;
    }
  }

  // Apply CDM4-FD2-TD2 or CDM8-FD2-TD4.
  else if (cdm == csi_rs_cdm_type::CDM4_FD2_TD2 || cdm == csi_rs_cdm_type::CDM8_FD2_TD4) {
    if (cdm == csi_rs_cdm_type::CDM4_FD2_TD2) {
      srsgnb_assert(
          l_idx <= L_PRIME_MAX_TD2, "l_idx value: {} outside of range: 0..{} for CDM4-FD2-TD2", l_idx, L_PRIME_MAX_TD2);

      // Get the CDM table.
      table = span<const cdm_sequence>(cdm4_fd2_td2_table);

    } else {
      srsgnb_assert(
          l_idx <= L_PRIME_MAX_TD4, "l_idx value: %d outside of range: 0..%d for CDM8-FD2-TD4", l_idx, L_PRIME_MAX_TD4);

      // Get the CDM table.
      table = span<const cdm_sequence>(cdm8_fd2_td4_table);
    }

    // CDM frequency index.
    unsigned k_prime = 0;
    // Iterate through the input sequence.
    for (unsigned k = 0; k != seq_in_len; ++k) {
      // Get the required output symbols and apply the CDM tables.
      seq_out[k] = table[cdm_idx].w_t[l_idx] * table[cdm_idx].w_f[k_prime++] * seq_in[k];
      // Wrap k_prime.
      k_prime &= K_PRIME_MAX;
    }
  }

  // If no CDM, do nothing.
  else {
    return;
  }
}

std::unique_ptr<srsgnb::csi_rs_processor> srsgnb::create_csi_rs_processor()
{
  return std::make_unique<csi_rs_processor_impl>(create_pseudo_random_generator_sw_factory()->create());
}
