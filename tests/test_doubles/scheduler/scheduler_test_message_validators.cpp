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

#include "scheduler_test_message_validators.h"
#include "../lib/scheduler/support/dmrs_helpers.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"
#include "srsran/ran/uci/uci_mapping.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/scheduler/result/pusch_info.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

template <typename... Args>
void print_if_present(Args... args)
{
  if constexpr (sizeof...(args) > 0) {
    srslog::fetch_basic_logger("TEST").warning(args...);
    srslog::flush();
  }
}

#define TRUE_OR_RETURN(cond, ...)                                                                                      \
  if (not(cond)) {                                                                                                     \
    print_if_present(__VA_ARGS__);                                                                                     \
    return false;                                                                                                      \
  }

/// Check that the CRBs are within the allowed BWP/CORESET boundaries.
static bool are_crbs_valid(const pdsch_information& pdsch, const std::optional<coreset_configuration>& coreset0)
{
  const vrb_interval vrbs = pdsch.rbs.type1();

  crb_interval crb_lims;
  if (pdsch.dci_fmt == dci_dl_format::f1_0 and pdsch.ss_set_type != search_space_set_type::ue_specific) {
    const unsigned coreset_start_crb = pdsch.coreset_cfg->get_coreset_start_crb();
    if (coreset0.has_value()) {
      crb_lims = crb_interval{coreset_start_crb, coreset_start_crb + coreset0.value().coreset0_crbs().length()};
    } else {
      crb_lims = crb_interval{coreset_start_crb, coreset_start_crb + pdsch.bwp_cfg->crbs.length()};
    }
  } else {
    crb_lims = pdsch.bwp_cfg->crbs;
  }

  // TODO: support interleaving.
  const crb_interval crbs = prb_to_crb(crb_lims, prb_interval{vrbs.start(), vrbs.stop()});
  TRUE_OR_RETURN(crb_lims.contains(crbs));
  return true;
}

static bool is_pdsch_info_valid(const pdsch_information& pdsch, const std::optional<coreset_configuration>& coreset0)
{
  TRUE_OR_RETURN(pdsch.coreset_cfg != nullptr);
  TRUE_OR_RETURN(pdsch.bwp_cfg != nullptr);
  TRUE_OR_RETURN(not pdsch.codewords.empty());
  TRUE_OR_RETURN(pdsch.nof_layers > 0);
  TRUE_OR_RETURN(pdsch.codewords[0].tb_size_bytes > 0);
  TRUE_OR_RETURN(are_crbs_valid(pdsch, coreset0));

  switch (pdsch.dci_fmt) {
    case dci_dl_format::f1_0: {
      TRUE_OR_RETURN(pdsch.nof_layers == 1);
      TRUE_OR_RETURN(pdsch.codewords.size() == 1);
      TRUE_OR_RETURN(pdsch.codewords[0].mcs_table == pdsch_mcs_table::qam64);
    } break;
    case dci_dl_format::f1_1: {
      TRUE_OR_RETURN(pdsch.coreset_cfg->id != to_coreset_id(0));
    } break;
    default:
      report_fatal_error("DCI format not supported");
  }

  return true;
}

bool test_helper::is_valid_dl_msg_alloc(const dl_msg_alloc& grant, const std::optional<coreset_configuration>& coreset0)
{
  TRUE_OR_RETURN(is_pdsch_info_valid(grant.pdsch_cfg, coreset0));
  TRUE_OR_RETURN(grant.pdsch_cfg.codewords[0].new_data == not grant.tb_list.empty());
  TRUE_OR_RETURN(grant.tb_list.size() <= 2);

  if (grant.pdsch_cfg.dci_fmt == dci_dl_format::f1_0) {
    TRUE_OR_RETURN(grant.tb_list.size() <= 1);
  }

  return true;
}

// Fetch ulsch_configuration based on PUSCH grant.
static ulsch_configuration get_ulsch_config(const ul_sched_info& grant)
{
  ulsch_configuration ulsch_cfg{};
  ulsch_cfg.tbs       = units::bytes{grant.pusch_cfg.tb_size_bytes}.to_bits();
  ulsch_cfg.mcs_descr = grant.pusch_cfg.mcs_descr;

  if (grant.uci.has_value()) {
    ulsch_cfg.alpha_scaling = alpha_scaling_to_float(grant.uci.value().alpha);
    if (grant.uci.value().harq.has_value()) {
      const auto& harq_info          = grant.uci.value().harq.value();
      ulsch_cfg.nof_harq_ack_bits    = units::bits{harq_info.harq_ack_nof_bits};
      ulsch_cfg.beta_offset_harq_ack = beta_harq_ack_to_float(harq_info.beta_offset_harq_ack);
    }
    if (grant.uci.value().csi.has_value()) {
      const auto& csi_info            = grant.uci.value().csi.value();
      ulsch_cfg.nof_csi_part1_bits    = units::bits{csi_info.csi_part1_nof_bits};
      ulsch_cfg.nof_csi_part2_bits    = units::bits{0U}; // TODO
      ulsch_cfg.beta_offset_csi_part1 = beta_csi_to_float(csi_info.beta_offset_csi_1);
      ulsch_cfg.beta_offset_csi_part2 =
          csi_info.beta_offset_csi_2.has_value() ? beta_csi_to_float(csi_info.beta_offset_csi_2.value()) : 0;
    }
  }

  ulsch_cfg.nof_rb                      = grant.pusch_cfg.rbs.type1().length();
  ulsch_cfg.start_symbol_index          = grant.pusch_cfg.symbols.start();
  ulsch_cfg.nof_symbols                 = grant.pusch_cfg.symbols.length();
  ulsch_cfg.dmrs_type                   = grant.pusch_cfg.dmrs.config_type;
  ulsch_cfg.dmrs_symbol_mask            = grant.pusch_cfg.dmrs.dmrs_symb_pos;
  ulsch_cfg.nof_cdm_groups_without_data = grant.pusch_cfg.dmrs.num_dmrs_cdm_grps_no_data;
  ulsch_cfg.nof_layers                  = grant.pusch_cfg.nof_layers;
  if (grant.pusch_cfg.tx_direct_current_location < 3300) {
    // Check if DC overlaps with PUSCH in RBs.
    const unsigned     dc_position_crbs = grant.pusch_cfg.tx_direct_current_location / NRE;
    const vrb_interval vrbs             = grant.pusch_cfg.rbs.type1();
    const crb_interval crbs = prb_to_crb(grant.pusch_cfg.bwp_cfg->crbs, prb_interval{vrbs.start(), vrbs.stop()});
    ulsch_cfg.contains_dc   = crbs.contains(dc_position_crbs);
  }

  return ulsch_cfg;
}

