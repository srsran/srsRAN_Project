/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dl_logical_channel_manager.h"

using namespace srsgnb;

namespace {

constexpr unsigned FIXED_SIZED_MAC_CE_SUBHEADER_SIZE = 1;
constexpr unsigned MIN_MAC_SUBHEADER_SIZE            = 2;
constexpr unsigned MAX_MAC_SUBHEADER_SIZE            = 3;

/// TS 38.321 sec 6.1 - MAC SDU subheader is 2 bytes if L<=128 and 3 otherwise.
unsigned get_mac_subheader_size(unsigned sdu_bytes)
{
  return sdu_bytes == 0 ? 0 : (sdu_bytes > 128 ? MAX_MAC_SUBHEADER_SIZE : MIN_MAC_SUBHEADER_SIZE);
}
unsigned get_mac_sdu_size(unsigned sdu_and_subheader_bytes)
{
  if (sdu_and_subheader_bytes == 0) {
    return 0;
  }
  unsigned sdu_size = sdu_and_subheader_bytes - MIN_MAC_SUBHEADER_SIZE;
  return sdu_size <= 128 ? sdu_size : sdu_size - 1;
}
unsigned get_mac_sdu_and_subheader_size(unsigned sdu_bytes)
{
  return sdu_bytes + get_mac_subheader_size(sdu_bytes);
}
unsigned get_mac_subpdu_and_subheader_size(lcid_dl_sch_t lcid, unsigned sdu_bytes)
{
  if (lcid.is_ce() and not lcid.is_var_len_ce()) {
    srsgnb_sanity_check(sdu_bytes == lcid.sizeof_ce(), "Invalid MAC CE size");
    return FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + sdu_bytes;
  }
  return get_mac_sdu_and_subheader_size(sdu_bytes);
}

} // namespace

unsigned dl_logical_channel_manager::allocate_mac_sdu(dl_msg_lc_info& lch_info, unsigned rem_bytes)
{
  lcid_t lcid = get_max_prio_lcid();
  if (lcid == lcid_t::INVALID_LCID) {
    return 0;
  }

  // Update Buffer Status of allocated LCID.
  unsigned alloc_bytes = 0;
  alloc_bytes          = allocate_mac_sdu(lcid, rem_bytes);

  if (alloc_bytes > 0) {
    lch_info.lcid        = (lcid_dl_sch_t::options)lcid;
    lch_info.sched_bytes = get_mac_sdu_size(alloc_bytes);
  }

  return alloc_bytes;
}

lcid_t dl_logical_channel_manager::get_max_prio_lcid() const
{
  // Prioritize by LCID.
  for (unsigned idx = 0; idx != channels.size(); ++idx) {
    if (channels[idx].active and channels[idx].buf_st > 0) {
      return (lcid_t)idx;
    }
  }
  return INVALID_LCID;
}

unsigned dl_logical_channel_manager::allocate_mac_sdu(lcid_t lcid, unsigned rem_bytes)
{
  unsigned lch_bytes = pending_bytes(lcid);
  if (lch_bytes == 0) {
    return 0;
  }

  // Account for available space and MAC subheader to decide the number of bytes to allocate.
  unsigned alloc_bytes = std::min(rem_bytes, get_mac_sdu_and_subheader_size(lch_bytes));

  // If it is last PDU of the TBS, allocate all leftover bytes.
  unsigned leftover_bytes = rem_bytes - alloc_bytes;
  if (leftover_bytes > 0 and (leftover_bytes <= MAX_MAC_SUBHEADER_SIZE or pending_bytes() == 0)) {
    alloc_bytes += leftover_bytes;
  }

  // Update DL Buffer Status to avoid reallocating the same LCID bytes.
  channels[lcid].buf_st -= get_mac_sdu_size(alloc_bytes);

  return alloc_bytes;
}

unsigned dl_logical_channel_manager::allocate_mac_ce(dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  if (pending_ces.empty()) {
    return 0;
  }

  lcid_dl_sch_t lcid        = pending_ces.front();
  unsigned      ce_size     = lcid.sizeof_ce();
  unsigned      alloc_bytes = get_mac_subpdu_and_subheader_size(lcid, ce_size);

  // Verify there is space for both MAC CE and subheader.
  if (rem_bytes < alloc_bytes) {
    alloc_bytes = 0;
  }

  if (alloc_bytes > 0) {
    pending_ces.pop_front();

    subpdu.lcid        = lcid;
    subpdu.sched_bytes = ce_size;
  }

  return alloc_bytes;
}

unsigned srsgnb::allocate_mac_sdus(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs)
{
  unsigned rem_tbs = total_tbs;

  // if we do not have enough bytes to fit MAC subheader, skip MAC SDU allocation.
  // Note: We assume upper layer accounts for its own subheaders when updating the buffer state.
  while (rem_tbs > MAX_MAC_SUBHEADER_SIZE and not tb_info.subpdus.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_mac_sdu(subpdu, rem_tbs);
    if (alloc_bytes == 0) {
      break;
    }

    // add new subPDU.
    tb_info.subpdus.push_back(subpdu);

    // update remaining space taking into account the MAC SDU subheader.
    rem_tbs -= alloc_bytes;
  }

  return total_tbs - rem_tbs;
}

unsigned srsgnb::allocate_mac_ces(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs)
{
  unsigned rem_tbs = total_tbs;

  while (lch_mng.has_pending_ces() and not tb_info.subpdus.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_mac_ce(subpdu, rem_tbs);
    if (alloc_bytes == 0) {
      break;
    }

    // add new subPDU.
    tb_info.subpdus.push_back(subpdu);

    // update remaining space taking into account the MAC CE subheader.
    rem_tbs -= alloc_bytes;
  }
  return total_tbs - rem_tbs;
}
