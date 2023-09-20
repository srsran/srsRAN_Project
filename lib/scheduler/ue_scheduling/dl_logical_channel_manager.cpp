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

#include "dl_logical_channel_manager.h"

using namespace srsran;

static unsigned get_mac_sdu_size(unsigned sdu_and_subheader_bytes)
{
  if (sdu_and_subheader_bytes == 0) {
    return 0;
  }
  const unsigned sdu_size = sdu_and_subheader_bytes - MIN_MAC_SDU_SUBHEADER_SIZE;
  return sdu_size < MAC_SDU_SUBHEADER_LENGTH_THRES ? sdu_size : sdu_size - 1;
}

dl_logical_channel_manager::dl_logical_channel_manager()
{
  // SRB0 is always activated.
  set_status(LCID_SRB0, true);
}

void dl_logical_channel_manager::configure(span<const logical_channel_config> log_channels_configs)
{
  for (unsigned i = 1; i != channels.size(); ++i) {
    channels[i].active = false;
  }
  for (const logical_channel_config& lc_ch : log_channels_configs) {
    set_status(lc_ch.lcid, true);
  }
}

unsigned dl_logical_channel_manager::allocate_mac_sdu(dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  subpdu.lcid        = lcid_dl_sch_t::MIN_RESERVED;
  subpdu.sched_bytes = 0;

  lcid_t lcid = get_max_prio_lcid();
  if (lcid == lcid_t::INVALID_LCID) {
    return 0;
  }

  // Update Buffer Status of allocated LCID.
  return allocate_mac_sdu(subpdu, lcid, rem_bytes);
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

unsigned dl_logical_channel_manager::allocate_mac_sdu(dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes)
{
  unsigned lch_bytes = pending_bytes(lcid);
  if (lch_bytes == 0 or rem_bytes <= MIN_MAC_SDU_SUBHEADER_SIZE) {
    return 0;
  }

  // Account for available space and MAC subheader to decide the number of bytes to allocate.
  unsigned alloc_bytes = std::min(rem_bytes, lch_bytes);

  // If it is last PDU of the TBS, allocate all leftover bytes.
  unsigned leftover_bytes = rem_bytes - alloc_bytes;
  if (leftover_bytes > 0 and (leftover_bytes <= MAX_MAC_SDU_SUBHEADER_SIZE or pending_bytes() == 0)) {
    alloc_bytes += leftover_bytes;
  }
  if (alloc_bytes == MAC_SDU_SUBHEADER_LENGTH_THRES + MIN_MAC_SDU_SUBHEADER_SIZE) {
    // avoid invalid combination of MAC subPDU and subheader size.
    alloc_bytes--;
  }
  unsigned sdu_size = get_mac_sdu_size(alloc_bytes);

  // Update DL Buffer Status to avoid reallocating the same LCID bytes.
  channels[lcid].buf_st -= std::min(sdu_size, channels[lcid].buf_st);

  subpdu.lcid        = (lcid_dl_sch_t::options)lcid;
  subpdu.sched_bytes = sdu_size;

  return alloc_bytes;
}

unsigned dl_logical_channel_manager::allocate_mac_ce(dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  subpdu.lcid        = lcid_dl_sch_t::MIN_RESERVED;
  subpdu.sched_bytes = 0;

  // Allocate ConRes CE ID if pending.
  unsigned alloc_bytes = allocate_ue_con_res_id_mac_ce(subpdu, rem_bytes);
  if (alloc_bytes > 0) {
    return alloc_bytes;
  }

  if (pending_ces.empty()) {
    return 0;
  }
  const lcid_dl_sch_t lcid = pending_ces.front().ce_lcid;

  // Derive space needed for CE subheader + payload.
  const unsigned ce_size = lcid.sizeof_ce();
  if (lcid.is_var_len_ce()) {
    alloc_bytes = get_mac_sdu_required_bytes(ce_size);
  } else {
    alloc_bytes = ce_size + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  }

  // Verify there is space for both MAC CE and subheader.
  if (rem_bytes < alloc_bytes) {
    return 0;
  }

  subpdu.lcid        = lcid;
  subpdu.sched_bytes = ce_size;
  subpdu.ce_payload  = pending_ces.front().ce_payload;

  pending_ces.pop_front();

  return alloc_bytes;
}

unsigned srsran::allocate_mac_sdus(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs)
{
  unsigned rem_tbs = total_tbs;

  // if we do not have enough bytes to fit MAC subheader, skip MAC SDU allocation.
  // Note: We assume upper layer accounts for its own subheaders when updating the buffer state.
  while (rem_tbs > MAX_MAC_SDU_SUBHEADER_SIZE and not tb_info.lc_chs_to_sched.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_mac_sdu(subpdu, rem_tbs);
    if (alloc_bytes == 0) {
      break;
    }

    // add new subPDU.
    tb_info.lc_chs_to_sched.push_back(subpdu);

    // update remaining space taking into account the MAC SDU subheader.
    rem_tbs -= alloc_bytes;
  }

  return total_tbs - rem_tbs;
}

unsigned srsran::allocate_mac_ces(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs)
{
  unsigned rem_tbs = total_tbs;

  while ((lch_mng.is_con_res_id_pending() or lch_mng.has_pending_ces()) and not tb_info.lc_chs_to_sched.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_mac_ce(subpdu, rem_tbs);
    if (alloc_bytes == 0) {
      break;
    }

    // add new subPDU.
    tb_info.lc_chs_to_sched.push_back(subpdu);

    // update remaining space taking into account the MAC CE subheader.
    rem_tbs -= alloc_bytes;
  }
  return total_tbs - rem_tbs;
}

unsigned dl_logical_channel_manager::allocate_ue_con_res_id_mac_ce(dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  if (not pending_con_res_id) {
    return 0;
  }

  subpdu.lcid        = lcid_dl_sch_t::MIN_RESERVED;
  subpdu.sched_bytes = 0;

  static const auto ce_size     = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce();
  unsigned          alloc_bytes = ce_size + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;

  // Verify there is space for both MAC CE and subheader.
  if (rem_bytes < alloc_bytes) {
    alloc_bytes = 0;
  }

  if (alloc_bytes > 0) {
    pending_con_res_id = false;

    subpdu.lcid        = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID};
    subpdu.sched_bytes = ce_size;
  }

  return alloc_bytes;
}

unsigned
srsran::allocate_ue_con_res_id_mac_ce(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs)
{
  unsigned rem_tbs = total_tbs;

  if (not tb_info.lc_chs_to_sched.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_ue_con_res_id_mac_ce(subpdu, rem_tbs);
    if (alloc_bytes != 0) {
      // add new subPDU.
      tb_info.lc_chs_to_sched.push_back(subpdu);

      // update remaining space taking into account the MAC CE subheader.
      rem_tbs -= alloc_bytes;
    }
  }
  return total_tbs - rem_tbs;
}
