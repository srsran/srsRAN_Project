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

using namespace srsgnb;
using namespace srs_du;

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
    req.drbs_to_setup.back().drb_id         = drb_id;
    req.drbs_to_setup.back().rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_am;
  }

  return req;
}
