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

#include "srsran/ran/pdsch/pdsch_antenna_ports_mapping.h"
#include <array>

using namespace srsran;

// The following entries are taken from Table 7.3.1.2.2-1, TS 38.212, which is applicable when dmrs-Type=1, maxLength=1.
static const uint32_t antenna_port_mapping_dmrs_type1_max_length1_table_entries = 12;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type1_max_length1_table_entries>
    antenna_port_mapping_dmrs_type1_max_length1 = {{
        // clang-format off
        {1, {0}, 1},
        {1, {1}, 1},
        {1, {0, 1}, 1},
        {2, {0}, 1},
        {2, {1}, 1},
        {2, {2}, 1},
        {2, {3}, 1},
        {2, {0, 1}, 1},
        {2, {2, 3}, 1},
        {2, {0, 1, 2}, 1},
        {2, {0, 1, 2, 3}, 1},
        {2, {0, 2}, 1},
        // clang-format on
    }};

// The following entries are taken from Table 7.3.1.2.2-2, TS 38.212, which is applicable when dmrs-Type=1, maxLength=2
// and only codeword=0 is enabled.
static const uint32_t antenna_port_mapping_dmrs_type1_max_length2_one_cw_table_entries = 31;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type1_max_length2_one_cw_table_entries>
    antenna_port_mapping_dmrs_type1_max_length2_one_cw = {{
        // clang-format off
        {1, {0}, 1},
        {1, {1}, 1},
        {1, {0, 1}, 1},
        {2, {0}, 1},
        {2, {1}, 1},
        {2, {2}, 1},
        {2, {3}, 1},
        {2, {0, 1}, 1},
        {2, {2, 3}, 1},
        {2, {0, 1, 2}, 1},
        {2, {0, 1, 2, 3}, 1},
        {2, {0, 2}, 1},
        {2, {0}, 2},
        {2, {1}, 2},
        {2, {2}, 2},
        {2, {3}, 2},
        {2, {4}, 2},
        {2, {5}, 2},
        {2, {6}, 2},
        {2, {7}, 2},
        {2, {0, 1}, 2},
        {2, {2, 3}, 2},
        {2, {4, 5}, 2},
        {2, {6, 7}, 2},
        {2, {0, 4}, 2},
        {2, {2, 6}, 2},
        {2, {0, 1, 4}, 2},
        {2, {2, 3, 6}, 2},
        {2, {0, 1,4, 5}, 2},
        {2, {2, 3,6, 7}, 2},
        {2, {0, 2,4, 6}, 2},
        // clang-format on
    }};

// The following entries are taken from Table 7.3.1.2.2-2, TS 38.212, which is applicable when dmrs-Type=1, maxLength=2
// and both codeword=0 and codeword=1 are enabled.
static const uint32_t antenna_port_mapping_dmrs_type1_max_length2_two_cw_table_entries = 4;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type1_max_length2_two_cw_table_entries>
    antenna_port_mapping_dmrs_type1_max_length2_two_cw = {{
        // clang-format off
        {2, {0, 1, 2, 3, 4}, 2},
        {2, {0, 1, 2, 3, 4, 6}, 2},
        {2, {0, 1, 2, 3, 4, 5, 6}, 2},
        {2, {0, 1, 2, 3, 4, 5, 6, 7}, 2},
        // clang-format on
    }};

// The following entries are taken from Table 7.3.1.2.2-3, TS 38.212, which is applicable when dmrs-Type=2, maxLength=1
// and only codeword=0 is enabled.
static const uint32_t antenna_port_mapping_dmrs_type2_max_length1_one_cw_table_entries = 24;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length1_one_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length1_one_cw = {{
        // clang-format off
        {1, {0}, 1},
        {1, {1}, 1},
        {1, {0, 1}, 1},
        {2, {0}, 1},
        {2, {1}, 1},
        {2, {2}, 1},
        {2, {3}, 1},
        {2, {0, 1}, 1},
        {2, {2, 3}, 1},
        {2, {0, 1, 2}, 1},
        {2, {0, 1, 2, 3}, 1},
        {3, {0}, 1},
        {3, {1}, 1},
        {3, {2}, 1},
        {3, {3}, 1},
        {3, {4}, 1},
        {3, {5}, 1},
        {3, {0, 1}, 1},
        {3, {2, 3}, 1},
        {3, {4, 5}, 1},
        {3, {0, 1, 2}, 1},
        {3, {3, 4, 5}, 1},
        {3, {0, 1, 2, 3}, 1},
        {2, {0, 2}, 1},
        // clang-format on
    }};

