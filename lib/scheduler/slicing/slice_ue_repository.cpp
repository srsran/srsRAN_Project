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

#include "slice_ue_repository.h"

using namespace srsran;

slice_ue::slice_ue(ue& u_, ue_cell& ue_cc_, ran_slice_id_t slice_id_) : u(u_), ue_cc(ue_cc_), slice_id(slice_id_)
{
  u.dl_logical_channels().register_ran_slice(slice_id);
}

// class slice_ue_repository

slice_ue_repository::slice_ue_repository(ran_slice_id_t slice_id_, du_cell_index_t cell_index_) :
  slice_id(slice_id_), cell_index(cell_index_)
{
}

bool slice_ue_repository::add_ue(ue& u)
{
  if (not ue_map.contains(u.ue_index)) {
    u.dl_logical_channels().register_ran_slice(slice_id);
    ue_cell* ue_cc = u.find_cell(cell_index);
    srsran_sanity_check(ue_cc != nullptr, "Invalid UE added to RAN slice");
    ue_map.emplace(u.ue_index, u, *ue_cc, slice_id);
    return true;
  }
  return false;
}

void slice_ue_repository::add_logical_channel(ue& u, lcid_t lcid, lcg_id_t lcg_id)
{
  if (lcid == LCID_SRB0) {
    // SRB0 is never handled by slice scheduler.
    return;
  }

  // Create UE entry if not created yet.
  add_ue(u);

  // Add LCID to DL slice.
  u.dl_logical_channels().set_lcid_ran_slice(lcid, slice_id);

  // Add LCG-ID to UL slice.
  u.ul_logical_channels().set_lcg_ran_slice(lcg_id, slice_id);
}

void slice_ue_repository::rem_logical_channel(du_ue_index_t ue_idx, lcid_t lcid)
{
  if (not ue_map.contains(ue_idx)) {
    return;
  }
  auto& slice_u = ue_map[ue_idx];

  dl_logical_channel_manager& dl_lc_mng = slice_u.u.dl_logical_channels();
  srsran_sanity_check(dl_lc_mng.has_slice(slice_id), "slice_ue should not be created without slice");
  if (dl_lc_mng.get_slice_id(lcid) != slice_id) {
    // LCID is not associated with this slice.
    return;
  }

  // Disconnect RAN slice from LCID.
  dl_lc_mng.reset_lcid_ran_slice(lcid);

  if (not slice_u.u.dl_logical_channels().has_slice(slice_id)) {
    // If no more bearers active for this UE, remove it from the slice.
    rem_ue(ue_idx);
  }

  // If there are still other bearers in the slice, do not remove the LCG ID.
  logical_channel_config_list_ptr lc_chs = slice_u.u.ue_cfg_dedicated()->logical_channels();
  lcg_id_t lcg_id_to_rem = lc_chs.value().contains(lcid) ? lc_chs.value()[lcid]->lc_group : MAX_NOF_LCGS;
  bool     to_destroy    = true;
  for (logical_channel_config_ptr lc : *lc_chs) {
    if (lc->lcid != lcid and lc->lc_group == lcg_id_to_rem and dl_lc_mng.get_slice_id(lc->lcid) == slice_id) {
      to_destroy = false;
      break;
    }
  }
  if (to_destroy) {
    slice_u.u.ul_logical_channels().reset_lcg_ran_slice(lcg_id_to_rem);
  }
}

void slice_ue_repository::rem_ue(du_ue_index_t ue_index)
{
  if (not ue_map.contains(ue_index)) {
    return;
  }
  auto& slice_u = ue_map[ue_index];
  slice_u.u.dl_logical_channels().deregister_ran_slice(slice_id);
  slice_u.u.ul_logical_channels().deregister_ran_slice(slice_id);
  ue_map.erase(ue_index);
}

unsigned slice_ue::pending_ul_newtx_bytes() const
{
  static constexpr unsigned SR_GRANT_BYTES = 512;

  unsigned pending_bytes = u.ul_logical_channels().pending_bytes(slice_id);

  // Subtract the bytes already allocated in UL HARQs.
  unsigned bytes_in_harqs = 0;
  for (unsigned cell_idx = 0, e = u.nof_cells(); cell_idx != e; ++cell_idx) {
    const ue_cell& cc = u.get_cell(to_ue_cell_index(cell_idx));
    if (pending_bytes <= bytes_in_harqs) {
      break;
    }
    bytes_in_harqs += cc.harqs.total_ul_bytes_waiting_ack();
  }
  pending_bytes -= std::min(pending_bytes, bytes_in_harqs);
  if (pending_bytes > 0) {
    return pending_bytes;
  }

  // In case a SR is pending and this is the SRB slice, we return a minimum SR grant size if no other bearers have
  // pending data.
  if (slice_id == SRB_RAN_SLICE_ID and has_pending_sr()) {
    pending_bytes = u.ul_logical_channels().pending_bytes();
    pending_bytes -= std::min(pending_bytes, bytes_in_harqs);
    return pending_bytes == 0 ? SR_GRANT_BYTES : 0;
  }

  return 0;
}
