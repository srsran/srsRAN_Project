/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_cell_config.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"
#include "srsgnb/scheduler/config/cell_config_builder_params.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"

#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include "srsgnb/support/error_handling.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {
namespace config_helpers {

inline scheduler_expert_config make_default_scheduler_expert_config()
{
  scheduler_expert_config cfg;
  cfg.ra.max_nof_msg3_harq_retxs = 4;
  cfg.ra.msg3_mcs_index          = 0;
  cfg.ra.rar_mcs_index           = 0;

  /// As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for SIB1.
  cfg.si.sib1_mcs_index    = 5;
  cfg.si.sib1_retx_period  = sib1_rtx_periodicity::ms160;
  cfg.si.sib1_dci_aggr_lev = aggregation_level::n4;

  cfg.ue.max_nof_harq_retxs = 4;
  cfg.ue.fixed_dl_mcs       = 10;
  cfg.ue.fixed_ul_mcs       = 10;
  /// Note: A MCS index of 7 can handle Msg4 of size 458 bytes.
  cfg.ue.max_msg4_mcs = 7;

  return cfg;
}

inline void build_pucch_guardbands_list(std::vector<sched_grid_resource>& pucch_guardbands)
{
  uplink_config ul_config = make_default_ue_uplink_config();

  auto list_contain_resource = [&pucch_guardbands](const sched_grid_resource& res) {
    return std::find(pucch_guardbands.begin(), pucch_guardbands.end(), res) != pucch_guardbands.end();
  };

  for (const auto& pucch_res : ul_config.init_ul_bwp.pucch_cfg->pucch_res_list) {
    sched_grid_resource res_0, res_1;
    res_0.prbs.set(pucch_res.starting_prb, pucch_res.starting_prb + 1);

    if (pucch_res.intraslot_freq_hopping) {
      res_1.prbs.set(pucch_res.second_hop_prb, pucch_res.second_hop_prb + 1);
      switch (pucch_res.format) {
        case pucch_format::FORMAT_1: {
          res_0.symbols.set(pucch_res.format_1.starting_sym_idx,
                            pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols / 2);
          res_1.symbols.set(pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols / 2,
                            pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols);
          break;
        }
        default:
          srsgnb_terminate("Only PUCCH format 1 currently supported");
      }
    } else {
      switch (pucch_res.format) {
        case pucch_format::FORMAT_1: {
          res_0.symbols.set(pucch_res.format_1.starting_sym_idx,
                            pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols);
          break;
        }
        default:
          srsgnb_terminate("Only PUCCH format 1 currently supported");
      }
    }

    if (not res_0.is_empty() and not list_contain_resource(res_0)) {
      pucch_guardbands.emplace_back(res_0);
    }
    if (not res_1.is_empty() and not list_contain_resource(res_1)) {
      pucch_guardbands.emplace_back(res_1);
    }
  }
}

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline du_cell_config make_default_du_cell_config(const cell_config_builder_params& params = {})
{
  du_cell_config cfg{};
  cfg.pci     = params.pci;
  cfg.plmn    = "00101";
  cfg.tac     = 1;
  cfg.cell_id = 1;

  cfg.dl_carrier       = make_default_carrier_configuration(params);
  cfg.ul_carrier       = make_default_carrier_configuration(params);
  cfg.coreset0_idx     = params.coreset0_index;
  cfg.searchspace0_idx = 0U;
  cfg.dl_cfg_common    = make_default_dl_config_common(params);
  cfg.ul_cfg_common    = make_default_ul_config_common(params);
  cfg.scs_common       = params.scs_common;
  cfg.ssb_cfg          = make_default_ssb_config(params);
  cfg.dmrs_typeA_pos   = dmrs_typeA_position::pos2;
  cfg.cell_barred      = false;
  cfg.intra_freq_resel = false;

  build_pucch_guardbands_list(cfg.pucch_guardbands);

  return cfg;
}

} // namespace config_helpers
} // namespace srsgnb
