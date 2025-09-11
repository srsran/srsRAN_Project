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

#include "srsran/scheduler/config/serving_cell_config_validator.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/csi_rs/csi_rs_config_helpers.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/scheduler/config/sched_cell_config_helpers.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/config/validator_helpers.h"
#include "fmt/std.h"
#include <numeric>

using namespace srsran;

#define VERIFY_ID_EXISTS(cond_lambda, id_list, ...)                                                                    \
  if (std::find_if(id_list.begin(), id_list.end(), cond_lambda) == id_list.end()) {                                    \
    return make_unexpected(fmt::format(__VA_ARGS__));                                                                  \
  }

static bool
csi_offset_colliding_with_sr(unsigned sr_offset, unsigned csi_offset, unsigned sr_period, unsigned csi_period)
{
  unsigned lcm_csi_sr_period = std::lcm(sr_period, csi_period);
  for (unsigned csi_off = csi_offset; csi_off < lcm_csi_sr_period; csi_off += csi_period) {
    for (unsigned sr_off = sr_offset; sr_off < lcm_csi_sr_period; sr_off += sr_period) {
      if (csi_off == sr_off) {
        return true;
      }
    }
  }

  return false;
}

validator_result config_validators::validate_pdcch_cfg(const serving_cell_config& ue_cell_cfg,
                                                       const dl_config_common&    dl_cfg_common)
{
  const auto& init_dl_bwp = ue_cell_cfg.init_dl_bwp;
  if (init_dl_bwp.pdcch_cfg.has_value()) {
    const auto& pdcch_cfg = init_dl_bwp.pdcch_cfg.value();

    VERIFY(has_unique_ids(pdcch_cfg.coresets, &coreset_configuration::id), "Duplication of CoresetId");
    VERIFY(has_unique_ids(pdcch_cfg.search_spaces, [](const search_space_configuration& ss) { return ss.get_id(); }),
           "Duplication of SearchSpaceId");

    for (const auto& ss : pdcch_cfg.search_spaces) {
      const bool cset_id_found_in_ded =
          std::find_if(pdcch_cfg.coresets.begin(),
                       pdcch_cfg.coresets.end(),
                       [cs_id = ss.get_coreset_id()](const coreset_configuration& cset_cfg) {
                         return cset_cfg.id == cs_id;
                       }) != pdcch_cfg.coresets.end();
      const bool cst_id_found_in_common =
          dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value()
              ? dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().id == ss.get_coreset_id()
              : false;
      const bool cst_id_found_in_coreset0 = ss.get_coreset_id() == 0;
      VERIFY(cset_id_found_in_ded or cst_id_found_in_common or cst_id_found_in_coreset0,
             "Coreset Id. {} indexed by SearchSpace Id. {} not found within the configured Coresets",
             fmt::underlying(ss.get_coreset_id()),
             fmt::underlying(ss.get_id()));
    }

    const unsigned total_nof_monitored_pdcch_candidates =
        config_helpers::compute_tot_nof_monitored_pdcch_candidates_per_slot(ue_cell_cfg, dl_cfg_common);
    const auto scs = dl_cfg_common.init_dl_bwp.generic_params.scs;
    VERIFY(total_nof_monitored_pdcch_candidates <= max_nof_monitored_pdcch_candidates(scs),
           "Nof. PDCCH candidates monitored per slot for a DL BWP={} exceeds maximum value={}\n",
           total_nof_monitored_pdcch_candidates,
           max_nof_monitored_pdcch_candidates(scs));
  }

  // TODO: Validate other parameters.
  return {};
}

static validator_result validate_zp_csi_rs(const serving_cell_config& ue_cell_cfg)
{
  if (not ue_cell_cfg.init_dl_bwp.pdsch_cfg.has_value()) {
    return {};
  }
  const auto& pdsch_cfg = ue_cell_cfg.init_dl_bwp.pdsch_cfg.value();

  const auto& zp_list = pdsch_cfg.zp_csi_rs_res_list;
  VERIFY(not zp_list.empty() == pdsch_cfg.p_zp_csi_rs_res.has_value(),
         "Only periodic ZP-CSI-RS-ResourceId is supported");
  VERIFY(not zp_list.empty() == ue_cell_cfg.csi_meas_cfg.has_value(),
         "CSI-MeasConfig must be configured if and only if ZP-CSI-RS is configured");
  if (zp_list.empty()) {
    // No CSI configured.
    return {};
  }
  const csi_meas_config& csi_cfg = ue_cell_cfg.csi_meas_cfg.value();

  // Consistency of ZP-CSI-RS resources and periodic resource set.
  VERIFY(has_unique_ids(zp_list, &zp_csi_rs_resource::id), "Duplication of ZP-CSI-RS-ResourceId");
  VERIFY(pdsch_cfg.p_zp_csi_rs_res->id == (zp_csi_rs_res_set_id_t)0,
         "Only ZP-CSI-RS-ResourceId=0 is allowed for periodic ZP-CSI-RS");
  VERIFY(are_all_unique(pdsch_cfg.p_zp_csi_rs_res->zp_csi_rs_res_list), "Duplication of ZP-CSI-RS-ResourceId");

  // CSI-IM resources should match with ZP-CSI-RS.
  for (const auto& csi_im : csi_cfg.csi_im_res_list) {
    bool found = std::any_of(zp_list.begin(), zp_list.end(), [&csi_im](const zp_csi_rs_resource& zp) {
      return zp.period == csi_im.csi_res_period and zp.offset == csi_im.csi_res_offset and
             zp.res_mapping.freq_band_rbs == csi_im.freq_band_rbs and
             zp.res_mapping.first_ofdm_symbol_in_td == csi_im.csi_im_res_element_pattern->symbol_location;
    });
    VERIFY(found,
           "CSI-IM does not overlap with ZP-CSI-RS. CSI-IM: {{period={} offset={} band={} symbol={}}}",
           csi_im.csi_res_period ? fmt::to_string(fmt::underlying(*csi_im.csi_res_period)) : "none",
           csi_im.csi_res_offset,
           csi_im.freq_band_rbs,
           csi_im.csi_im_res_element_pattern->symbol_location);
  }

  return {};
}

