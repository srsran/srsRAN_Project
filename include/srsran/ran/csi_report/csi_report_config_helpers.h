/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/ran/csi_rs/csi_meas_config.h"

namespace srsran {

/// \brief Returns the Chanel State Information (CSI) report configuration parameters based on CSI-MeasConfig.
/// \param csi_meas CSI-RS measurement configuration.
/// \return Returns CSI report configuration parameters.
csi_report_configuration create_csi_report_configuration(const csi_meas_config& csi_meas);

/// CSI report configuration validator.
bool is_valid(const csi_report_configuration& config);

/// \brief Tells whether the CSI report is configured for PUSCH.
///
/// \remark There is no explicit statement defining this in the TS; however, we can infer from \c reportConfigType in \c
/// CSI-ReportConfig, TS 38.331, that the CSI report is configured for PUSCH if it is either \c semiPersistentOnPUSCH or
/// \c aperiodic, while it's configured for PUCCH if it is \c periodic or \c semiPersistentOnPUCCH.
bool is_pusch_configured(const csi_meas_config& csi_meas);

} // namespace srsran
