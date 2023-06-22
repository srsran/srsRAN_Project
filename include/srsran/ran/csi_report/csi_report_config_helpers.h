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
#include "srsran/ran/csi_rs/csi_meas_config.h"

namespace srsran {

/// \brief Returns the Chanel State Information (CSI) report configuration parameters based on CSI-MeasConfig.
/// \param csi_meas CSI-RS measurement configuration.
/// \return Returns CSI report configuration parameters.
csi_report_configuration create_csi_report_configuration(const csi_meas_config& csi_meas);

} // namespace srsran
