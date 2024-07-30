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

void slice_ue::add_logical_channel(lcid_t lcid)
{
  bearers.set(lcid);
}

void slice_ue::rem_logical_channel(lcid_t lcid)
{
  bearers.reset(lcid);
}

bool slice_ue::has_pending_dl_newtx_bytes() const
{
  // TODO: Check for pending bytes only in bearers belonging to this slice.
  return u.has_pending_dl_newtx_bytes();
}

unsigned slice_ue::pending_dl_srb_newtx_bytes() const
{
  return u.pending_dl_srb_newtx_bytes();
}

unsigned slice_ue::pending_dl_newtx_bytes(lcid_t lcid) const
{
  return u.pending_dl_newtx_bytes(lcid);
}

bool slice_ue::has_pending_dl_srb_newtx_bytes() const
{
  return u.has_pending_dl_srb_newtx_bytes();
}

unsigned slice_ue::pending_ul_newtx_bytes() const
{
  // TODO: Check for pending bytes only in bearers belonging to this slice.
  return u.pending_ul_newtx_bytes();
}

bool slice_ue::has_pending_sr() const
{
  return u.has_pending_sr();
}

unsigned slice_ue::pending_ul_srb_newtx_bytes() const
{
  return u.pending_ul_srb_newtx_bytes();
}