static unsigned is_ulsch_tbs_valid(const ul_sched_info& grant)
{
  // As per TS 38.214, Section 5.1.3.2 and 6.1.4.2, and TS 38.212, Section 7.3.1.1 and 7.3.1.2, TB scaling filed is only
  // used for DCI Format 1-0 (in the DL). Therefore, for the PUSCH this is set to 0.
  constexpr unsigned tb_scaling_field = 0;

  unsigned nof_rbs = 0;
  if (grant.pusch_cfg.rbs.is_type1()) {
    nof_rbs = grant.pusch_cfg.rbs.type1().length();
  } else {
    report_fatal_error("Allocation type 0 not supported");
  }

  const unsigned dmrs_prbs = calculate_nof_dmrs_per_rb(grant.pusch_cfg.dmrs);

  unsigned tbs_bytes =
      tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = grant.pusch_cfg.symbols.length(),
                                                            .nof_dmrs_prb     = dmrs_prbs,
                                                            .nof_oh_prb       = grant.context.nof_oh_prb,
                                                            .mcs_descr        = grant.pusch_cfg.mcs_descr,
                                                            .nof_layers       = grant.pusch_cfg.nof_layers,
                                                            .tb_scaling_field = tb_scaling_field,
                                                            .n_prb            = nof_rbs}) /
      8U;

  return grant.pusch_cfg.tb_size_bytes == tbs_bytes;
}

bool test_helper::is_valid_ul_sched_info(const ul_sched_info& grant)
{
  TRUE_OR_RETURN(grant.pusch_cfg.nof_layers > 0);

  // Check code rate.
  const float               max_code_rate       = 0.95;
  const ulsch_configuration ulsch_cfg           = get_ulsch_config(grant);
  const ulsch_information   ulsch_information   = get_ulsch_information(ulsch_cfg);
  float                     effective_code_rate = ulsch_information.get_effective_code_rate();
  TRUE_OR_RETURN(effective_code_rate <= max_code_rate,
                 "rnti={}: Invalid PUSCH code rate ({}>{})",
                 grant.pusch_cfg.rnti,
                 effective_code_rate,
                 max_code_rate);

  // Check TBS.
  TRUE_OR_RETURN(is_ulsch_tbs_valid(grant), "rnti={}: Invalid PUSCH TBS", grant.pusch_cfg.rnti);

  // Check CRBs within BWP.
  if (grant.pusch_cfg.rbs.is_type1()) {
    const vrb_interval vrbs = grant.pusch_cfg.rbs.type1();
    TRUE_OR_RETURN(vrbs.length() > 0);
    if (grant.pusch_cfg.transform_precoding) {
      const bool valid = transform_precoding::is_nof_prbs_valid(vrbs.length());
      TRUE_OR_RETURN(valid, "Invalid number of RBs for transform precoding");
    }
    const crb_interval crbs = prb_to_crb(grant.pusch_cfg.bwp_cfg->crbs, prb_interval{vrbs.start(), vrbs.stop()});
    TRUE_OR_RETURN(grant.pusch_cfg.bwp_cfg->crbs.contains(crbs), "PUSCH outside of BWP boundaries");
  }

  return true;
}

bool test_helper::is_valid_dl_msg_alloc_list(span<const dl_msg_alloc>                    grants,
                                             const std::optional<coreset_configuration>& coreset0)
{
  static_vector<rnti_t, MAX_UE_PDUS_PER_SLOT> rntis;
  for (const auto& grant : grants) {
    TRUE_OR_RETURN(is_valid_dl_msg_alloc(grant, coreset0));

    // Ensure uniqueness of RNTI.
    TRUE_OR_RETURN(std::count(rntis.begin(), rntis.end(), grant.pdsch_cfg.rnti) == 0,
                   "Duplicate RNTI in list of PDSCHs",
                   grant.pdsch_cfg.rnti);
    rntis.push_back(grant.pdsch_cfg.rnti);
  }

  return true;
}
