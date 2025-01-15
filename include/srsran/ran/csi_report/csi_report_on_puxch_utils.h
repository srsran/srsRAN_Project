/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/csi_report/csi_report_configuration.h"

namespace srsran {

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

} // namespace srsran
