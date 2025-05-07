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

#include "srsran/mac/cell_configuration.h"
#include "srsran/mac/mac_lc_config.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
}

/// Derives MAC Cell Configuration from DU Cell Configuration.
mac_cell_creation_request make_mac_cell_config(du_cell_index_t                                 cell_index,
                                               const srs_du::du_cell_config&                   du_cfg,
                                               const byte_buffer&                              sib1,
                                               span<const bcch_dl_sch_payload_type>            si_messages,
                                               const sched_cell_configuration_request_message& sched_cell_cfg);

/// \brief Generates default SRB MAC Logical Channel configuration for SRBs other than SRB0.
mac_lc_config make_default_srb_mac_lc_config(lcid_t lcid);

/// \brief Generates default DRB MAC Logical Channel configuration for DRBs.
mac_lc_config make_default_drb_mac_lc_config();

/// Returns the \c prioritized_bit_rate enum value greater than or equal to given bitrate in bit per second.
prioritized_bit_rate get_pbr_ceil(uint64_t bitrate_bps);

/// \brief Generates DRB MAC Logical Channel configuration for GBR DRBs.
mac_lc_config make_gbr_drb_mac_lc_config(const gbr_qos_flow_information& gbr_qos_flow_info);

/// \brief Generates DRB MAC Logical Channel configuration for non-GBR DRBs.
mac_lc_config make_non_gbr_drb_mac_lc_config();

} // namespace srsran
