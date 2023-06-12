/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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