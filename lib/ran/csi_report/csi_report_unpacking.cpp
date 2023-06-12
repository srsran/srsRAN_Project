/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/csi_report/csi_report_unpacking.h"
#include "csi_report_unpacking_helpers.h"
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

/// Gets the CSI report size in function of the RI.
static units::bits get_csi_report_size_ri(const csi_report_configuration& config, csi_report_data::ri_type ri)
{
  switch (config.quantities) {
    case csi_report_quantities::cri_ri_pmi_cqi:
    case csi_report_quantities::cri_ri_cqi:
    case csi_report_quantities::cri_ri_li_pmi_cqi:
      return get_csi_report_size_cri_ri_li_pmi_cqi(config, ri);
    case csi_report_quantities::other:
    default:
      return units::bits{0};
  }
}

units::bits srsran::get_csi_report_pucch_size(const csi_report_configuration& config)
{
  units::bits csi_report_size(0);
  unsigned    nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook);

  // For each possible RI, find the largest CSI report size.
  for (unsigned ri = 1, ri_end = nof_csi_antenna_ports + 1; ri != ri_end; ++ri) {
    csi_report_size = std::max(csi_report_size, get_csi_report_size_ri(config, ri));
  }

  return csi_report_size;
}

csi_report_data srsran::csi_report_unpack_pucch(const csi_report_packed& packed, const csi_report_configuration& config)
{
  // Select unpacking depending on the CSI report quantities.
  switch (config.quantities) {
    case csi_report_quantities::cri_ri_pmi_cqi:
    case csi_report_quantities::cri_ri_cqi:
    case csi_report_quantities::cri_ri_li_pmi_cqi:
      return csi_report_unpack_pucch_cri_ri_li_pmi_cqi(packed, config);
    case csi_report_quantities::other:
    default:
      report_error("Invalid CSI report quantities.");
  }
}