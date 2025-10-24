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

#include "srsran/scheduler/config/rlm_helper.h"
#include "srsran/ran/ssb/ssb_mapping.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace rlm_helper;

radio_link_monitoring_config
srsran::rlm_helper::make_radio_link_monitoring_config(const rlm_builder_params&       params,
                                                      span<const nzp_csi_rs_resource> csi_rs_resources)
{
  srsran_assert(params.L_max == 4U or params.L_max == 8U or params.L_max == 64U, "Invalid SSB L max");
  // Ensure the SSB parameters are provided when the RLM resources use SSB.
  srsran_assert(params.resource_type != rlm_resource_type::ssb or
                    params.resource_type != rlm_resource_type::ssb_and_csi_rs or params.ssb_params.has_value(),
                "Missing SSB parameters needed to create RLM resources using SSB");

  radio_link_monitoring_config rlm_cfg;

  auto get_N_RLM = [](uint8_t l_max) -> uint8_t {
    // Defines N_RLM values in Table 5-1, TS 38.213.
    switch (l_max) {
      case 4:
        return 2;
      case 8:
        return 4;
      case 64:
        return 8;
      default:
        srsran_assertion_failure("Invalid L_max value");
        return 2;
    }
  };

  const unsigned max_nof_rlm_resources = get_N_RLM(params.L_max);

  if (params.resource_type == rlm_resource_type::default_type) {
    return rlm_cfg;
  }

  // [Implementation-defined] We build the RLM resource list with 1 resource only: assign the SSB id of the only SSB
  // beam currently supported.
  unsigned rlm_rs_idx = 0U;
  if (params.resource_type == rlm_resource_type::ssb or params.resource_type == rlm_resource_type::ssb_and_csi_rs) {
    srsran_assert(params.ssb_params.value().ssb_bitmap == static_cast<uint64_t>(0b1) << 63U, "Invalid SSB bitmap");
    auto& rlm_rs  = rlm_cfg.rlm_resources.emplace_back();
    rlm_rs.res_id = to_rlm_res_id(rlm_rs_idx++);
    // [Implementation-defined] This is the only supported option at the moment.
    rlm_rs.resource_purpose = radio_link_monitoring_config::radio_link_monitoring_rs::purpose::rlf;
    rlm_rs.detection_resource.emplace<ssb_id_t>(static_cast<ssb_id_t>(params.ssb_params.value().ssb_beam_ids[0U]));
  }

  // [Implementation-defined] We build the RLM resource list with the maximum allowed number of resources N_RLM, as per
  // Table 5-1.
  if (params.resource_type == rlm_resource_type::csi_rs or params.resource_type == rlm_resource_type::ssb_and_csi_rs) {
    auto csi_rs_res = csi_rs_resources.begin();

    // In the CSI configuration, there are 5 CSI-RS resources, 1 for measurement, 4 for tracking. While all 4 CSI-RS are
    // candidates for being used for RLM, the CSI-RS resource for measurement can be a candidate only when configured
    // with 1 port. For the RLM config, we want to be consistent and use always the tracking CSI-RS resources.
    const bool all_csi_res_are_candidates =
        std::all_of(csi_rs_resources.begin(), csi_rs_resources.end(), [&](const auto& csi_rs) {
          return csi_rs.res_mapping.nof_ports == 1U and csi_rs.res_mapping.cdm == csi_rs_cdm_type::no_CDM and
                 (csi_rs.res_mapping.freq_density == csi_rs_freq_density_type::one or
                  csi_rs.res_mapping.freq_density == csi_rs_freq_density_type::three);
        });
    if (all_csi_res_are_candidates and csi_rs_resources.size() == 5U) {
      csi_rs_res++;
    }

    for (; rlm_rs_idx != max_nof_rlm_resources; ++rlm_rs_idx, csi_rs_res++) {
      // Stop creating resources if there are no more CSI-RS resources to iterate over or the RLM config has reached its
      // maximum number of resources.
      if (csi_rs_res == csi_rs_resources.end() or rlm_cfg.rlm_resources.full()) {
        break;
      }
      // As per TS 38.213, Section 5, "For a CSI-RS resource configuration, powerControlOffsetSS is not applicable and a
      // UE expects to be provided only 'noCDM' from cdm-Type, only 'one' and 'three' from density, and only '1 port'
      // from nrofPorts".
      if (csi_rs_res->res_mapping.nof_ports == 1U and csi_rs_res->res_mapping.cdm == csi_rs_cdm_type::no_CDM and
          (csi_rs_res->res_mapping.freq_density == csi_rs_freq_density_type::one or
           csi_rs_res->res_mapping.freq_density == csi_rs_freq_density_type::three)) {
        auto& rlm_rs  = rlm_cfg.rlm_resources.emplace_back();
        rlm_rs.res_id = to_rlm_res_id(rlm_rs_idx);
        // [Implementation-defined] This is the only supported option at the moment.
        rlm_rs.resource_purpose = radio_link_monitoring_config::radio_link_monitoring_rs::purpose::rlf;
        rlm_rs.detection_resource.emplace<nzp_csi_rs_res_id_t>(csi_rs_res->res_id);
      }
    }
  }

  return rlm_cfg;
}
