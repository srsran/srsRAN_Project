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

#include "srsran/asn1/rrc_nr/serving_cell.h"
#include "srsran/ran/csi_rs/csi_meas_config.h"

namespace srsran {
namespace srs_du {

/// \brief Fills ASN.1 ZP-CSI-RS-Resource struct.
/// \param[in] cfg ZP-CSI-RS-Resource configuration.
/// \return ASN.1 ZP-CSI-RS-Resource struct.
asn1::rrc_nr::zp_csi_rs_res_s make_asn1_zp_csi_rs_resource(const zp_csi_rs_resource& cfg);

/// \brief Fills ASN.1 ZP-CSI-RS-ResourceSet struct.
/// \param[in] cfg ZP-CSI-RS-ResourceSet configuration.
/// \return ASN.1 ZP-CSI-RS-ResourceSet struct.
asn1::rrc_nr::zp_csi_rs_res_set_s make_asn1_zp_csi_rs_resource_set(const zp_csi_rs_resource_set& cfg);

/// \brief Fills ASN.1 CSI-MeasConfig struct.
/// \param[out] out The ASN.1 CSI-MeasConfig struct to fill.
/// \param[in] src Previous CSI Measurement configuration of UE.
/// \param[in] dest Next CSI Measurement configuration of UE.
void calculate_csi_meas_config_diff(asn1::rrc_nr::csi_meas_cfg_s& out,
                                    const csi_meas_config&        src,
                                    const csi_meas_config&        dest);

} // namespace srs_du
} // namespace srsran