validator_result config_validators::validate_pdsch_cfg(const serving_cell_config& ue_cell_cfg)
{
  const auto& init_dl_bwp = ue_cell_cfg.init_dl_bwp;
  if (not init_dl_bwp.pdsch_cfg.has_value()) {
    return {};
  }
  const pdsch_config& pdsch_cfg = init_dl_bwp.pdsch_cfg.value();
  const auto&         dl_lst    = pdsch_cfg.pdsch_td_alloc_list;
  for (const auto& pdsch : dl_lst) {
    VERIFY(pdsch.k0 <= SCHEDULER_MAX_K0, "k0={} value exceeds maximum supported k0", pdsch.k0);
  }

  HANDLE_ERROR(validate_zp_csi_rs(ue_cell_cfg));

  // TODO: Validate other parameters.
  return {};
}

validator_result config_validators::validate_pucch_cfg(const serving_cell_config& ue_cell_cfg,
                                                       const pucch_config_common& pucch_cfg_common,
                                                       unsigned                   nof_dl_antennas)
{
  VERIFY(ue_cell_cfg.ul_config.has_value() and ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value(),
         "Missing configuration for uplinkConfig or pucch-Config in spCellConfig");

  const auto& pucch_cfg = ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Helper to retrives a given PUCCH resource given its ID from the PUCCH resource list.
  auto get_pucch_resource_with_cell_id = [&pucch_cfg](unsigned res_id) {
    return std::find_if(pucch_cfg.pucch_res_list.begin(),
                        pucch_cfg.pucch_res_list.end(),
                        [res_id](const pucch_resource& res) { return res.res_id.cell_res_id == res_id; });
  };

  auto get_pucch_resource_with_ue_id = [&pucch_cfg](unsigned res_id) {
    return std::find_if(pucch_cfg.pucch_res_list.begin(),
                        pucch_cfg.pucch_res_list.end(),
                        [res_id](const pucch_resource& res) { return res.res_id.ue_res_id == res_id; });
  };

  // Verify that the PUCCH resources IDs of each PUCCH resource set point at a corresponding item in the PUCCH reource
  // list.
  VERIFY(pucch_cfg.pucch_res_set.size() >= 2, "At least 2 PUCCH resource sets need to be configured in PUCCH-Config");
  VERIFY(pucch_cfg.pucch_res_set[0].pucch_res_set_id == pucch_res_set_idx::set_0 and
             pucch_cfg.pucch_res_set[1].pucch_res_set_id == pucch_res_set_idx::set_1,
         "PUCCH resource sets 0 and 1 are expected to have PUCCH-ResourceSetId 0 and 1, respectively");
  VERIFY((not pucch_cfg.pucch_res_set[0].pucch_res_id_list.empty()) and
             (not pucch_cfg.pucch_res_set[1].pucch_res_id_list.empty()),
         "PUCCH resource sets 0 and 1 are expected to have a non-empty set of PUCCH resource id");
  for (size_t pucch_res_set_idx = 0; pucch_res_set_idx != 2; ++pucch_res_set_idx) {
    for (auto res_idx : pucch_cfg.pucch_res_set[pucch_res_set_idx].pucch_res_id_list) {
      const auto* pucch_res_it = get_pucch_resource_with_cell_id(res_idx.cell_res_id);
      VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_it,
             "PUCCH cell res. id={} in PUCCH res. set id={} not found in the PUCCH resource list",
             res_idx.cell_res_id,
             pucch_res_set_idx);
    }
  }

  // Verify that the size of PUCCH resource set 1 is not smaller than the size of PUCCH resource set 0.
  VERIFY(pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() <= pucch_cfg.pucch_res_set[1].pucch_res_id_list.size(),
         "PUCCH resource set 1's size should be greater or equal to PUCCH resource set 0's size");

  // Verify each resource format matches the corresponding parameters.
  for (auto res : pucch_cfg.pucch_res_list) {
    const bool format_match_format_params =
        (res.format == pucch_format::FORMAT_0 and std::holds_alternative<pucch_format_0_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_1 and std::holds_alternative<pucch_format_1_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_2 and std::holds_alternative<pucch_format_2_3_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_3 and std::holds_alternative<pucch_format_2_3_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_4 and std::holds_alternative<pucch_format_4_cfg>(res.format_params));
    VERIFY(format_match_format_params,
           "PUCCH cell res id={} format does not match the PUCCH format parameters",
           res.res_id.cell_res_id);
  }

  // Verify that PUCCH Format 0 and Format 1 aren't both present in the UE configuration.
  bool has_format_0 = false;
  bool has_format_1 = false;
  bool has_format_2 = false;
  bool has_format_3 = false;
  bool has_format_4 = false;
  for (auto res : pucch_cfg.pucch_res_list) {
    has_format_0 = has_format_0 or res.format == pucch_format::FORMAT_0;
    has_format_1 = has_format_1 or res.format == pucch_format::FORMAT_1;
    has_format_2 = has_format_2 or res.format == pucch_format::FORMAT_2;
    has_format_3 = has_format_3 or res.format == pucch_format::FORMAT_3;
    has_format_4 = has_format_4 or res.format == pucch_format::FORMAT_4;
  }
  VERIFY(not(has_format_0 and has_format_1),
         "Only PUCCH Format 0 or Format 1 can be configured in a UE configuration, not both.");
  VERIFY(static_cast<unsigned>(has_format_2) + static_cast<unsigned>(has_format_3) +
                 static_cast<unsigned>(has_format_4) ==
             1,
         "Only one of PUCCH Format 2, Format 3 or Format 4 can be configured in a UE configuration.");

  // [Implementation defined] The scheduler expects the resources from the common resource set and Resource Set 0 to use
  // the same format. The formats from the common resource sets are expressed in TS 38.213 Table 9.2.1-1.
  if (has_format_0) {
    VERIFY(pucch_cfg_common.pucch_resource_common <= 2,
           "When using PUCCH Format 0, the valid values for pucch_resource_common are {{0, 1, 2}}.");
  } else {
    VERIFY(pucch_cfg_common.pucch_resource_common > 2,
           "When using PUCCH Format 1, the valid values for pucch_resource_common are {{3, ..., 15}}.");
  }

  // Verify that each PUCCH resource has a valid cell resource ID.
  for (auto res_idx : pucch_cfg.pucch_res_list) {
    // TODO: handle the cases of F0+F3 and F0+F4.
    if (has_format_0 and has_format_2) {
      // For Format 0, we use a special cell resources ID to indicate that the resource does not exist in the cell
      // resource list, but only exists in the UE dedicated configuration.
      unsigned cell_res_id_special_res = std::numeric_limits<unsigned>::max();
      VERIFY(res_idx.res_id.cell_res_id < pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES or
                 res_idx.res_id.cell_res_id == cell_res_id_special_res,
             "PUCCH cell res. id={} exceeds the maximum supported PUCCH resource ID",
             res_idx.res_id.cell_res_id);
    } else {
      VERIFY(res_idx.res_id.cell_res_id < pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES,
             "PUCCH cell res. id={} exceeds the maximum supported PUCCH resource ID",
             res_idx.res_id.cell_res_id);
    }
  }

  // NOTE: No need to check this for Format 0, as this struct doesn't exist for F0.
  if (has_format_1) {
    VERIFY(pucch_cfg.format_1_common_param.has_value(), "Missing PUCCH-format1 parameters in PUCCH-Config");
  }
  if (has_format_2) {
    VERIFY(pucch_cfg.format_2_common_param.has_value(), "Missing PUCCH-format2 parameters in PUCCH-Config");
  }
  if (has_format_3) {
    VERIFY(pucch_cfg.format_3_common_param.has_value(), "Missing PUCCH-format3 parameters in PUCCH-Config");
  }
  if (has_format_4) {
    VERIFY(pucch_cfg.format_4_common_param.has_value(), "Missing PUCCH-format4 parameters in PUCCH-Config");
  }

  if (has_format_0) {
    VERIFY(pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() == pucch_cfg.pucch_res_set[1].pucch_res_id_list.size(),
           "With Format 0, the PUCCH resource sets 0 and 1 must have the same size");
  }

  // Check PUCCH Formats for each PUCCH Resource Set.
  for (auto res_idx : pucch_cfg.pucch_res_set[0].pucch_res_id_list) {
    const auto* pucch_res_it = get_pucch_resource_with_ue_id(res_idx.ue_res_id);
    VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_it and
               (pucch_res_it->format == pucch_format::FORMAT_0 or pucch_res_it->format == pucch_format::FORMAT_1),
           "Only PUCCH Resource Format 0 or Format 1 expected in PUCCH resource set 0.");
  }
  for (auto res_idx : pucch_cfg.pucch_res_set[1].pucch_res_id_list) {
    const auto* pucch_res_it = get_pucch_resource_with_ue_id(res_idx.ue_res_id);
    VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_it and
               (pucch_res_it->format == pucch_format::FORMAT_2 or pucch_res_it->format == pucch_format::FORMAT_3 or
                pucch_res_it->format == pucch_format::FORMAT_4),
           "Only PUCCH Resource Format 2, Format 3 or Format 4 expected in PUCCH resource set 1.");
  }

  // Verify the PUCCH resource id that indicated in the SR resource config exists in the PUCCH resource list.
  VERIFY(pucch_cfg.sr_res_list.size() == 1, "Only SchedulingRequestResourceConfig with size 1 supported");
  VERIFY(pucch_cfg.pucch_res_list.end() !=
             get_pucch_resource_with_cell_id(pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id),
         "PUCCH cell res. id={} given in SR resource config not found in the PUCCH resource list",
         pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id);

  const auto* pucch_res_sr = get_pucch_resource_with_cell_id(pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id);
  VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_sr,
         "PUCCH cell res. id={} for SR could not be found in PUCCH resource list",
         pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id);
  VERIFY(pucch_res_sr->format == pucch_format::FORMAT_0 or pucch_res_sr->format == pucch_format::FORMAT_1,
         "PUCCH resource used for SR is expected to be Format 0 or Format 1");

  // With Format 0, the last resource in PUCCH resource set 1 should point at the SR resource. Also, the last (or second
  // last if there is CSI) resource in PUCCH resource set 1 should have symbols and starting PRBs that match those of
  // the SR resource.
  // TODO: handle the cases of F0+F3 and F0+F4.
  if (has_format_0 and has_format_2) {
    const auto* last_res_in_set_0 =
        get_pucch_resource_with_cell_id(pucch_cfg.pucch_res_set[0].pucch_res_id_list.back().cell_res_id);
    VERIFY(pucch_cfg.pucch_res_list.end() != last_res_in_set_0 and
               last_res_in_set_0->res_id == pucch_cfg.sr_res_list.front().pucch_res_id,
           "With Format 0, the last PUCCH resource in PUCCH resource set 0 should point at the SR resource");
    // The harq_res_in_set_1_for_sr is only created in the UE, it doesn't exist in the cell list of resources. Use the
    // ue_res_id to find it.
    const auto* harq_res_in_set_1_for_sr =
        get_pucch_resource_with_ue_id(pucch_cfg.pucch_res_set[1].pucch_res_id_list.back().ue_res_id);
    VERIFY(pucch_cfg.pucch_res_list.end() != harq_res_in_set_1_for_sr and
               harq_res_in_set_1_for_sr->format == pucch_format::FORMAT_2 and
               std::holds_alternative<pucch_format_2_3_cfg>(harq_res_in_set_1_for_sr->format_params),
           "With Format 0, PUCCH resource set 1 should contain a reserved HARQ-ACK resource for SR slots of Format 2");
    VERIFY(harq_res_in_set_1_for_sr->starting_prb == pucch_res_sr->starting_prb and
               harq_res_in_set_1_for_sr->second_hop_prb == pucch_res_sr->second_hop_prb and
               harq_res_in_set_1_for_sr->starting_sym_idx == pucch_res_sr->starting_sym_idx and
               harq_res_in_set_1_for_sr->nof_symbols == pucch_res_sr->nof_symbols,
           "With Format 0, PUCCH resource set 1 should contain a resource Format 2 reserved for HARQ-ACK with symbols "
           " and starting PRBs that match the SR resource");
  }

  // Verify that the PUCCH setting used for CSI report have been configured properly.
  if (ue_cell_cfg.csi_meas_cfg.has_value()) {
    const auto& csi_cfg = ue_cell_cfg.csi_meas_cfg.value();
    VERIFY(not csi_cfg.csi_report_cfg_list.empty() and
               std::holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                   csi_cfg.csi_report_cfg_list.front().report_cfg_type) and
               not std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                       csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                       .pucch_csi_res_list.empty(),
           "PUCCH-CSI-ResourceList has not been configured in the CSI-reportConfig");

    const auto& csi = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        csi_cfg.csi_report_cfg_list.front().report_cfg_type);
    const unsigned csi_res_id = csi.pucch_csi_res_list.front().pucch_res_id.cell_res_id;
    // Verify the PUCCH resource id that indicated in the CSI resource config exists in the PUCCH resource list.
    const auto* csi_pucch_res = get_pucch_resource_with_cell_id(csi_res_id);
    VERIFY(csi_pucch_res != pucch_cfg.pucch_res_list.end(),
           "PUCCH cell res. id={} given in PUCCH-CSI-resourceList not found in the PUCCH resource list",
           csi_res_id);
    VERIFY(csi_pucch_res->format == pucch_format::FORMAT_2 or csi_pucch_res->format == pucch_format::FORMAT_3 or
               csi_pucch_res->format == pucch_format::FORMAT_4,
           "PUCCH resource used for CSI is expected to be Format 2, Format 3 or Format 4");

    // Verify the CSI/SR bits do not exceed the PUCCH F2/F3/F4 payload.
    unsigned       pucch_f2_f3_f4_max_payload = pucch_cfg.get_max_payload(csi_pucch_res->format);
    const auto     csi_report_cfg             = create_csi_report_configuration(ue_cell_cfg.csi_meas_cfg.value());
    const unsigned csi_report_size            = get_csi_report_pucch_size(csi_report_cfg).part1_size.value();
    unsigned       sr_offset                  = pucch_cfg.sr_res_list.front().offset;
    const bool     csi_sr_collision =
        csi_offset_colliding_with_sr(sr_offset,
                                     csi.report_slot_offset,
                                     sr_periodicity_to_slot(pucch_cfg.sr_res_list.front().period),
                                     csi_report_periodicity_to_uint(csi.report_slot_period));

    // Verify that, with Format 0 and Format 2, the CSI and SR don't fall on the same slot(s).
    if (pucch_res_sr->format == pucch_format::FORMAT_0 and csi_pucch_res->format == pucch_format::FORMAT_2) {
      VERIFY(not csi_sr_collision,
             "With PUCCH Format 0 and Format 2, we don't support SR opportunities falling on a CSI report slot");
    }

    // If SR and CSI are reported within the same slot, 1 SR bit can be multiplexed with CSI within the same PUCCH
    // resource.
    unsigned sr_bits_mplexed_with_csi = csi_sr_collision ? 1U : 0U;
    // In the PUCCH resource for CSI, there are no HARQ-ACK bits being reported; therefore we only need to check where
    // the CSI + SR bits fit into the max payload.
    const unsigned uci_bits_pucch_resource = csi_report_size + sr_bits_mplexed_with_csi;
    VERIFY(pucch_f2_f3_f4_max_payload >= uci_bits_pucch_resource,
           "UCI num. of bits ({}) exceeds the maximum CSI's PUCCH Format 2/3/4 payload ({})",
           uci_bits_pucch_resource,
           pucch_f2_f3_f4_max_payload);

    // TODO: handle the cases of F0+F3 and F0+F4.
    if (has_format_0 and has_format_2) {
      // With Format 0 and CSI, the second-last resource in PUCCH resource set 1 should point at the CSI resource.
      const auto* harq_set_1_res_for_csi =
          get_pucch_resource_with_ue_id(pucch_cfg.pucch_res_set[1]
                                            .pucch_res_id_list[pucch_cfg.pucch_res_set[1].pucch_res_id_list.size() - 2U]
                                            .ue_res_id);
      VERIFY(pucch_cfg.pucch_res_list.end() != harq_set_1_res_for_csi and
                 harq_set_1_res_for_csi->res_id == csi_pucch_res->res_id,
             "With Format 0 and CSI, the last PUCCH resource in PUCCH resource set 1 should point at the CSI resource");

      // The second-last resource in PUCCH resource set 0 should have symbols and starting PRBs that match those of the
      // CSI resource.
      // The harq_res_in_set_1_for_sr is only created in the UE, it doesn't exist in the cell list of resources. Use
      // the ue_res_id to find it.
      const auto* harq_res_in_set_0_for_csi =
          get_pucch_resource_with_ue_id(pucch_cfg.pucch_res_set[0]
                                            .pucch_res_id_list[pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() - 2U]
                                            .ue_res_id);
      VERIFY(pucch_cfg.pucch_res_list.end() != harq_res_in_set_0_for_csi and
                 harq_res_in_set_0_for_csi->format == pucch_format::FORMAT_0 and
                 std::holds_alternative<pucch_format_0_cfg>(harq_res_in_set_0_for_csi->format_params),
             "With Format 0, PUCCH resource set 0 should contain a F2 HARQ-ACK resource reserved for CSI slots");
      VERIFY(harq_res_in_set_0_for_csi->starting_prb == csi_pucch_res->starting_prb and
                 harq_res_in_set_0_for_csi->second_hop_prb == csi_pucch_res->second_hop_prb and
                 harq_res_in_set_0_for_csi->starting_sym_idx == csi_pucch_res->starting_sym_idx and
                 harq_res_in_set_0_for_csi->nof_symbols == csi_pucch_res->nof_symbols,
             "With Format 0, PUCCH resource set 0 should contain a F0 resource reserved for HARQ-ACK with symbols "
             " and starting PRBs that match the CSI resource");
    }
  }

  const auto& init_ul_bwp = ue_cell_cfg.ul_config.value().init_ul_bwp;
  if (init_ul_bwp.pucch_cfg.has_value()) {
    const auto& dl_to_ack_lst = init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack;
    for (const auto& k1 : dl_to_ack_lst) {
      VERIFY(k1 <= SCHEDULER_MAX_K1, "k1={} value exceeds maximum supported k1", k1);
      VERIFY(k1 >= 1, "k1={} value below minimum supported k1", k1);
    }
  }

  return {};
}

