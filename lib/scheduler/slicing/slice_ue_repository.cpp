/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slice_ue_repository.h"

using namespace srsran;

slice_ue::slice_ue(const ue& u_) : u(u_) {}

void slice_ue::add_logical_channel(lcid_t lcid, lcg_id_t lcg_id)
{
  srsran_assert(lcid < MAX_NOF_RB_LCIDS, "Invalid LCID={} to add for a slice UE", fmt::underlying(lcid));
  srsran_assert(lcg_id < MAX_NOF_LCGS,
                "Invalid LCG ID={} for bearer with LCID={}",
                fmt::underlying(lcg_id),
                fmt::underlying(lcid));

  if (lcid >= bearers.size()) {
    bearers.resize(lcid + 1);
  }
  bearers.set(lcid);
  if (lcg_id >= lcg_ids.size()) {
    lcg_ids.resize(lcg_id + 1);
  }
  lcg_ids.set(lcg_id);
}

void slice_ue::rem_logical_channel(lcid_t lcid)
{
  srsran_assert(lcid < MAX_NOF_RB_LCIDS, "Invalid LCID={} to remove from a slice UE", fmt::underlying(lcid));
  bearers.reset(lcid);
  lcg_id_t lcg_id_to_rem = get_lcg_id_for_bearer(lcid);
  srsran_assert(lcg_id_to_rem < MAX_NOF_LCGS, "Unable to fetch LCG ID for bearer with LCID={}", fmt::underlying(lcid));
  // Check whether there are bearers with same LCG ID. If not, remove LCG ID from slice.
  for (unsigned lcid_idx = 0, e = bearers.size(); lcid_idx != e; ++lcid_idx) {
    if (bearers.test(uint_to_lcid(lcid_idx))) {
      lcg_id_t other_lcg_id = get_lcg_id_for_bearer(uint_to_lcid(lcid_idx));
      if (lcg_id_to_rem == other_lcg_id) {
        return;
      }
    }
  }
  lcg_ids.reset(lcg_id_to_rem);
}

bool slice_ue::has_pending_dl_newtx_bytes() const
{
  // If at least one slice bearer has pending data, return true.
  for (unsigned lcid = 0, e = bearers.size(); lcid != e; ++lcid) {
    if (bearers.test(lcid) and u.has_pending_dl_newtx_bytes(uint_to_lcid(lcid))) {
      return true;
    }
  }

  // In case of SRB slice and there are pending CE bytes and no pending newtx data
  // Note: We only report pending newtx bytes when there are pending CEs in the case of SRB slice and if there are no
  // pending data in other slices. Otherwise, we let other slices allocate the CEs.
  if (bearers.size() > 1 and bearers.test(LCID_SRB1) and u.has_pending_ce_bytes()) {
    // Check if there aren't pending bytes in other slices.
    return not u.has_pending_dl_newtx_bytes();
  }

  return false;
}

unsigned slice_ue::pending_dl_newtx_bytes() const
{
  // Compute pending bytes in LC CHs.
  unsigned pending_bytes = 0;
  for (unsigned lcid = 0, e = bearers.size(); lcid != e; ++lcid) {
    if (bearers.test(lcid)) {
      pending_bytes += u.pending_dl_newtx_bytes(uint_to_lcid(lcid));
    }
  }

  unsigned pending_ce_bytes = u.pending_ce_bytes();
  if (pending_ce_bytes > 0) {
    // In case the slice UE has pending bytes, we also include the CE bytes.
    if (pending_bytes > 0) {
      pending_bytes += pending_ce_bytes;
    } else if (bearers.size() > 1 and bearers.test(LCID_SRB1)) {
      // In case of SRB slice, we also include the CE bytes if the UE has no pending data in other slices.
      if (not u.has_pending_dl_newtx_bytes()) {
        pending_bytes += pending_ce_bytes;
      }
    }
  }

  return pending_bytes;
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
  const ue_configuration*            ue_ded_cfg = u.ue_cfg_dedicated();
  span<const logical_channel_config> lc_cfgs    = ue_ded_cfg->logical_channels();
  for (const auto& cfg : lc_cfgs) {
    if (cfg.lcid == lcid) {
      return cfg.lc_group;
    }
  }
  return LCG_ID_INVALID;
}
