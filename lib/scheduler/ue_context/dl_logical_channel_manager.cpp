/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  sorted_channels.resize(1, LCID_SRB0);
}

void dl_logical_channel_manager::deactivate()
{
  for (unsigned lcid = LCID_SRB0; lcid <= LCID_MAX_DRB; lcid++) {
    set_status((lcid_t)lcid, false);
  }
  pending_ces.clear();
  sorted_channels.clear();
}

void dl_logical_channel_manager::configure(span<const logical_channel_config> log_channels_configs)
{
  // Clear old list of sorted channels.
  sorted_channels.clear();
  sorted_channels.resize(log_channels_configs.size());

  for (unsigned i = 1; i != channels.size(); ++i) {
    channels[i].active = false;
  }
  for (const logical_channel_config& lc_ch : log_channels_configs) {
    set_status(lc_ch.lcid, true);
    sorted_channels.push_back(lc_ch.lcid);
  }

  // Sort logical channels based on priority set in MAC LC configuration.
  std::sort(
      sorted_channels.begin(), sorted_channels.end(), [log_channels_configs](const auto lcid_lhs, const auto lcid_rhs) {
        auto it_lhs = std::find_if(log_channels_configs.begin(),
                                   log_channels_configs.end(),
                                   [lcid_lhs](const logical_channel_config& lc_ch) { return lc_ch.lcid == lcid_lhs; });
        auto it_rhs = std::find_if(log_channels_configs.begin(),
                                   log_channels_configs.end(),
                                   [lcid_rhs](const logical_channel_config& lc_ch) { return lc_ch.lcid == lcid_rhs; });
        return it_lhs->priority < it_rhs->priority;
      });
}

unsigned dl_logical_channel_manager::allocate_mac_sdu(dl_msg_lc_info& subpdu, unsigned rem_bytes, lcid_t lcid)
{
  subpdu.lcid        = lcid_dl_sch_t::MIN_RESERVED;
  subpdu.sched_bytes = 0;

  lcid_t lcid_with_prio = lcid == INVALID_LCID ? get_max_prio_lcid() : lcid;
  if (lcid_with_prio == INVALID_LCID) {
    return 0;
  }

  // Update Buffer Status of allocated LCID.
  return allocate_mac_sdu(subpdu, lcid_with_prio, rem_bytes);
}

lcid_t dl_logical_channel_manager::get_max_prio_lcid() const
{
  for (const auto lcid : sorted_channels) {
    if (channels[lcid].active and channels[lcid].buf_st > 0) {
      return lcid;
    }
  }
  return INVALID_LCID;
}

unsigned dl_logical_channel_manager::allocate_mac_sdu(dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes)
{
  srsran_sanity_check(lcid < MAX_NOF_RB_LCIDS, "Max LCID value 32 exceeded");

  unsigned lch_bytes = pending_bytes(lcid);
  if (lch_bytes == 0 or rem_bytes <= MIN_MAC_SDU_SUBHEADER_SIZE) {
    return 0;
  }

  // Account for available space and MAC subheader to decide the number of bytes to allocate.
  unsigned alloc_bytes = std::min(rem_bytes, get_mac_sdu_required_bytes(lch_bytes));

  // Allocate all leftover bytes in following cases:
  //  - If it is last PDU of the TBS.
  //  - [Implementation-defined] If \c leftover_bytes is < 5 bytes, as it results in small SDU size.
  unsigned leftover_bytes = rem_bytes - alloc_bytes;
  if (leftover_bytes > 0 and ((leftover_bytes <= MAX_MAC_SDU_SUBHEADER_SIZE + 1) or total_pending_bytes() == 0)) {
    alloc_bytes += leftover_bytes;
  }
  if (alloc_bytes == MAC_SDU_SUBHEADER_LENGTH_THRES + MIN_MAC_SDU_SUBHEADER_SIZE) {
    // Avoid invalid combination of MAC subPDU and subheader size.
    alloc_bytes--;
  }
  unsigned sdu_size = get_mac_sdu_size(alloc_bytes);

  // Update DL Buffer Status to avoid reallocating the same LCID bytes.
  channels[lcid].buf_st -= std::min(sdu_size, channels[lcid].buf_st);

  if (lcid != LCID_SRB0 and channels[lcid].buf_st > 0) {
    constexpr static unsigned RLC_SEGMENTATION_OVERHEAD = 4;
    // Allocation was not enough to empty the logical channel. In this specific case, we add some bytes to account
    // for the RLC segmentation overhead.
    // Note: This update is only relevant for PDSCH allocations for slots > slot_tx. For the case of PDSCH
    // slot==slot_tx, there will be an RLC Buffer Occupancy update right away, which will set a new buffer value.
    channels[lcid].buf_st += RLC_SEGMENTATION_OVERHEAD;
  }

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

unsigned
srsran::allocate_mac_sdus(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs, lcid_t lcid)
{
  static constexpr unsigned min_mac_sdu_space = 4; // Needs to fit at least MAC SDU subheader and RLC header.
  unsigned                  rem_tbs           = total_tbs;

  // If we do not have enough bytes to fit MAC subheader, skip MAC SDU allocation.
  // Note: We assume upper layer accounts for its own subheaders when updating the buffer state.
  while (rem_tbs > min_mac_sdu_space and not tb_info.lc_chs_to_sched.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_mac_sdu(subpdu, rem_tbs, lcid);
    if (alloc_bytes == 0) {
      break;
    }

    // Add new subPDU.
    tb_info.lc_chs_to_sched.push_back(subpdu);

    // Update remaining space taking into account the MAC SDU subheader.
    rem_tbs -= alloc_bytes;
  }

  return total_tbs - rem_tbs;
}

unsigned srsran::allocate_mac_ces(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs)
{
  unsigned rem_tbs = total_tbs;

  while (lch_mng.has_pending_ces() and not tb_info.lc_chs_to_sched.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_mac_ce(subpdu, rem_tbs);
    if (alloc_bytes == 0) {
      break;
    }

    // Add new subPDU.
    tb_info.lc_chs_to_sched.push_back(subpdu);

    // Update remaining space taking into account the MAC CE subheader.
    rem_tbs -= alloc_bytes;
  }
  return total_tbs - rem_tbs;
}

unsigned dl_logical_channel_manager::allocate_ue_con_res_id_mac_ce(dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  if (not is_con_res_id_pending()) {
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

span<const lcid_t> dl_logical_channel_manager::get_prioritized_logical_channels() const
{
  return sorted_channels;
}

unsigned
srsran::allocate_ue_con_res_id_mac_ce(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs)
{
  unsigned rem_tbs = total_tbs;

  if (not tb_info.lc_chs_to_sched.full()) {
    dl_msg_lc_info subpdu;
    unsigned       alloc_bytes = lch_mng.allocate_ue_con_res_id_mac_ce(subpdu, rem_tbs);
    if (alloc_bytes != 0) {
      // Add new subPDU.
      tb_info.lc_chs_to_sched.push_back(subpdu);

      // Update remaining space taking into account the MAC CE subheader.
      rem_tbs -= alloc_bytes;
    }
  }
  return total_tbs - rem_tbs;
}