validator_result config_validators::validate_pusch_cfg(const uplink_config& ul_config, bool has_csi)
{
  VERIFY(ul_config.init_ul_bwp.pusch_cfg.has_value(), "Missing configuration for pusch-Config in spCellConfig");

  const auto& pusch_cfg = ul_config.init_ul_bwp.pusch_cfg.value();

  VERIFY(pusch_cfg.uci_cfg.has_value(), "Missing configuration for uci-OnPUSCH in pusch-Config");
  VERIFY(pusch_cfg.uci_cfg.value().beta_offsets_cfg.has_value() and
             std::holds_alternative<uci_on_pusch::beta_offsets_semi_static>(
                 pusch_cfg.uci_cfg.value().beta_offsets_cfg.value()),
         "Missing configuration or wrong configuration for beta-Offsets in uci-OnPUSCH");
  const auto& beta_offsets =
      std::get<uci_on_pusch::beta_offsets_semi_static>(pusch_cfg.uci_cfg.value().beta_offsets_cfg.value());
  VERIFY(beta_offsets.beta_offset_ack_idx_1.has_value() and beta_offsets.beta_offset_ack_idx_2.has_value() and
             beta_offsets.beta_offset_ack_idx_3.has_value(),
         "betaOffsetACK-Index1, betaOffsetACK-Index2 and betaOffsetACK-Index3 in uci-OnPUSCH must be configured");
  if (has_csi) {
    VERIFY(beta_offsets.beta_offset_csi_p1_idx_1.has_value() and beta_offsets.beta_offset_csi_p1_idx_2.has_value(),
           "When CSI is enabled, betaOffsetCSI-Part1-Index1 and betaOffsetCSI-Part1-Index2 in uci-OnPUSCH must be "
           "configured");
  }
  VERIFY(pusch_cfg.pusch_pwr_ctrl.has_value(), "Missing configuration for pusch-PowerControl in pusch-Config");
  VERIFY(not pusch_cfg.pusch_pwr_ctrl.value().is_tpc_accumulation_disabled,
         "TPC accumulation for PUSCH power control is expected to be enabled");

  VERIFY(pusch_cfg.pusch_pwr_ctrl.value().p0_alphasets.size() == 1 and
             pusch_cfg.pusch_pwr_ctrl.value().p0_alphasets.front().p0.has_value(),
         "The P0-alphaSet list for PUSCH power control is expected to have size 1 with p0 value configured");

  return {};
}

