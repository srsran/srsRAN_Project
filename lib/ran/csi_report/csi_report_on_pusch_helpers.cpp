/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "csi_report_on_puxch_helpers.h"

using namespace srsran;

csi_report_pusch_size srsran::get_csi_report_pusch_size(const csi_report_configuration& config)
{
  csi_report_pusch_size result                = {};
  unsigned              nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook);

  // Get CSI Part 1 field sizes which do not depend on the number of layers.
  ri_li_cqi_cri_sizes part1_sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1, config.nof_csi_rs_resources);

  // Calculate CSI Part 1 size following TS38.212 Table 6.3.2.1.2-3.
  unsigned part1_size = 0;

  // CRI, RI and wideband CQI for the first TB are always present and their size does not depend on the RI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    part1_size += part1_sizes.cri;
    part1_size += part1_sizes.ri;
    part1_size += part1_sizes.wideband_cqi_first_tb;
  }

  result.part1_size = units::bits{part1_size};

  // Skip CSI Part 2 if there is one transmit port and no quantity is reported in CSI Part 2.
  if ((nof_csi_antenna_ports == 1) || ((config.quantities != csi_report_quantities::cri_ri_li_pmi_cqi) &&
                                       (config.quantities != csi_report_quantities::cri_ri_pmi_cqi))) {
    return result;
  }

  // Prepare CSI Part 2 entry.
  uci_part2_size_description::entry& entry = result.part2_correspondence.entries.emplace_back();

  // Prepare RI field as parameter.
  uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
  parameter.offset                                 = part1_sizes.cri;
  parameter.width                                  = part1_sizes.ri;

  // Fill the entry table in function of the RI.
  for (unsigned i_nof_layers = 0; i_nof_layers != nof_csi_antenna_ports; ++i_nof_layers) {
    // Get CSI Part2 field sizes which depend on the number of layers.
    ri_li_cqi_cri_sizes part2_sizes = get_ri_li_cqi_cri_sizes(
        config.pmi_codebook, config.ri_restriction, i_nof_layers + 1, config.nof_csi_rs_resources);

    // Calculate CSI Part 2 size following TS38.212 Table 6.3.2.1.2-4.
    unsigned part2_size = 0;

    // Wideband CQI for the second TB.
    if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
        (config.quantities == csi_report_quantities::cri_ri_cqi) ||
        (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
      part2_size += part2_sizes.wideband_cqi_second_tb;
    }

    // LI.
    if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
      part2_size += part2_sizes.li;
    }

    // PMI.
    if ((config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) ||
        (config.quantities == csi_report_quantities::cri_ri_pmi_cqi)) {
      part2_size += csi_report_get_size_pmi(config.pmi_codebook, i_nof_layers + 1);
    }

    entry.map.emplace_back(part2_size);
  }

  result.part2_min_size = units::bits(*std::min_element(entry.map.begin(), entry.map.end()));
  result.part2_max_size = units::bits(*std::max_element(entry.map.begin(), entry.map.end()));

  return result;
}