// The following entries are taken from Table 7.3.1.2.2-3, TS 38.212, which is applicable when dmrs-Type=2, maxLength=1
// and both codeword=0 and codeword=1 are enabled.
static const uint32_t antenna_port_mapping_dmrs_type2_max_length1_two_cw_table_entries = 2;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length1_two_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length1_two_cw = {{
        // clang-format off
        {3, {0, 1, 2, 3, 4}, 1},
        {3, {0, 1, 2, 3, 4, 5}, 1},
        // clang-format on
    }};

// The following entries are taken from Table 7.3.1.2.2-4, TS 38.212, which is applicable when dmrs-Type=2, maxLength=2
// and only codeword=0 is enabled.
static const uint32_t antenna_port_mapping_dmrs_type2_max_length2_one_cw_table_entries = 58;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length2_one_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length2_one_cw = {{
        // clang-format off
        {1, {0}, 1},
        {1, {1}, 1},
        {1, {0, 1}, 1},
        {2, {0}, 1},
        {2, {1}, 1},
        {2, {2}, 1},
        {2, {3}, 1},
        {2, {0, 1}, 1},
        {2, {2, 3}, 1},
        {2, {0, 1, 2}, 1},
        {2, {0, 1, 2, 3}, 1},
        {3, {0}, 1},
        {3, {1}, 1},
        {3, {2}, 1},
        {3, {3}, 1},
        {3, {4}, 1},
        {3, {5}, 1},
        {3, {0, 1}, 1},
        {3, {2, 3}, 1},
        {3, {4, 5}, 1},
        {3, {0, 1, 2}, 1},
        {3, {3, 4, 5}, 1},
        {3, {0, 1, 2, 3}, 1},
        {2, {0, 2}, 1},
        {3, {0}, 2},
        {3, {1}, 2},
        {3, {2}, 2},
        {3, {3}, 2},
        {3, {4}, 2},
        {3, {5}, 2},
        {3, {6}, 2},
        {3, {7}, 2},
        {3, {8}, 2},
        {3, {9}, 2},
        {3, {10}, 2},
        {3, {11}, 2},
        {3, {0, 1}, 2},
        {3, {2, 3}, 2},
        {3, {4, 5}, 2},
        {3, {6, 7}, 2},
        {3, {8, 9}, 2},
        {3, {10, 11}, 2},
        {3, {0, 1, 6}, 2},
        {3, {2, 3, 8}, 2},
        {3, {4, 5, 10}, 2},
        {3, {0, 1, 6, 7}, 2},
        {3, {2, 3, 8, 9}, 2},
        {3, {4, 5, 10, 11}, 2},
        {1, {0}, 2},
        {1, {1}, 2},
        {1, {6}, 2},
        {1, {7}, 2},
        {1, {0, 1}, 2},
        {1, {6, 7}, 2},
        {2, {0, 1}, 2},
        {2, {2, 3}, 2},
        {2, {6, 7}, 2},
        {2, {8, 9}, 2},
        // clang-format on
    }};

// The following entries are taken from Table 7.3.1.2.2-4, TS 38.212, which is applicable when dmrs-Type=2, maxLength=2
// and both codeword=0 and codeword=1 are enabled.
static const uint32_t antenna_port_mapping_dmrs_type2_max_length2_two_cw_table_entries = 6;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length2_two_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length2_two_cw = {{
        // clang-format off
        {3, {0, 1, 2, 3, 4}, 1},
        {3, {0, 1, 2, 3, 4, 5}, 1},
        {2, {0, 1, 2, 3, 6}, 2},
        {2, {0, 1, 2, 3, 6, 8}, 2},
        {2, {0, 1, 2, 3, 6, 7,8}, 2},
        {2, {0, 1, 2, 3, 6, 7,8, 9}, 2},
        // clang-format on
    }};

