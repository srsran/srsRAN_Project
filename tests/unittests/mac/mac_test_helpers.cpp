/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "mac_test_helpers.h"

using namespace srsran;
using namespace srsran::test_helpers;

void mac_test_ue::add_bearer(lcid_t lcid)
{
  bearers.emplace(lcid);
  bearers[lcid].bearer.lcid      = lcid;
  bearers[lcid].bearer.ul_bearer = &bearers[lcid].ul_notifier;
  bearers[lcid].bearer.dl_bearer = &bearers[lcid].dl_notifier;
}

mac_ue_create_request mac_test_ue::make_ue_create_request()
{
  mac_ue_create_request msg = make_default_ue_creation_request();
  msg.ue_index              = ue_index;
  msg.crnti                 = rnti;
  for (const auto& b : bearers) {
    msg.bearers.push_back(b.bearer);
  }
  msg.ul_ccch_msg = nullptr;
  return msg;
}
