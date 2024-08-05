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

slice_ue::slice_ue(const ue& u_) : u(u_), bearers(MAX_NOF_RB_LCIDS) {}

void slice_ue::add_logical_channel(lcid_t lcid, lcg_id_t lcg_id)
{
  bearers.set(lcid);
  lcid_to_lcg_id_lookup[lcid] = lcg_id;
}

void slice_ue::rem_logical_channel(lcid_t lcid)
{
  bearers.reset(lcid);
  lcid_to_lcg_id_lookup.erase(lcid);
}

bool slice_ue::has_pending_dl_newtx_bytes() const
{
  for (unsigned lcid = 0; lcid < bearers.size(); ++lcid) {
    if (bearers.test(lcid) and u.has_pending_dl_newtx_bytes(uint_to_lcid(lcid))) {
      return true;
    }
  }
  return false;
}

unsigned slice_ue::pending_dl_newtx_bytes() const
{
  unsigned pending_bytes = 0;
  for (unsigned lcid = 0; lcid < bearers.size(); ++lcid) {
    if (bearers.test(lcid)) {
      pending_bytes += u.pending_dl_newtx_bytes(uint_to_lcid(lcid));
    }
  }
  return pending_bytes;
}

unsigned slice_ue::pending_ul_newtx_bytes() const
{
  constexpr static unsigned SR_GRANT_BYTES = 512;

  bounded_bitset<MAX_NOF_LCGS> lcg_ids(MAX_NOF_LCGS);
  unsigned                     pending_bytes = 0;
  for (unsigned lcid = 0; lcid < bearers.size(); ++lcid) {
    if (bearers.test(lcid)) {
      lcg_ids.set(lcid_to_lcg_id_lookup.at(uint_to_lcid(lcid)));
    }
  }
  for (unsigned lcg_id = 0; lcg_id < lcg_ids.size(); ++lcg_id) {
    if (lcg_ids.test(lcg_id)) {
      pending_bytes += u.pending_ul_newtx_bytes(uint_to_lcg_id(lcg_id));
    }
  }
  // Subtract the bytes already allocated in UL HARQs.
  for (unsigned cell_idx = 0; cell_idx < nof_cells(); ++cell_idx) {
    const ue_cell& ue_cc = get_cell(to_ue_cell_index(cell_idx));
    if (pending_bytes == 0) {
      break;
    }
    unsigned harq_bytes = 0;
    for (unsigned i = 0; i != ue_cc.harqs.nof_ul_harqs(); ++i) {
      const ul_harq_process& h_ul = ue_cc.harqs.ul_harq(i);
      if (not h_ul.empty()) {
        harq_bytes += h_ul.last_tx_params().tbs_bytes;
      }
    }
    harq_bytes += ue_cc.harqs.ntn_get_tbs_pending_crcs();
    pending_bytes -= std::min(pending_bytes, harq_bytes);
  }

  // If there are no pending bytes, check if a SR is pending.
  return pending_bytes > 0 ? pending_bytes : (has_pending_sr() ? SR_GRANT_BYTES : 0);
}

bool slice_ue::has_pending_sr() const
{
  return u.has_pending_sr();
}
