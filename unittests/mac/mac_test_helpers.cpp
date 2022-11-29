/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_test_helpers.h"

using namespace srsgnb;
using namespace srsgnb::test_helpers;

void mac_test_ue::add_bearer(lcid_t lcid)
{
  bearers.emplace(lcid);
  bearers[lcid].bearer.lcid      = lcid;
  bearers[lcid].bearer.ul_bearer = &bearers[lcid].ul_notifier;
  bearers[lcid].bearer.dl_bearer = &bearers[lcid].dl_notifier;
}

mac_ue_create_request_message mac_test_ue::make_ue_create_request()
{
  mac_ue_create_request_message msg;
  msg.cell_index = to_du_cell_index(0);
  msg.ue_index   = ue_index;
  msg.crnti      = rnti;
  for (const auto& b : bearers) {
    msg.bearers.push_back(b.bearer);
  }
  msg.ul_ccch_msg        = nullptr;
  msg.ue_activity_timer  = &activity_timer;
  msg.mac_cell_group_cfg = {};
  msg.phy_cell_group_cfg = {};
  msg.serv_cell_cfg      = {};
  return msg;
}
