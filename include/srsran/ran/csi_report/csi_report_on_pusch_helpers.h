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
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_packed.h"
#include "srsran/ran/csi_report/csi_report_pusch_size.h"

namespace srsran {

/// \brief Calculates the Channel State Information (CSI) payload sizes when the CSI report is transmitted in PUSCH.
///
/// Field widths are defined in TS38.212 Section 6.3.2.1.2.
csi_report_pusch_size get_csi_report_pusch_size(const csi_report_configuration& config);

/// \brief Checks that the CSI payload carried over PUSCH is valid given a CSI report configuration.
///
/// This validator can be used before calling \ref csi_report_unpack_pusch to avoid triggering any assertions.
///
/// \param[in] csi1_packed Packed CSI Part 1 report.
/// \param[in] csi2_packed Packed CSI Part 2 report.
/// \param[in] config CSI report configuration.
/// \return \c true if the CSI payload is valid, \c false otherwise.
bool validate_pusch_csi_payload(const csi_report_packed&        csi1_packed,
                                const csi_report_packed&        csi2_packed,
                                const csi_report_configuration& config);

/// \brief Unpacks a Channel State Information (CSI) report multiplexed in PUSCH containing CSI Part 1 and Part 2.
///
/// The CSI report is defined in TS38.212 Section 6.3.2.1.2.
///
/// \param[in] csi1_packed Packed CSI Part 1 report.
/// \param[in] csi2_packed Packed CSI Part 2 report.
/// \param[in] config CSI report configuration.
/// \return The CSI report data.
csi_report_data csi_report_unpack_pusch(const csi_report_packed&        csi1_packed,
                                        const csi_report_packed&        csi2_packed,
                                        const csi_report_configuration& config);

/// \brief Unpacks Channel State Information (CSI) report multiplexed in PUSCH containing CSI Part 1 only.
///
/// The CSI report is defined in TS38.212 Section 6.3.2.1.2.
///
/// \param[in] csi1_packed Packed CSI Part 1 report.
/// \param[in] config CSI report configuration.
/// \return The CSI report data.
csi_report_data csi_report_unpack_pusch(const csi_report_packed& csi1_packed, const csi_report_configuration& config);
} // namespace srsran
