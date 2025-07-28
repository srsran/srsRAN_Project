/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/asn1/rrc_nr/sys_info.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {
namespace srs_du {

/// Creates and fills an ASN.1 SIB19 message structure for NTN cell configuration.
/// \param[in] sib19_params The SIB19 configuration parameters to encode.
/// \return The filled ASN.1 SIB19 structure.
asn1::rrc_nr::sib19_r17_s make_asn1_rrc_cell_sib19(const sib19_info& sib19_params);

/// Fills advanced cell NTN-config parameters in ASN.1 SIB19 message.
/// \param[in]  sib19_params content of SIB19 msg.
/// \param[out] out The ASN.1 SIB19 struct to fill.
/// \return A buffer with packed SIB19 as BCCH-DL-SCH message.
void make_asn1_rrc_advanced_cell_sib19(const sib19_info& sib19_params, asn1::rrc_nr::sib19_r17_s& out);

} // namespace srs_du
} // namespace srsran
