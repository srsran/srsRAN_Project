/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_manager_test_helpers.h"
#include "srsgnb/mac/config/mac_cell_group_config_factory.h"

using namespace srsgnb;
using namespace srs_du;

dummy_cell_resource_allocator::dummy_cell_resource_allocator()
{
  ue_resource_pool.reserve(MAX_NOF_DU_UES);
  next_context_update_result.rlc_bearers.resize(1);
  next_context_update_result.rlc_bearers[0].lcid       = LCID_SRB1;
  next_context_update_result.rlc_bearers[0].rlc_cfg    = make_default_srb_rlc_config();
  next_context_update_result.spcell_cfg.serv_cell_idx  = SERVING_CELL_PCELL_IDX;
  next_context_update_result.spcell_cfg.cell_index     = to_du_cell_index(0);
  next_context_update_result.spcell_cfg.spcell_cfg_ded = config_helpers::make_default_initial_ue_serving_cell_config();
  next_context_update_result.mcg_cfg                   = config_helpers::make_initial_mac_cell_group_config();
  next_context_update_result.pcg_cfg                   = {}; // TODO
}

const cell_group_config* dummy_cell_resource_allocator::update_context(du_ue_index_t   ue_index,
                                                                       du_cell_index_t pcell_index,
                                                                       const f1ap_ue_context_update_request& upd_req)
{
  last_ue_index   = ue_index;
  last_ue_pcell   = pcell_index;
  last_ue_ctx_upd = upd_req;
  ue_resource_pool.emplace(ue_index, next_context_update_result);
  return &ue_resource_pool[ue_index];
}

f1ap_ue_context_update_request
srsgnb::srs_du::create_f1ap_ue_context_update_request(du_ue_index_t                   ue_idx,
                                                      std::initializer_list<srb_id_t> srbs_to_addmod,
                                                      std::initializer_list<drb_id_t> drbs_to_addmod)
{
  f1ap_ue_context_update_request req;

  req.ue_index = ue_idx;

  for (srb_id_t srb_id : srbs_to_addmod) {
    req.srbs_to_setup.emplace_back();
    req.srbs_to_setup.back() = srb_id;
  }

  for (drb_id_t drb_id : drbs_to_addmod) {
    req.drbs_to_setup.emplace_back();
    req.drbs_to_setup.back().drb_id = drb_id;
    req.drbs_to_setup.back().mode   = drb_rlc_mode::am;
  }

  return req;
}
