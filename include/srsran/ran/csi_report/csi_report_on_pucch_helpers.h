/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_packed.h"
#include "srsran/support/units.h"

namespace srsran {

/// \brief Gets the Channel State Information (CSI) report size when the CSI report is transmitted in PUCCH.
///
/// Field widths are defined in TS38.212 Section 6.3.1.1.2.
///
/// \param[in] config CSI report configuration.
/// \return The report size in bits.
units::bits get_csi_report_pucch_size(const csi_report_configuration& config);

/// \brief Checks that the CSI payload carried over PUCCH is valid given a CSI report configuration.
///
/// This validator can be used before calling \ref csi_report_unpack_pucch to avoid triggering any assertions.
///
/// \param[in] packed Packed CSI report.
/// \param[in] config CSI report configuration.
/// \return \c true if the CSI payload is valid, \c false otherwise.
bool validate_pucch_csi_payload(const csi_report_packed& packed, const csi_report_configuration& config);

/// \brief Unpacks Channel State Information (CSI) report multiplexed in PUCCH.
///
/// The unpacking is CSI report unpacking defined in TS38.212 Section 6.3.1.1.2.
///
/// \param[in] packed Packed CSI report.
/// \param[in] config CSI report configuration.
/// \return The CSI report data.
csi_report_data csi_report_unpack_pucch(const csi_report_packed& packed, const csi_report_configuration& config);

} // namespace srsran
