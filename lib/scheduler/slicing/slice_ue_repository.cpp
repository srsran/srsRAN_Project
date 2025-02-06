/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slice_ue_repository.h"

using namespace srsran;

slice_ue::slice_ue(ue& u_, ran_slice_id_t slice_id_) : u(u_), slice_id(slice_id_) {}

void slice_ue::add_logical_channel(lcid_t lcid, lcg_id_t lcg_id)
{
  // Add LCID to DL slice.
  u.dl_logical_channels().set_ran_slice(lcid, slice_id);

  if (lcg_id >= lcg_ids.size()) {
    lcg_ids.resize(lcg_id + 1);
  }
  lcg_ids.set(lcg_id);
}

void slice_ue::rem_logical_channel(lcid_t lcid)
{
  lcg_id_t lcg_id_to_rem = get_lcg_id_for_bearer(lcid);
  srsran_assert(lcg_id_to_rem < MAX_NOF_LCGS, "Unable to fetch LCG ID for bearer with LCID={}", fmt::underlying(lcid));
  // Check whether there are bearers with same LCG ID. If not, remove LCG ID from slice.
  logical_channel_config_list_ptr lc_chs = u.ue_cfg_dedicated()->logical_channels();
  if (std::none_of(
          lc_chs->begin(), lc_chs->end(), [lcg_id_to_rem](const auto& lc) { return lc->lc_group == lcg_id_to_rem; })) {
    lcg_ids.reset(lcg_id_to_rem);
  }
  u.dl_logical_channels().reset_ran_slice(lcid);
}

bool slice_ue::has_pending_dl_newtx_bytes() const
{
  return u.dl_logical_channels().has_pending_bytes(slice_id);
}

unsigned slice_ue::pending_dl_newtx_bytes() const
{
  return u.dl_logical_channels().pending_bytes(slice_id);
}

unsigned slice_ue::pending_ul_newtx_bytes() const
{
  static constexpr unsigned SR_GRANT_BYTES = 512;

  unsigned pending_bytes = 0;
  for (unsigned lcg_id = 0, e = lcg_ids.size(); lcg_id != e; ++lcg_id) {
    if (lcg_ids.test(lcg_id)) {
      pending_bytes += u.pending_ul_newtx_bytes(uint_to_lcg_id(lcg_id));
    }
  }
  // Subtract the bytes already allocated in UL HARQs.
  unsigned bytes_in_harqs = 0;
  for (unsigned cell_idx = 0, e = nof_cells(); cell_idx != e; ++cell_idx) {
    const ue_cell& ue_cc = get_cell(to_ue_cell_index(cell_idx));
    if (pending_bytes <= bytes_in_harqs) {
      break;
    }
    bytes_in_harqs += ue_cc.harqs.total_ul_bytes_waiting_ack();
  }
  pending_bytes -= std::min(pending_bytes, bytes_in_harqs);
  if (pending_bytes > 0) {
    return pending_bytes;
  }

  // If there are no pending bytes, check if a SR is pending.
  // Note: We consider all LCGs, so that the UL grant is not unnecessarily small, when there are bytes already pending
  // for other slices of the UE.
  if (has_pending_sr()) {
    for (unsigned lcg_id = 0, e = lcg_ids.size(); lcg_id != e; ++lcg_id) {
      pending_bytes += u.pending_ul_newtx_bytes(uint_to_lcg_id(lcg_id));
    }
    pending_bytes -= std::min(pending_bytes, bytes_in_harqs);
    return std::max(pending_bytes, SR_GRANT_BYTES);
  }

  return 0;
}

bool slice_ue::has_pending_sr() const
{
  return u.has_pending_sr();
}

lcg_id_t slice_ue::get_lcg_id_for_bearer(lcid_t lcid) const
{
  logical_channel_config_list_ptr lc_chs = u.ue_cfg_dedicated()->logical_channels();
  return lc_chs->contains(lcid) ? lc_chs.value()[lcid]->lc_group : LCG_ID_INVALID;
}
