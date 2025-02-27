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

#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"

using namespace srsran;

/// Derives MAC Cell Configuration from DU Cell Configuration.
mac_cell_creation_request srsran::make_mac_cell_config(du_cell_index_t               cell_index,
                                                       const srs_du::du_cell_config& du_cfg,
                                                       std::vector<byte_buffer>      bcch_dl_sch_payloads,
                                                       const sched_cell_configuration_request_message& sched_cell_cfg)
{
  mac_cell_creation_request mac_cfg{};
  mac_cfg.cell_index           = cell_index;
  mac_cfg.pci                  = du_cfg.pci;
  mac_cfg.scs_common           = du_cfg.scs_common;
  mac_cfg.ssb_cfg              = du_cfg.ssb_cfg;
  mac_cfg.dl_carrier           = du_cfg.dl_carrier;
  mac_cfg.ul_carrier           = du_cfg.ul_carrier;
  mac_cfg.cell_barred          = du_cfg.cell_barred;
  mac_cfg.intra_freq_resel     = du_cfg.intra_freq_resel;
  mac_cfg.bcch_dl_sch_payloads = std::move(bcch_dl_sch_payloads);
  mac_cfg.sched_req            = sched_cell_cfg;

  return mac_cfg;
}

mac_lc_config srsran::make_default_srb_mac_lc_config(lcid_t lcid)
{
  mac_lc_config mac_cfg{};
  // See TS 38.331, 9.2.1 Default SRB configurations.
  mac_cfg.priority            = lcid == LCID_SRB2 ? 3 : 1;
  mac_cfg.lcg_id              = uint_to_lcg_id(0);
  mac_cfg.pbr                 = prioritized_bit_rate::infinity;
  mac_cfg.bsd                 = bucket_size_duration::ms5;
  mac_cfg.lc_sr_mask          = false;
  mac_cfg.lc_sr_delay_applied = false;
  mac_cfg.sr_id               = uint_to_sched_req_id(0);
  return mac_cfg;
}

mac_lc_config srsran::make_default_drb_mac_lc_config()
{
  mac_lc_config mac_cfg{};
  // [Implementation-Defined] Setting priority higher than the least priority among SRBs.
  mac_cfg.priority = 5;
  mac_cfg.lcg_id   = uint_to_lcg_id(2);
  // [Implementation-Defined] By default we set the PBR to 0 so that there is no prioritized bitrate for the DRB. But,
  // can be customised based on the requirement.
  mac_cfg.pbr                 = prioritized_bit_rate::kBps0;
  mac_cfg.bsd                 = bucket_size_duration::ms300;
  mac_cfg.lc_sr_mask          = false;
  mac_cfg.lc_sr_delay_applied = false;
  mac_cfg.sr_id               = uint_to_sched_req_id(0);
  return mac_cfg;
}

prioritized_bit_rate srsran::get_pbr_ceil(uint64_t bitrate_bps)
{
  // Convert given bitrate (bps) to kilo Bytes per second.
  const float given_bitrate_kBps = bitrate_bps / (1000 * 8);
  // Prioritized bitrate values as per TS 38.331, \c prioritisedBitRate.
  static constexpr std::array<unsigned, 15> pbr_kBps{
      0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
  // Find PBR value greater than or equal to give bitrate.
  for (unsigned pbr : pbr_kBps) {
    if (given_bitrate_kBps <= pbr) {
      return static_cast<prioritized_bit_rate>(pbr);
    }
  }
  return prioritized_bit_rate::infinity;
}

mac_lc_config srsran::make_gbr_drb_mac_lc_config(const gbr_qos_flow_information& gbr_qos_flow_info)
{
  mac_lc_config mac_cfg{make_default_drb_mac_lc_config()};
  // [Implementation-Defined] Setting priority higher than the non-GBR DRBs.
  mac_cfg.priority = 4;
  // [Implementation-Defined] Setting LCG ID other than the one assigned to non-GBR DRBs.
  mac_cfg.lcg_id = uint_to_lcg_id(1);
  // Set PBR based on the given GBR QoS flow information.
  mac_cfg.pbr = get_pbr_ceil(gbr_qos_flow_info.gbr_ul);
  return mac_cfg;
}

mac_lc_config srsran::make_non_gbr_drb_mac_lc_config()
{
  mac_lc_config mac_cfg{make_default_drb_mac_lc_config()};
  // [Implementation-Defined] Value of PBR is set such that the final value of B_j (computed before applying
  // logical channel prioritization procedure) is small (non-zero) for non-GBR DRBs. See TS 38.321, 5.4.3.1.1-3, for
  // computation of B_j and logical channel prioritization procedure.
  mac_cfg.pbr = prioritized_bit_rate::kBps8;
  return mac_cfg;
}