validator_result config_validators::validate_srs_cfg(const serving_cell_config& ue_cell_cfg)
{
  VERIFY(ue_cell_cfg.ul_config.has_value() and ue_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.has_value(),
         "Missing configuration for uplinkConfig or srs-Config in spCellConfig");

  const auto& srs_cfg = ue_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();

  VERIFY(srs_cfg.srs_res_set_list.size() == 1 and srs_cfg.srs_res_set_list.front().id == srs_config::MIN_SRS_RES_SET_ID,
         "The SRS resource set list is expected to have size 1 and its only set is expected to have ID 0");
  VERIFY(srs_cfg.srs_res_set_list.front().srs_res_id_list.size() == 1,
         "The SRS resource list of the SRS resource set ID 0 is expected to have size 1");
  VERIFY(srs_cfg.srs_res_list.size() == 1 and srs_cfg.srs_res_list.front().id.ue_res_id == srs_config::MIN_SRS_RES_ID,
         "The SRS resource list is expected to have size 1 and its only resource is expected to have ID 0");
  VERIFY(srs_cfg.srs_res_set_list.front().srs_res_id_list.front() == srs_cfg.srs_res_list.front().id.ue_res_id,
         "The SRS resource set ID 0's resource should point to the SRS resource ID 0");
  const auto& srs_res_set = srs_cfg.srs_res_set_list.front();
  VERIFY(srs_res_set.srs_res_set_usage == srs_usage::codebook, "Only SRS resource set usage \"codebook\" is supported");

  const auto& srs_res = srs_cfg.srs_res_list.front();
  VERIFY(
      (std::holds_alternative<srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type) and
       srs_res.res_type == srs_resource_type::aperiodic) or
          (std::holds_alternative<srs_config::srs_resource_set::periodic_resource_type>(srs_res_set.res_type) and
           srs_res.res_type == srs_resource_type::periodic) or
          (std::holds_alternative<srs_config::srs_resource_set::semi_persistent_resource_type>(srs_res_set.res_type) and
           srs_res.res_type == srs_resource_type::semi_persistent),
      "The SRS resource set and its resource should be of the same type");
  if (srs_res.res_type == srs_resource_type::periodic) {
    VERIFY(srs_res.periodicity_and_offset.has_value(),
           "The SRS resource should have a periodicity and offset when the resource type is periodic");
    VERIFY(srs_res.periodicity_and_offset.value().offset <
               static_cast<unsigned>(srs_res.periodicity_and_offset.value().period),
           "The SRS resource offset should be less than the periodicity");
  }
  VERIFY(srs_res.tx_comb.tx_comb_offset < static_cast<uint8_t>(srs_res.tx_comb.size),
         "The SRS resource txCombOffset should be less than the TX comb size");
  const uint8_t max_tx_comb_cs = srs_res.tx_comb.size == tx_comb_size::n2 ? 7U : 11U;
  VERIFY(srs_res.tx_comb.tx_comb_cyclic_shift <= max_tx_comb_cs,
         "The SRS resource tx_comb_cyclic_shift should be less than or equal to 7 for TX comb size n2 and TX comb size "
         "11 for n4");
  VERIFY(srs_res.res_mapping.rept_factor <= srs_res.res_mapping.nof_symb,
         "The SRS resource repetition factor should be less than or equal to the number of symbols");
  // NOTE: The parameter \c start_pos indicates the distance from the last symbol of the slot. The actual starting
  // OFDM symbol is NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - (srs_res.res_mapping.start_pos + 1).
  // The final symbol =
  //         NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - (srs_res.res_mapping.start_pos + 1) + srs_res.res_mapping.nof_symb
  // needs to be less than or equal to NOF_OFDM_SYM_PER_SLOT_NORMAL_CP.
  VERIFY(static_cast<uint8_t>(srs_res.res_mapping.nof_symb) <= srs_res.res_mapping.start_pos + 1,
         "The SRS resource number of symbols and start position should be such that the SRS resource fits within the "
         "slot symbols");

  return {};
}

