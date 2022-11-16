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
#include "srsgnb/scheduler/config/du_cell_config_master_params.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include "srsgnb/support/error_handling.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {
namespace config_helpers {

inline scheduler_expert_config make_default_scheduler_config()
{
  scheduler_expert_config cfg;
  cfg.ra.max_nof_msg3_harq_retxs = 4;
  cfg.ra.msg3_mcs_index          = 0;
  cfg.ra.rar_mcs_index           = 0;

  cfg.ue.max_nof_harq_retxs = 4;
  cfg.ue.fixed_dl_mcs       = 10;
  cfg.ue.fixed_ul_mcs       = 10;

  return cfg;
}

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline du_cell_config make_default_du_cell_config(const du_cell_config_master_params& params = {})
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
  return cfg;
}

} // namespace config_helpers
} // namespace srsgnb
