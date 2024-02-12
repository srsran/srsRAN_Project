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

#include "srsran/scheduler/config/serving_cell_config_validator.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/csi_rs/csi_rs_config_helpers.h"
#include "srsran/ran/pucch/pucch_info.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/config/validator_helpers.h"

using namespace srsran;

#define VERIFY_ID_EXISTS(cond_lambda, id_list, ...)                                                                    \
  if (std::find_if(id_list.begin(), id_list.end(), cond_lambda) == id_list.end()) {                                    \
    return error_type<std::string>(fmt::format(__VA_ARGS__));                                                          \
  }

validator_result srsran::config_validators::validate_pdcch_cfg(const serving_cell_config& ue_cell_cfg,
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
             ss.get_coreset_id(),
             ss.get_id());
    }
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
           csi_im.csi_res_period,
           csi_im.csi_res_offset,
           csi_im.freq_band_rbs,
           csi_im.csi_im_res_element_pattern->symbol_location);
  }
  return {};
}

validator_result srsran::config_validators::validate_pdsch_cfg(const serving_cell_config& ue_cell_cfg)
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

validator_result srsran::config_validators::validate_pucch_cfg(const serving_cell_config& ue_cell_cfg,
                                                               unsigned                   nof_dl_antennas)
{
  VERIFY(ue_cell_cfg.ul_config.has_value() and ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value(),
         "Missing configuration for uplinkConfig or pucch-Config in spCellConfig");

  const auto& pucch_cfg = ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Helper to retrives a given PUCCH resource given its ID from the PUCCH resource list.
  auto get_pucch_resource_with_id = [&pucch_cfg](unsigned res_id) {
    return std::find_if(pucch_cfg.pucch_res_list.begin(),
                        pucch_cfg.pucch_res_list.end(),
                        [res_id](const pucch_resource& res) { return res.res_id.cell_res_id == res_id; });
  };

  VERIFY(pucch_cfg.format_1_common_param.has_value(), "Missing PUCCH-format1 parameters in PUCCH-Config");
  VERIFY(pucch_cfg.format_2_common_param.has_value(), "Missing PUCCH-format2 parameters in PUCCH-Config");

  // Verify that the PUCCH resources IDs of each PUCCH resource set point at a corresponding item in the PUCCH reource
  // list.
  VERIFY(pucch_cfg.pucch_res_set.size() >= 2, "At least 2 PUCCH resource sets need to be configured in PUCCH-Config");
  VERIFY(pucch_cfg.pucch_res_set[0].pucch_res_set_id == 0 and pucch_cfg.pucch_res_set[1].pucch_res_set_id == 1,
         "PUCCH resouce sets 0 and 1 are expected to have PUCCH-ResourceSetId 0 and 1, respectively");
  VERIFY((not pucch_cfg.pucch_res_set[0].pucch_res_id_list.empty()) and
             (not pucch_cfg.pucch_res_set[1].pucch_res_id_list.empty()),
         "PUCCH resouce sets 0 and 1 are expected to have a non-empty set of PUCCH resource id");
  for (size_t pucch_res_set_idx = 0; pucch_res_set_idx != 2; ++pucch_res_set_idx) {
    for (auto res_idx : pucch_cfg.pucch_res_set[pucch_res_set_idx].pucch_res_id_list) {
      const auto* pucch_res_it = get_pucch_resource_with_id(res_idx.cell_res_id);
      VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_it,
             "PUCCH cell res. id={} in PUCCH res. set id={} not found in the PUCCH resource list",
             res_idx.cell_res_id,
             pucch_res_set_idx);
    }
  }

  // Verify each resource format matches the corresponding parameters.
  for (auto res : pucch_cfg.pucch_res_list) {
    const bool format_match_format_params =
        (res.format == pucch_format::FORMAT_0 and variant_holds_alternative<pucch_format_0_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_1 and variant_holds_alternative<pucch_format_1_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_2 and variant_holds_alternative<pucch_format_2_3_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_3 and variant_holds_alternative<pucch_format_2_3_cfg>(res.format_params)) or
        (res.format == pucch_format::FORMAT_4 and variant_holds_alternative<pucch_format_4_cfg>(res.format_params));
    VERIFY(format_match_format_params,
           "PUCCH cell res id={} format does not match the PUCCH format parameters",
           res.res_id.cell_res_id);
  }

  // Check PUCCH Formats for each PUCCH Resource Set.
  for (auto res_idx : pucch_cfg.pucch_res_set[0].pucch_res_id_list) {
    const auto* pucch_res_it = get_pucch_resource_with_id(res_idx.cell_res_id);
    VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_it and pucch_res_it->format == pucch_format::FORMAT_1,
           "Only PUCCH Resource Format 1 expected in PUCCH resource set 0.");
  }
  for (auto res_idx : pucch_cfg.pucch_res_set[1].pucch_res_id_list) {
    const auto* pucch_res_it = get_pucch_resource_with_id(res_idx.cell_res_id);
    VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_it and pucch_res_it->format == pucch_format::FORMAT_2,
           "Only PUCCH Resource Format 2 expected in PUCCH resource set 1.");
  }

  // Verify the PUCCH resource id that indicated in the SR resource config exists in the PUCCH resource list.
  VERIFY(pucch_cfg.sr_res_list.size() == 1, "Only SchedulingRequestResourceConfig with size 1 supported");
  VERIFY(pucch_cfg.pucch_res_list.end() !=
             get_pucch_resource_with_id(pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id),
         "PUCCH cell res. id={} given in SR resource config not found in the PUCCH resource list",
         pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id);

  const auto* pucch_res_sr = get_pucch_resource_with_id(pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id);
  VERIFY(pucch_cfg.pucch_res_list.end() != pucch_res_sr,
         "PUCCH cell res. id={} for SR could not be found in PUCCH resource list",
         pucch_cfg.sr_res_list.front().pucch_res_id.cell_res_id);
  VERIFY(pucch_res_sr->format == pucch_format::FORMAT_1, "PUCCH resource used for SR is expected to be Format 1");

  // Verify that the PUCCH setting used for CSI report have been configured properly.
  if (ue_cell_cfg.csi_meas_cfg.has_value()) {
    const auto& csi_cfg = ue_cell_cfg.csi_meas_cfg.value();
    VERIFY(not csi_cfg.csi_report_cfg_list.empty() and
               variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                   csi_cfg.csi_report_cfg_list.front().report_cfg_type) and
               not variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                       csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                       .pucch_csi_res_list.empty(),
           "PUCCH-CSI-ResourceList has not been configured in the CSI-reportConfig");

    const auto& csi = variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        csi_cfg.csi_report_cfg_list.front().report_cfg_type);
    const unsigned csi_res_id = csi.pucch_csi_res_list.front().pucch_res_id.cell_res_id;
    // Verify the PUCCH resource id that indicated in the CSI resource config exists in the PUCCH resource list.
    const auto* csi_pucch_res_id = get_pucch_resource_with_id(csi_res_id);
    VERIFY(csi_pucch_res_id != pucch_cfg.pucch_res_list.end(),
           "PUCCH cell res. id={} given in PUCCH-CSI-resourceList not found in the PUCCH resource list",
           csi_res_id);
    VERIFY(csi_pucch_res_id->format == pucch_format::FORMAT_2,
           "PUCCH resource used for CSI is expected to be Format 2");

    // Verify the CSI/SR bits do not exceed the PUCCH F2 payload.
    const auto&    csi_pucch_res_params = variant_get<pucch_format_2_3_cfg>(csi_pucch_res_id->format_params);
    const unsigned pucch_f2_max_payload =
        get_pucch_format2_max_payload(csi_pucch_res_params.nof_prbs,
                                      csi_pucch_res_params.nof_symbols,
                                      to_max_code_rate_float(pucch_cfg.format_2_common_param.value().max_c_rate));
    const auto     csi_report_cfg  = create_csi_report_configuration(ue_cell_cfg.csi_meas_cfg.value());
    const unsigned csi_report_size = get_csi_report_pucch_size(csi_report_cfg).value();
    unsigned       sr_offset       = pucch_cfg.sr_res_list.front().offset;
    // If SR and CSI are reported within the same slot, 1 SR bit can be multiplexed with CSI within the same PUCCH
    // resource.
    const unsigned csi_period    = csi_report_periodicity_to_uint(csi.report_slot_period);
    const unsigned lowest_period = std::min(sr_periodicity_to_slot(pucch_cfg.sr_res_list.front().period), csi_period);
    unsigned sr_bits_mplexed_with_csi = sr_offset % lowest_period != csi.report_slot_offset % lowest_period ? 0U : 1U;
    // In the PUCCH resource for CSI, there are no HARQ-ACK bits being reported; therefore we only need to check where
    // the CSI + SR bits fit into the max payload.
    const unsigned uci_bits_pucch_resource = csi_report_size + sr_bits_mplexed_with_csi;
    VERIFY(pucch_f2_max_payload >= uci_bits_pucch_resource,
           "UCI num. of bits ({}) exceeds the maximum CSI's PUCCH Format 2 payload ({})",
           uci_bits_pucch_resource,
           pucch_f2_max_payload);

    // Although all PUCCH Format 2 resource have the same parameters, we check the max payload for all Format 2
    // resources for HARQ-ACK.
    // For 1 or 2 antennas tx, 2 HARQ bits can be multiplexed with CSI within the same PUCCH resource.
    unsigned       harq_bits_mplexed_with_csi = nof_dl_antennas > 2 ? 0U : 2U;
    const unsigned uci_bits_harq_resource     = csi_report_size + harq_bits_mplexed_with_csi + sr_bits_mplexed_with_csi;
    const unsigned pucch_res_set_idx_for_f2   = 1;
    for (pucch_res_id_t res_idx : pucch_cfg.pucch_res_set[pucch_res_set_idx_for_f2].pucch_res_id_list) {
      auto*          res_f2_it                = get_pucch_resource_with_id(res_idx.cell_res_id);
      const auto&    harq_f2_pucch_res_params = variant_get<pucch_format_2_3_cfg>(res_f2_it->format_params);
      const unsigned pucch_harq_f2_max_payload =
          get_pucch_format2_max_payload(harq_f2_pucch_res_params.nof_prbs,
                                        harq_f2_pucch_res_params.nof_symbols,
                                        to_max_code_rate_float(pucch_cfg.format_2_common_param.value().max_c_rate));
      VERIFY(pucch_harq_f2_max_payload >= uci_bits_harq_resource,
             "UCI num. of bits ({}) exceeds the maximum HARQ-ACK's PUCCH Format 2 payload ({})",
             uci_bits_harq_resource,
             pucch_harq_f2_max_payload);
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

validator_result
srsran::config_validators::validate_csi_meas_cfg(const serving_cell_config&               ue_cell_cfg,
                                                 const optional<tdd_ul_dl_config_common>& tdd_cfg_common)
{
  if (not ue_cell_cfg.csi_meas_cfg.has_value()) {
    return {};
  }
  const csi_meas_config& csi_meas_cfg = ue_cell_cfg.csi_meas_cfg.value();

  // Ensure no Resource and ResourceSet ID duplications.
  // > NZP-CSI-RS Resource List
  VERIFY(has_unique_ids(csi_meas_cfg.nzp_csi_rs_res_list, &nzp_csi_rs_resource::res_id),
         "Duplication of NZP-CSI-RS-ResourceId");
  // > NZP-CSI-RS ResourceSet
  VERIFY(has_unique_ids(csi_meas_cfg.nzp_csi_rs_res_set_list, &nzp_csi_rs_resource_set::res_set_id),
         "Duplication of NZP-CSI-RS-ResourceSetId");
  optional<nzp_csi_rs_res_id_t> nzp_id = find_disconnected_id(csi_meas_cfg.nzp_csi_rs_res_set_list,
                                                              csi_meas_cfg.nzp_csi_rs_res_list,
                                                              &nzp_csi_rs_resource_set::nzp_csi_rs_res,
                                                              &nzp_csi_rs_resource::res_id);
  VERIFY(
      not nzp_id.has_value(), "ResourceId={} in NZP-CSI-RS-ResourceSet has no associated NZP-CSI-RS-Resource", *nzp_id);
  // > CSI-IM-ResourceList
  VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_list, &csi_im_resource::res_id), "Duplication of CSI-IM-ResourceId");
  // > CSI-IM-ResourceSetList.
  VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_set_list, &csi_im_resource_set::res_set_id),
         "Duplication of CSI-IM-ResourceSetId");
  optional<csi_im_res_id_t> im_id = find_disconnected_id(csi_meas_cfg.csi_im_res_set_list,
                                                         csi_meas_cfg.csi_im_res_list,
                                                         &csi_im_resource_set::csi_ims_resources,
                                                         &csi_im_resource::res_id);
  VERIFY(not im_id.has_value(), "ResourceId={} in CSI-IM-ResourceSet has no associated CSI-IM-Resource", *im_id);
  VERIFY(has_unique_ids(csi_meas_cfg.csi_ssb_res_set_list, &csi_ssb_resource_set::res_set_id),
         "Duplication of CSI-SSB-ResourceSetId");
  VERIFY(has_unique_ids(csi_meas_cfg.csi_res_cfg_list, &csi_resource_config::res_cfg_id),
         "Duplication of CSI-ResourceConfigId");
  VERIFY(has_unique_ids(csi_meas_cfg.csi_report_cfg_list, &csi_report_config::report_cfg_id),
         "Duplication of CSI-ReportConfigId");

  // NZP-CSI-RS-Resource List. Verify firstOFDMSymbolInTimeDomain2 parameter.
  for (const auto& res : csi_meas_cfg.nzp_csi_rs_res_list) {
    const auto&   res_mapping = res.res_mapping;
    const uint8_t row_idx     = csi_rs::get_csi_rs_resource_mapping_row_number(
        res_mapping.nof_ports, res_mapping.freq_density, res_mapping.cdm, res_mapping.fd_alloc);
    // As per Table 7.4.1.5.3-1,Section 38.211, the parameter firstOFDMSymbolInTimeDomain2 for symbol \f$l_1\f$
    // should be given by higher layers for Tables rows 13, 14, 16, 17.
    if (row_idx == 13 or row_idx == 14 or row_idx == 16 or row_idx == 17) {
      VERIFY(res_mapping.first_ofdm_symbol_in_td2.has_value(),
             "Missing parameter firstOFDMSymbolInTimeDomain2 for NZP-CSI-RS Resource Id. {} ",
             res.res_id);
    }
  }

  // NZP-CSI-RS-ResourceList. Verify that NZP-CSI-RS do not collide with each other.
  for (auto it = csi_meas_cfg.nzp_csi_rs_res_list.begin(); it != csi_meas_cfg.nzp_csi_rs_res_list.end(); ++it) {
    if (it->csi_res_offset.has_value() and it->csi_res_period.has_value()) {
      for (auto it2 = it + 1; it2 != csi_meas_cfg.nzp_csi_rs_res_list.end(); ++it2) {
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

  // NZP-CSI-RS-ResourceList. Verify if CSI-RS symbols allocation are on DL symbols.
  if (tdd_cfg_common.has_value()) {
    for (const nzp_csi_rs_resource& res : csi_meas_cfg.nzp_csi_rs_res_list) {
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
               res.res_id,
               tdd_period_slots);

        const cyclic_prefix     cp{cyclic_prefix::NORMAL};
        const ofdm_symbol_range dl_symbols =
            get_active_tdd_dl_symbols(tdd_cfg_common.value(), res.csi_res_offset.value() % tdd_period_slots, cp);

        VERIFY(not dl_symbols.empty(),
               "NZP-CSI-RS-ResourceId={} with offset={} is scheduled in slot={} with no DL symbols",
               res.res_id,
               res.csi_res_offset.value(),
               res.csi_res_offset.value() % tdd_period_slots);

        if (dl_symbols.length() != get_nsymb_per_slot(cp)) {
          // In case of special slot.
          const csi_rs_pattern csi_res_mapping = get_csi_rs_pattern(csi_rs_cfg);
          VERIFY(csi_res_mapping.get_reserved_pattern().symbol_mask.find_highest() < dl_symbols.stop(),
                 "NZP-CSI-RS-ResourceId={} with symbol mask={} would be scheduled in non-DL symbols",
                 res.res_id,
                 csi_res_mapping.get_reserved_pattern().symbol_mask);
        }
      }
    }
  }
  // CSI-ResourceConfig.
  for (const auto& res_cfg : csi_meas_cfg.csi_res_cfg_list) {
    if (variant_holds_alternative<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list)) {
      const auto& variant_value = variant_get<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list);
      for (const auto& res_set_id : variant_value.nzp_csi_rs_res_set_list) {
        VERIFY_ID_EXISTS([res_set_id](const nzp_csi_rs_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.nzp_csi_rs_res_set_list,
                         "NZP CSI-RS resource set id={} does not exist",
                         res_set_id);
      }
      for (const auto& res_set_id : variant_value.csi_ssb_res_set_list) {
        VERIFY_ID_EXISTS([res_set_id](const csi_ssb_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.csi_ssb_res_set_list,
                         "CSI SSB resource set id={} does not exist",
                         res_set_id);
      }
    } else if (variant_holds_alternative<csi_resource_config::csi_im_resource_set_list>(res_cfg.csi_rs_res_set_list)) {
      const auto& variant_value =
          variant_get<csi_resource_config::csi_im_resource_set_list>(res_cfg.csi_rs_res_set_list);
      for (const auto& res_set_id : variant_value) {
        VERIFY_ID_EXISTS([res_set_id](const csi_im_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.csi_im_res_set_list,
                         "CSI IM resource set id={} does not exist",
                         res_set_id);
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
        res_for_channel_meas);

    if (rep_cfg.csi_im_res_for_interference.has_value()) {
      const auto csi_im_res_for_interference = rep_cfg.csi_im_res_for_interference.value();
      VERIFY_ID_EXISTS([csi_im_res_for_interference](
                           const csi_resource_config& rhs) { return rhs.res_cfg_id == csi_im_res_for_interference; },
                       csi_meas_cfg.csi_res_cfg_list,
                       "CSI Recourse Config id={} does not exist",
                       csi_im_res_for_interference);
    }

    if (rep_cfg.nzp_csi_rs_res_for_interference.has_value()) {
      const auto nzp_csi_rs_res_for_interference = rep_cfg.nzp_csi_rs_res_for_interference.value();
      VERIFY_ID_EXISTS(
          [nzp_csi_rs_res_for_interference](const csi_resource_config& rhs) {
            return rhs.res_cfg_id == nzp_csi_rs_res_for_interference;
          },
          csi_meas_cfg.csi_res_cfg_list,
          "CSI Recourse Config id={} does not exist",
          nzp_csi_rs_res_for_interference);
    }

    if (variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
            rep_cfg.report_cfg_type)) {
      const auto& pucch_csi =
          variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(rep_cfg.report_cfg_type);
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