validator_result
config_validators::validate_nzp_csi_rs_list(span<const nzp_csi_rs_resource>               nzp_csi_rs_res_list,
                                            const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common)
{
  // Check: NZP-CSI-RS Resource List ID uniqueness
  VERIFY(has_unique_ids(nzp_csi_rs_res_list, &nzp_csi_rs_resource::res_id), "Duplication of NZP-CSI-RS-ResourceId");

  // Check: Verify firstOFDMSymbolInTimeDomain2 parameter.
  for (const auto& res : nzp_csi_rs_res_list) {
    const auto&   res_mapping = res.res_mapping;
    const uint8_t row_idx     = csi_rs::get_csi_rs_resource_mapping_row_number(
        res_mapping.nof_ports, res_mapping.freq_density, res_mapping.cdm, res_mapping.fd_alloc);
    // As per Table 7.4.1.5.3-1,Section 38.211, the parameter firstOFDMSymbolInTimeDomain2 for symbol \f$l_1\f$
    // should be given by higher layers for Tables rows 13, 14, 16, 17.
    if (row_idx == 13 or row_idx == 14 or row_idx == 16 or row_idx == 17) {
      VERIFY(res_mapping.first_ofdm_symbol_in_td2.has_value(),
             "Missing parameter firstOFDMSymbolInTimeDomain2 for NZP-CSI-RS Resource Id. {} ",
             fmt::underlying(res.res_id));
    }
  }

  // Check: Verify that NZP-CSI-RS do not collide with each other.
  for (const auto* it = nzp_csi_rs_res_list.begin(); it != nzp_csi_rs_res_list.end(); ++it) {
    if (it->csi_res_offset.has_value() and it->csi_res_period.has_value()) {
      for (const auto* it2 = it + 1; it2 != nzp_csi_rs_res_list.end(); ++it2) {
        if (it2->csi_res_offset.has_value() and it2->csi_res_period.has_value()) {
          VERIFY(it->csi_res_period.value() == it2->csi_res_period.value(),
                 "NZP-CSI-RS resources with different periods not supported");

          if (*it->csi_res_offset != *it2->csi_res_offset) {
            continue;
          }
          // In case that match in slot offset, ensure symbols are different.
          VERIFY(it->res_mapping.first_ofdm_symbol_in_td != it2->res_mapping.first_ofdm_symbol_in_td,
                 "NZP-CSI-RS resources with same slot offset and same symbol");
          // TODO: Extend to support CSI-RS in the same symbols, but different frequency resources.
        }
      }
    }
  }

  // Check: Verify if CSI-RS symbols allocation are on DL symbols.
  if (tdd_cfg_common.has_value()) {
    for (const nzp_csi_rs_resource& res : nzp_csi_rs_res_list) {
      // Period and offset are specified only for periodic and semi-persistent NZP-CSI-RS-Resources.
      if (res.csi_res_offset.has_value() and res.csi_res_period.has_value()) {
        // Get the symbol mapping from the NZ-CSI-RS configuration.
        const auto&   res_mapping = res.res_mapping;
        const uint8_t row_idx     = csi_rs::get_csi_rs_resource_mapping_row_number(
            res_mapping.nof_ports, res_mapping.freq_density, res_mapping.cdm, res_mapping.fd_alloc);
        csi_rs_pattern_configuration csi_rs_cfg{.start_rb                 = res_mapping.freq_band_rbs.start(),
                                                .nof_rb                   = res_mapping.freq_band_rbs.length(),
                                                .csi_rs_mapping_table_row = static_cast<unsigned>(row_idx),
                                                .symbol_l0                = res_mapping.first_ofdm_symbol_in_td,
                                                .cdm                      = res_mapping.cdm,
                                                .freq_density             = res_mapping.freq_density};
        // symbol_l1 is only used in some configuration, and might not be provided by the higher layers; in such
        // cases, we set an invalid value for symbol_l1, as a way to let the PHY know this value should not be used.
        csi_rs_cfg.symbol_l1 =
            res_mapping.first_ofdm_symbol_in_td2.has_value() ? res_mapping.first_ofdm_symbol_in_td2.value() : 0;
        csi_rs::convert_freq_domain(
            csi_rs_cfg.freq_allocation_ref_idx, res.res_mapping.fd_alloc, static_cast<unsigned>(row_idx));

        const unsigned tdd_period_slots    = nof_slots_per_tdd_period(tdd_cfg_common.value());
        const unsigned csi_rs_period_slots = csi_resource_periodicity_to_uint(res.csi_res_period.value());

        VERIFY(csi_rs_period_slots % tdd_period_slots == 0,
               "Period={} of NZP-CSI-RS-ResourceId={} is not a multiple of the TDD pattern period={}",
               csi_rs_period_slots,
               fmt::underlying(res.res_id),
               tdd_period_slots);

        const cyclic_prefix     cp{cyclic_prefix::NORMAL};
        const ofdm_symbol_range dl_symbols =
            get_active_tdd_dl_symbols(tdd_cfg_common.value(), res.csi_res_offset.value() % tdd_period_slots, cp);

        VERIFY(not dl_symbols.empty(),
               "NZP-CSI-RS-ResourceId={} with offset={} is scheduled in slot={} with no DL symbols",
               fmt::underlying(res.res_id),
               res.csi_res_offset.value(),
               res.csi_res_offset.value() % tdd_period_slots);

        if (dl_symbols.length() != get_nsymb_per_slot(cp)) {
          // In case of special slot.
          const csi_rs_pattern csi_res_mapping = get_csi_rs_pattern(csi_rs_cfg);
          unsigned             max_symbol_idx  = csi_res_mapping.get_reserved_pattern().symbol_mask.find_highest();
          VERIFY(max_symbol_idx < dl_symbols.stop(),
                 "NZP-CSI-RS-ResourceId={}, with slot offset {} and symbol index {}, falls outside DL symbols {} in "
                 "special slot.",
                 fmt::underlying(res.res_id),
                 res.csi_res_offset.value(),
                 max_symbol_idx,
                 dl_symbols);
        }
      }
    }
  }

  return {};
}

