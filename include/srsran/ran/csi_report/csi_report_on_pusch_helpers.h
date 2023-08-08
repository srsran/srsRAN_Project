/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_pusch_size.h"

namespace srsran {

/// \brief Calculates the Channel State Information (CSI) payload sizes when the CSI report is transmitted in PUSCH.
///
/// Fields widths are defined in TS38.212 Section 6.3.2.1.2.
csi_report_pusch_size get_csi_report_pusch_size(const csi_report_configuration& config);

} // namespace srsran
