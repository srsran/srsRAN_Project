/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_cell_config.h"
#include "srsgnb/du/du_qos_config.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"
#include "srsgnb/scheduler/config/cell_config_builder_params.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include "srsgnb/support/error_handling.h"
#include <map>

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

  /// As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for Paging.
  cfg.pg.paging_mcs_index    = 5;
  cfg.pg.paging_dci_aggr_lev = aggregation_level::n4;
  cfg.pg.max_paging_retries  = 2;

  cfg.ue.max_nof_harq_retxs = 4;
  cfg.ue.initial_cqi        = 3;
  /// Note: A MCS index of 7 can handle Msg4 of size 458 bytes.
  cfg.ue.max_msg4_mcs              = 7;
  cfg.ue.max_consecutive_pusch_kos = 128;
  cfg.ue.initial_ul_sinr           = 5;

  cfg.metrics_report_period = std::chrono::milliseconds{1000};

  return cfg;
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
  cfg.ul_carrier.arfcn = band_helper::get_ul_arfcn_from_dl_arfcn(cfg.dl_carrier.arfcn);
  cfg.coreset0_idx     = params.coreset0_index;
  cfg.searchspace0_idx = 0U;
  cfg.dl_cfg_common    = make_default_dl_config_common(params);
  cfg.ul_cfg_common    = make_default_ul_config_common(params);
  cfg.scs_common       = params.scs_common;
  cfg.ssb_cfg          = make_default_ssb_config(params);
  cfg.dmrs_typeA_pos   = dmrs_typeA_position::pos2;
  cfg.cell_barred      = false;
  cfg.intra_freq_resel = false;

  if (not band_helper::is_paired_spectrum(cfg.dl_carrier.band)) {
    cfg.tdd_ul_dl_cfg_common.emplace(config_helpers::make_default_tdd_ul_dl_config_common(params));
  }

  cfg.ue_ded_serv_cell_cfg = create_default_initial_ue_serving_cell_config(params);

  return cfg;
}

/// Generates default QoS configuration used by gNB DU. The default configuration should be valid.
inline std::map<uint8_t, du_qos_config> make_default_du_qos_config_list()
{
  std::map<uint8_t, du_qos_config> qos_list = {};
  {
    // 5QI=7
    du_qos_config cfg{};
    cfg.rlc.mode                  = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly    = 50;
    qos_list[7]                   = cfg;
  }
  {
    // 5QI=9 (temporary default to UM)
    du_qos_config cfg{};
    cfg.rlc.mode                  = rlc_mode::um_bidir;
    cfg.rlc.um.tx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.sn_field_length = rlc_um_sn_size::size12bits;
    cfg.rlc.um.rx.t_reassembly    = 50;
    qos_list[9]                   = cfg;
  }
  return qos_list;
}

} // namespace config_helpers
} // namespace srsgnb
