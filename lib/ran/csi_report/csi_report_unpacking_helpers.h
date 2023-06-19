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

#pragma once
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_unpacking.h"
#include "srsran/support/units.h"

namespace srsran {

/// Collects the RI, LI, wideband CQI, and CSI fields bit-width.
struct ri_li_cqi_cri_sizes {
  unsigned ri;
  unsigned li;
  unsigned cqi_first_tb;
  unsigned cqi_second_tb;
  unsigned subband_diff_cqi_first_tb;
  unsigned subband_diff_cqi_second_tb;
  unsigned cri;
};

/// Gets the number of CSI-RS antenna ports from the PMI codebook type.
inline unsigned csi_report_get_nof_csi_rs_antenna_ports(pmi_codebook_type pmi_codebook)
{
  switch (pmi_codebook) {
    case pmi_codebook_type::one:
      return 1;
    case pmi_codebook_type::two:
      return 2;
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return 4;
    case pmi_codebook_type::other:
    default:
      return 0;
  }
}

/// Gets the CSI report size for CRI, RI, LI, PMI and CQI quantities.
units::bits get_csi_report_size_cri_ri_li_pmi_cqi(const csi_report_configuration& config, csi_report_data::ri_type ri);

/// Unpacks a CSI report for CRI, RI, LI, PMI and CQI quantities.
csi_report_data csi_report_unpack_pucch_cri_ri_li_pmi_cqi(const csi_report_packed&        packed,
                                                          const csi_report_configuration& config);

} // namespace srsran