namespace {
// Helper function to fetch the PDSCH antenna ports mapping table to use based on input configuration.
span<const pdsch_antenna_ports_mapping> get_pdsch_antenna_port_mapping_table(dmrs_config_type dmrs_cfg_type,
                                                                             dmrs_max_length  dmrs_max_len,
                                                                             bool             are_both_cws_enabled)
{
  if (dmrs_cfg_type == dmrs_config_type::type1 and dmrs_max_len == dmrs_max_length::len1) {
    return antenna_port_mapping_dmrs_type1_max_length1;
  }
  if (dmrs_cfg_type == dmrs_config_type::type1 and dmrs_max_len == dmrs_max_length::len2 and not are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type1_max_length2_one_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type1 and dmrs_max_len == dmrs_max_length::len2 and are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type1_max_length2_two_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len1 and not are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length1_one_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len1 and are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length1_two_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len2 and not are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length2_one_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len2 and are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length2_two_cw;
  }
  report_fatal_error("No PDSCH antenna ports mapping found for the given DMRS configuration.");
}

// Helper function to fetch a row from the relevant PDSCH antenna ports mapping table based on input configuration.
const pdsch_antenna_ports_mapping* get_pdsch_antenna_port_mapping_row(unsigned         nof_layers,
                                                                      unsigned         nof_dl_antenna_ports,
                                                                      dmrs_config_type dmrs_cfg_type,
                                                                      dmrs_max_length  dmrs_max_len,
                                                                      bool             are_both_cws_enabled)
{
  srsran_assert(nof_layers <= nof_dl_antenna_ports,
                "Number of DL layers={} cannot be greater than number of DL antenna ports={} of the cell.",
                nof_layers,
                nof_dl_antenna_ports);
  const auto  mapping_table = get_pdsch_antenna_port_mapping_table(dmrs_cfg_type, dmrs_max_len, are_both_cws_enabled);
  const auto* it            = std::find_if(mapping_table.begin(),
                                mapping_table.end(),
                                [nof_layers, nof_dl_antenna_ports](const pdsch_antenna_ports_mapping& mapping) {
                                  return (mapping.dmrs_ports.size() == nof_layers) and
                                         (*std::max_element(mapping.dmrs_ports.cbegin(), mapping.dmrs_ports.cend()) <
                                          nof_dl_antenna_ports);
                                });
  if (it == mapping_table.end()) {
    srsran_assertion_failure("No matching PDSCH antenna ports mapping found in TS 38.212, tables 7.3.1.2.2-1/2/3/4");
  }
  return it;
}
} // namespace

pdsch_antenna_ports_mapping srsran::get_pdsch_antenna_port_mapping(unsigned         nof_layers,
                                                                   unsigned         nof_dl_antenna_ports,
                                                                   dmrs_config_type dmrs_cfg_type,
                                                                   dmrs_max_length  dmrs_max_len,
                                                                   bool             are_both_cws_enabled)
{
  return *get_pdsch_antenna_port_mapping_row(
      nof_layers, nof_dl_antenna_ports, dmrs_cfg_type, dmrs_max_len, are_both_cws_enabled);
}

unsigned srsran::get_pdsch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                          unsigned         nof_dl_antenna_ports,
                                                          dmrs_config_type dmrs_cfg_type,
                                                          dmrs_max_length  dmrs_max_len,
                                                          bool             are_both_cws_enabled)
{
  const auto mapping_table = get_pdsch_antenna_port_mapping_table(dmrs_cfg_type, dmrs_max_len, are_both_cws_enabled);
  return std::distance(mapping_table.begin(),
                       get_pdsch_antenna_port_mapping_row(
                           nof_layers, nof_dl_antenna_ports, dmrs_cfg_type, dmrs_max_len, are_both_cws_enabled));
}
