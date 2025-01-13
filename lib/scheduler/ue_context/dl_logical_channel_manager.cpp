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

#include "dl_logical_channel_manager.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"

using namespace srsran;

static const logical_channel_config& get_default_logical_channel_config(lcid_t lcid)
{
  // Default logical channel configs.
  constexpr static logical_channel_config lcid0_cfg = config_helpers::create_default_logical_channel_config(LCID_SRB0);
  constexpr static logical_channel_config lcid1_cfg = config_helpers::create_default_logical_channel_config(LCID_SRB1);
  constexpr static logical_channel_config lcid2_cfg = config_helpers::create_default_logical_channel_config(LCID_SRB2);
  constexpr static logical_channel_config default_drb_cfg =
      config_helpers::create_default_logical_channel_config(LCID_MIN_DRB);
  switch (lcid) {
    case LCID_SRB0:
      return lcid0_cfg;
    case LCID_SRB1:
      return lcid1_cfg;
    case LCID_SRB2:
      return lcid2_cfg;
    default:
      break;
  }
  return default_drb_cfg;
}

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
  // Reserve entries to avoid allocating in hot path.
  sorted_channels.reserve(LCID_MIN_DRB);

  // SRB0 is always activated.
  set_status(LCID_SRB0, true);
}

void dl_logical_channel_manager::deactivate()
{
  for (unsigned lcid = LCID_SRB0; lcid <= LCID_MAX_DRB; lcid++) {
    set_status((lcid_t)lcid, false);
  }
  pending_ces.clear();
  sorted_channels.clear();
}

void dl_logical_channel_manager::set_fallback_state(bool enter_fallback)
{
  if (fallback_state == enter_fallback) {
    // no-op.
    return;
  }

  fallback_state = enter_fallback;
}

static uint8_t get_lc_prio(const logical_channel_config& cfg)
{
  uint8_t prio = 0;
  if (is_srb(cfg.lcid)) {
    prio = cfg.lcid <= LCID_SRB1 ? 0 : 1;
  } else {
    prio = cfg.qos.has_value() ? cfg.qos->qos.qos_priority_level : MAX_QOS_PRIORITY_LEVEL;
  }
  return prio;
}

void dl_logical_channel_manager::set_status(lcid_t lcid, bool active)
{
  srsran_sanity_check(lcid < MAX_NOF_RB_LCIDS, "Max LCID value 32 exceeded");

  if (channels[lcid].active == active and channels[lcid].cfg != nullptr) {
    // No state change.
    return;
  }

  // Update channel config.
  const bool new_lc = channels[lcid].cfg == nullptr;
  auto       it =
      std::find_if(channel_configs.begin(), channel_configs.end(), [lcid](const auto& c) { return c.lcid == lcid; });
  if (it != channel_configs.end()) {
    // In case the channel config was specified.
    channels[lcid].cfg = it;
  } else {
    // in case it was not specified, fallback to default config.
    channels[lcid].cfg = &get_default_logical_channel_config(lcid);
  }

  // set new state.
  channels[lcid].active = active;

  // Refresh sorted_channels list.
  if (new_lc) {
    sorted_channels.push_back(lcid);
  }
  std::sort(sorted_channels.begin(), sorted_channels.end(), [this](lcid_t lhs, lcid_t rhs) {
    return get_lc_prio(*channels[lhs].cfg) < get_lc_prio(*channels[rhs].cfg);
  });
}

void dl_logical_channel_manager::configure(span<const logical_channel_config> log_channels_configs)
{
  const bool cfg_changed = channel_configs != log_channels_configs;
  channel_configs        = log_channels_configs;

  if (not cfg_changed) {
    // No change in the config. However, we may need to update channel config pointers.
    for (const logical_channel_config& ch_cfg : channel_configs) {
      channels[ch_cfg.lcid].cfg = &ch_cfg;
    }
    return;
  }

  // If a previously custom configured LC is not in the list of new configs, we delete it.
  // Note: LCID will be removed from sorted_channels later.
  for (lcid_t lcid : sorted_channels) {
    if (channels[lcid].cfg != nullptr) {
      auto it = std::find_if(
          channel_configs.begin(), channel_configs.end(), [lcid](const auto& c) { return c.lcid == lcid; });
      if (it == channel_configs.end() and channels[lcid].cfg != &get_default_logical_channel_config(lcid)) {
        channels[lcid] = {};
      }
    }
  }

  // Set new LC configurations.
  for (const logical_channel_config& ch_cfg : channel_configs) {
    channels[ch_cfg.lcid].cfg    = &ch_cfg;
    channels[ch_cfg.lcid].active = true;
    // buffer state stays the same when configuration is updated.
  }

  // Refresh sorted channels list.
  sorted_channels.clear();
  sorted_channels.reserve(channels.size());
  for (unsigned lcid = 0, e = channels.size(); lcid != e; ++lcid) {
    if (channels[lcid].cfg != nullptr) {
      sorted_channels.push_back(channels[lcid].cfg->lcid);
    }
  }
  std::sort(sorted_channels.begin(), sorted_channels.end(), [this](lcid_t lhs, lcid_t rhs) {
    return get_lc_prio(*channels[lhs].cfg) < get_lc_prio(*channels[rhs].cfg);
  });
}

