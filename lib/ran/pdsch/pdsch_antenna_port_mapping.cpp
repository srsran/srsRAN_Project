/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/pdsch/pdsch_antenna_ports_mapping.h"
#include <array>

using namespace srsran;

// The following entries are taken from Table 7.3.1.2.2-1, TS 38.212, which is applicable when dmrs-Type=1, maxLength=1.
static const uint32_t antenna_port_mapping_dmrs_type1_max_length1_table_entries = 12;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type1_max_length1_table_entries>
    antenna_port_mapping_dmrs_type1_max_length1 = {{
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
    }};

// TODO: Add tables from TS 38.212, tables 7.3.1.2.2-2/3/4.

static span<const pdsch_antenna_ports_mapping> get_pdsch_antenna_port_mapping_table(dmrs_config_type dmrs_cfg_type,
                                                                                    dmrs_max_length  dmrs_max_len)
{
  if (dmrs_cfg_type == srsran::dmrs_config_type::type2 or dmrs_max_len == srsran::dmrs_max_length::len2) {
    srsran_assertion_failure(
        "PDSCH antenna ports mapping for the given DMRS configuration is currently not supported.");
  }
  return antenna_port_mapping_dmrs_type1_max_length1;
}

static const pdsch_antenna_ports_mapping* get_pdsch_antenna_port_mapping_row(unsigned         nof_layers,
                                                                             unsigned         nof_dl_antenna_ports,
                                                                             dmrs_config_type dmrs_cfg_type,
                                                                             dmrs_max_length  dmrs_max_len)
{
  if (nof_layers > nof_dl_antenna_ports) {
    srsran_assertion_failure("Number of DL layers={} cannot be greater than number of DL antenna ports={} of the cell.",
                             nof_layers,
                             nof_dl_antenna_ports);
  }
  const auto  mapping_table = get_pdsch_antenna_port_mapping_table(dmrs_cfg_type, dmrs_max_len);
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

pdsch_antenna_ports_mapping srsran::get_pdsch_antenna_port_mapping(unsigned         nof_layers,
                                                                   unsigned         nof_dl_antenna_ports,
                                                                   dmrs_config_type dmrs_cfg_type,
                                                                   dmrs_max_length  dmrs_max_len)
{
  return *get_pdsch_antenna_port_mapping_row(nof_layers, nof_dl_antenna_ports, dmrs_cfg_type, dmrs_max_len);
}

unsigned srsran::get_pdsch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                          unsigned         nof_dl_antenna_ports,
                                                          dmrs_config_type dmrs_cfg_type,
                                                          dmrs_max_length  dmrs_max_len)
{
  const auto mapping_table = get_pdsch_antenna_port_mapping_table(dmrs_cfg_type, dmrs_max_len);
  return std::distance(
      mapping_table.begin(),
      get_pdsch_antenna_port_mapping_row(nof_layers, nof_dl_antenna_ports, dmrs_cfg_type, dmrs_max_len));
}
