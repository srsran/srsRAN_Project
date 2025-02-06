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

  // Add LCG-ID to UL slice.
  u.ul_logical_channels().set_ran_slice(lcg_id, slice_id);
}

void slice_ue::rem_logical_channel(lcid_t lcid)
{
  dl_logical_channel_manager& dl_lc_mng = u.dl_logical_channels();
  srsran_sanity_check(dl_lc_mng.has_slice(slice_id), "slice_ue should not be created without slice");
  if (dl_lc_mng.get_slice_id(lcid) != slice_id) {
    // LCID is not associated with this slice.
    return;
  }

  // Disconnect RAN slice from LCID.
  dl_lc_mng.reset_ran_slice(lcid);

  // If there are still other bearers in the slice, do not remove the LCG ID.
  lcg_id_t                        lcg_id_to_rem = get_lcg_id(lcid);
  logical_channel_config_list_ptr lc_chs        = u.ue_cfg_dedicated()->logical_channels();
  bool                            to_destroy    = true;
  for (logical_channel_config_ptr lc : *lc_chs) {
    if (lc->lcid != lcid and lc->lc_group == lcg_id_to_rem and dl_lc_mng.get_slice_id(lc->lcid) == slice_id) {
      to_destroy = false;
      break;
    }
  }
  if (to_destroy) {
    u.ul_logical_channels().reset_ran_slice(lcg_id_to_rem);
  }
}

void slice_ue::rem_logical_channels()
{
  u.dl_logical_channels().deactivate(slice_id);
  u.ul_logical_channels().deactivate(slice_id);
}

unsigned slice_ue::pending_ul_newtx_bytes() const
{
  static constexpr unsigned SR_GRANT_BYTES = 512;

  unsigned pending_bytes = u.ul_logical_channels().pending_bytes(slice_id);

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

  // In case a SR is pending and this is the SRB slice, we return a minimum SR grant size if no other bearers have
  // pending data.
  if (slice_id == SRB_RAN_SLICE_ID and has_pending_sr()) {
    pending_bytes = u.ul_logical_channels().pending_bytes();
    pending_bytes -= std::min(pending_bytes, bytes_in_harqs);
    return pending_bytes == 0 ? SR_GRANT_BYTES : 0;
  }

  return 0;
}

bool slice_ue::has_pending_sr() const
{
  return u.has_pending_sr();
}