bool dl_logical_channel_manager::has_pending_bytes(const bounded_bitset<MAX_NOF_RB_LCIDS>& bearers) const
{
  if (fallback_state) {
    bool srb0_chosen = bearers.size() > 0 and bearers.test(LCID_SRB0);
    bool srb1_chosen = bearers.size() > 1 and bearers.test(LCID_SRB1);
    if (not srb0_chosen and not srb1_chosen) {
      return false;
    }
    return is_con_res_id_pending() or (srb0_chosen and has_pending_bytes(LCID_SRB0)) or
           (srb1_chosen and has_pending_bytes(LCID_SRB1));
  }

  // If at least one slice bearer has pending data, return true.
  for (lcid_t lcid : sorted_channels) {
    if (lcid < bearers.size() and bearers.test(lcid) and has_pending_bytes(lcid)) {
      return true;
    }
  }

  // In case SRB1 was selected (but with no data) and there are pending CE bytes.
  if (bearers.size() > 1 and bearers.test(LCID_SRB1) and has_pending_ces()) {
    // Check if any other bearer has pending data. If they do, CE is not considered.
    // Note: This extra check is to avoid multiple slices report pending data.
    for (lcid_t lcid : sorted_channels) {
      if (lcid > LCID_SRB1 and has_pending_bytes(lcid)) {
        return false;
      }
    }
    return true;
  }

  return false;
}

unsigned dl_logical_channel_manager::total_pending_bytes() const
{
  unsigned bytes = pending_ce_bytes();
  for (lcid_t lcid : sorted_channels) {
    bytes += pending_bytes(lcid);
  }
  return bytes;
}

unsigned dl_logical_channel_manager::pending_bytes() const
{
  if (fallback_state) {
    return pending_con_res_ce_bytes() + pending_bytes(LCID_SRB0) + pending_bytes(LCID_SRB1);
  }
  unsigned bytes = pending_ce_bytes();
  for (lcid_t lcid : sorted_channels) {
    bytes += lcid != LCID_SRB0 ? pending_bytes(lcid) : 0;
  }
  return bytes;
}

unsigned dl_logical_channel_manager::pending_bytes(const bounded_bitset<MAX_NOF_RB_LCIDS>& bearers) const
{
  if (fallback_state) {
    bool srb0_chosen = bearers.size() > 0 and bearers.test(LCID_SRB0);
    bool srb1_chosen = bearers.size() > 1 and bearers.test(LCID_SRB1);
    if (not srb0_chosen and not srb1_chosen) {
      return 0;
    }
    return pending_con_res_ce_bytes() + (srb0_chosen ? pending_bytes(LCID_SRB0) : 0) +
           (srb1_chosen ? pending_bytes(LCID_SRB1) : 0);
  }

  // Compute pending bytes for the given bearers.
  unsigned total_bytes = 0;
  for (lcid_t lcid : sorted_channels) {
    if (lcid < bearers.size() and bearers.test(lcid)) {
      total_bytes += pending_bytes(lcid);
    }
  }

  unsigned ce_bytes = pending_ce_bytes();
  if (ce_bytes > 0) {
    // There are also pending CE bytes.
    if (total_bytes > 0) {
      // In case the UE has pending bearer bytes, we also include the CE bytes.
      total_bytes += ce_bytes;
    } else if (bearers.size() > 1 and bearers.test(LCID_SRB1)) {
      // In case SRB1 was selected, and there are no pending bytes in the selected bearers, we return the pending CE
      // bytes iff the UE has no pending data on the remaining, non-selected bearers.
      // This is to avoid the situation where a UE, for instance, has DRB data to transmit, but the CE is allocated in
      // the SRB slice instead.
      for (lcid_t lcid : sorted_channels) {
        if (lcid > LCID_SRB1 and pending_bytes(lcid) > 0) {
          return 0;
        }
      }
      return ce_bytes;
    }
  }

  return total_bytes;
}

void dl_logical_channel_manager::handle_mac_ce_indication(const mac_ce_info& ce)
{
  if (ce.ce_lcid == lcid_dl_sch_t::UE_CON_RES_ID) {
    // CON RES is a special case, as it needs to be always scheduled first.
    pending_con_res_id = true;
    return;
  }
  if (ce.ce_lcid == lcid_dl_sch_t::TA_CMD) {
    auto ce_it = std::find_if(pending_ces.begin(), pending_ces.end(), [](const mac_ce_info& c) {
      return c.ce_lcid == lcid_dl_sch_t::TA_CMD;
    });
    if (ce_it != pending_ces.end()) {
      ce_it->ce_payload = ce.ce_payload;
      return;
    }
  }
  pending_ces.push_back(ce);
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
    if (has_pending_bytes(lcid)) {
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
