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

#include "mac_ul_ue_manager.h"

using namespace srsran;

mac_ul_ue_manager::mac_ul_ue_manager(du_rnti_table& rnti_table_) :
  logger(srslog::fetch_basic_logger("MAC")), rnti_table(rnti_table_)
{
}

bool mac_ul_ue_manager::add_ue(const mac_ue_create_request& request)
{
  srsran_assert(is_crnti(request.crnti), "Invalid C-RNTI={:#x}", request.crnti);
  srsran_assert(is_du_ue_index_valid(request.ue_index), "Invalid UE index={}", request.ue_index);

  // > Insert UE
  if (ue_db.contains(request.ue_index)) {
    return false;
  }
  ue_db.emplace(request.ue_index, request.ue_index, request.crnti);

  // > Add UE Bearers
  if (not addmod_bearers(request.ue_index, request.bearers)) {
    log_proc_failure(logger, request.ue_index, request.crnti, "UE Create Request", "Failed to add/mod UE bearers");
    return false;
  }

  return true;
}

void mac_ul_ue_manager::remove_ue(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    log_proc_failure(logger, ue_index, "UE Remove Request", "Invalid RNTI");
    return;
  }
  ue_db.erase(ue_index);
}

bool mac_ul_ue_manager::addmod_bearers(du_ue_index_t                                  ue_index,
                                       const std::vector<mac_logical_channel_config>& ul_logical_channels)
{
  if (not ue_db.contains(ue_index)) {
    logger.error("ue={} Interrupting DEMUX update. Cause: The provided index does not exist", ue_index);
    return false;
  }
  mac_ul_ue_context& u = ue_db[ue_index];

  for (const mac_logical_channel_config& channel : ul_logical_channels) {
    u.ul_bearers.insert(channel.lcid, channel.ul_bearer);
  }

  return true;
}

bool mac_ul_ue_manager::remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids)
{
  if (not ue_db.contains(ue_index)) {
    logger.error("ue={} Interrupting DEMUX update. Cause: The provided index does not exist", ue_index);
    return false;
  }
  mac_ul_ue_context& u = ue_db[ue_index];

  for (lcid_t lcid : lcids) {
    u.ul_bearers.erase(lcid);
  }
  return true;
}