validator_result config_validators::validate_csi_meas_cfg(const serving_cell_config&                    ue_cell_cfg,
                                                          const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common)
{
  if (not ue_cell_cfg.csi_meas_cfg.has_value()) {
    return {};
  }
  const csi_meas_config& csi_meas_cfg = ue_cell_cfg.csi_meas_cfg.value();

  HANDLE_ERROR(validate_nzp_csi_rs_list(csi_meas_cfg.nzp_csi_rs_res_list, tdd_cfg_common));

  // Ensure no Resource and ResourceSet ID duplications.
  // > NZP-CSI-RS ResourceSet
  VERIFY(has_unique_ids(csi_meas_cfg.nzp_csi_rs_res_set_list, &nzp_csi_rs_resource_set::res_set_id),
         "Duplication of NZP-CSI-RS-ResourceSetId");
  std::optional<nzp_csi_rs_res_id_t> nzp_id = find_disconnected_id(csi_meas_cfg.nzp_csi_rs_res_set_list,
                                                                   csi_meas_cfg.nzp_csi_rs_res_list,
                                                                   &nzp_csi_rs_resource_set::nzp_csi_rs_res,
                                                                   &nzp_csi_rs_resource::res_id);
  VERIFY(not nzp_id.has_value(),
         "ResourceId={} in NZP-CSI-RS-ResourceSet has no associated NZP-CSI-RS-Resource",
         fmt::underlying(*nzp_id));
  // > CSI-IM-ResourceList
  VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_list, &csi_im_resource::res_id), "Duplication of CSI-IM-ResourceId");
  // > CSI-IM-ResourceSetList.
  VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_set_list, &csi_im_resource_set::res_set_id),
         "Duplication of CSI-IM-ResourceSetId");
  std::optional<csi_im_res_id_t> im_id = find_disconnected_id(csi_meas_cfg.csi_im_res_set_list,
                                                              csi_meas_cfg.csi_im_res_list,
                                                              &csi_im_resource_set::csi_ims_resources,
                                                              &csi_im_resource::res_id);
  VERIFY(not im_id.has_value(),
         "ResourceId={} in CSI-IM-ResourceSet has no associated CSI-IM-Resource",
         fmt::underlying(*im_id));
  VERIFY(has_unique_ids(csi_meas_cfg.csi_ssb_res_set_list, &csi_ssb_resource_set::res_set_id),
         "Duplication of CSI-SSB-ResourceSetId");
  VERIFY(has_unique_ids(csi_meas_cfg.csi_res_cfg_list, &csi_resource_config::res_cfg_id),
         "Duplication of CSI-ResourceConfigId");
  VERIFY(has_unique_ids(csi_meas_cfg.csi_report_cfg_list, &csi_report_config::report_cfg_id),
         "Duplication of CSI-ReportConfigId");

  // CSI-ResourceConfig.
  for (const auto& res_cfg : csi_meas_cfg.csi_res_cfg_list) {
    if (std::holds_alternative<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list)) {
      const auto& variant_value = std::get<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list);
      for (const auto& res_set_id : variant_value.nzp_csi_rs_res_set_list) {
        VERIFY_ID_EXISTS([res_set_id](const nzp_csi_rs_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.nzp_csi_rs_res_set_list,
                         "NZP CSI-RS resource set id={} does not exist",
                         fmt::underlying(res_set_id));
      }
      for (const auto& res_set_id : variant_value.csi_ssb_res_set_list) {
        VERIFY_ID_EXISTS([res_set_id](const csi_ssb_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.csi_ssb_res_set_list,
                         "CSI SSB resource set id={} does not exist",
                         fmt::underlying(res_set_id));
      }
    } else if (std::holds_alternative<csi_resource_config::csi_im_resource_set_list>(res_cfg.csi_rs_res_set_list)) {
      const auto& variant_value = std::get<csi_resource_config::csi_im_resource_set_list>(res_cfg.csi_rs_res_set_list);
      for (const auto& res_set_id : variant_value) {
        VERIFY_ID_EXISTS([res_set_id](const csi_im_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.csi_im_res_set_list,
                         "CSI IM resource set id={} does not exist",
                         fmt::underlying(res_set_id));
      }
    }
    // TODO: BWP-Id and Resource Type validation.
  }
  // CSI-ReportConfig.
  for (const auto& rep_cfg : csi_meas_cfg.csi_report_cfg_list) {
    const auto res_for_channel_meas = rep_cfg.res_for_channel_meas;
    VERIFY_ID_EXISTS(
        [res_for_channel_meas](const csi_resource_config& rhs) { return rhs.res_cfg_id == res_for_channel_meas; },
        csi_meas_cfg.csi_res_cfg_list,
        "CSI Recourse Config id={} does not exist",
        fmt::underlying(res_for_channel_meas));

    if (rep_cfg.csi_im_res_for_interference.has_value()) {
      const auto csi_im_res_for_interference = rep_cfg.csi_im_res_for_interference.value();
      VERIFY_ID_EXISTS([csi_im_res_for_interference](
                           const csi_resource_config& rhs) { return rhs.res_cfg_id == csi_im_res_for_interference; },
                       csi_meas_cfg.csi_res_cfg_list,
                       "CSI Recourse Config id={} does not exist",
                       fmt::underlying(csi_im_res_for_interference));
    }

    if (rep_cfg.nzp_csi_rs_res_for_interference.has_value()) {
      const auto nzp_csi_rs_res_for_interference = rep_cfg.nzp_csi_rs_res_for_interference.value();
      VERIFY_ID_EXISTS(
          [nzp_csi_rs_res_for_interference](const csi_resource_config& rhs) {
            return rhs.res_cfg_id == nzp_csi_rs_res_for_interference;
          },
          csi_meas_cfg.csi_res_cfg_list,
          "CSI Recourse Config id={} does not exist",
          fmt::underlying(nzp_csi_rs_res_for_interference));
    }

    if (std::holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
            rep_cfg.report_cfg_type)) {
      const auto& pucch_csi =
          std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(rep_cfg.report_cfg_type);
      VERIFY(ue_cell_cfg.ul_config.has_value(), "Cell does not define a UL Config");
      VERIFY(ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value(), "Cell={} does not define a PUCCH Config");
      const auto& pucch_resources = ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list;
      for (const auto& pucch_res : pucch_csi.pucch_csi_res_list) {
        const auto& pucch_res_id = pucch_res.pucch_res_id;
        VERIFY_ID_EXISTS([pucch_res_id](const pucch_resource& rhs) { return rhs.res_id == pucch_res_id; },
                         pucch_resources,
                         "PUCCH resource id={} does not exist",
                         pucch_res_id.cell_res_id);
      }
    }
  }

  // TODO: Validate other parameters.
  return {};
}
