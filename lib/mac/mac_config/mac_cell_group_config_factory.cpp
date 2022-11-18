/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/mac/bsr_format.h"
#include "srsgnb/mac/config/mac_cell_group_config_factory.h"
#include "srsgnb/mac/mac_cell_group_config.h"
#include "srsgnb/mac/phr_config.h"
#include "srsgnb/mac/time_alignment_group_config.h"
#include "srsgnb/ran/sr_configuration.h"

using namespace srsgnb;

mac_cell_group_config config_helpers::make_initial_mac_cell_group_config()
{
  mac_cell_group_config        mcg_cfg{};
  scheduling_request_to_addmod sr_0{.sr_id = scheduling_request_id::SR_ID_MIN, .max_tx = sr_max_tx::n64};
  mcg_cfg.scheduling_request_config.emplace_back(sr_0);

  mcg_cfg.bsr_cfg.emplace();
  mcg_cfg.bsr_cfg.value().periodic_timer = periodic_bsr_timer::sf10;
  mcg_cfg.bsr_cfg.value().retx_timer     = retx_bsr_timer::sf80;

  mcg_cfg.tag_config.emplace_back(tag{.tag_id = uint_to_tag_id(0), .ta_timer = time_alignment_timer::infinity});

  mcg_cfg.phr_cfg.emplace();
  mcg_cfg.phr_cfg.value().periodic_timer         = phr_periodic_timer::sf10;
  mcg_cfg.phr_cfg.value().prohibit_timer         = phr_prohibit_timer::sf10;
  mcg_cfg.phr_cfg.value().power_factor_change    = phr_tx_power_factor_change::db1;
  mcg_cfg.phr_cfg.value().multiple_phr           = false;
  mcg_cfg.phr_cfg.value().dummy                  = false;
  mcg_cfg.phr_cfg.value().phr_type_to_other_cell = false;
  mcg_cfg.phr_cfg.value().phr_mode               = phr_mode_other_cg::real;

  mcg_cfg.skip_uplink_tx_dynamic = false;

  return mcg_cfg;
}
