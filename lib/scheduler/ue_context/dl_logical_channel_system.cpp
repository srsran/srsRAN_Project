/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dl_logical_channel_system.h"

using namespace srsran;

static unsigned get_mac_sdu_size(unsigned sdu_and_subheader_bytes)
{
  if (sdu_and_subheader_bytes == 0) {
    return 0;
  }
  const unsigned sdu_size = sdu_and_subheader_bytes - MIN_MAC_SDU_SUBHEADER_SIZE;
  return sdu_size < MAC_SDU_SUBHEADER_LENGTH_THRES ? sdu_size : sdu_size - 1;
}

dl_logical_channel_system::dl_logical_channel_system()
{
  static constexpr unsigned PRERESERVED_NOF_LCS_PER_UE  = 5;
  static constexpr unsigned PRERESERVED_NOF_DRBS_PER_UE = 2;
  static constexpr unsigned PRERESERVED_NOF_CES         = MAX_NOF_DU_UES;

  pending_ces.reserve(PRERESERVED_NOF_CES);
  qos_channels.reserve(PRERESERVED_NOF_DRBS_PER_UE * MAX_NOF_DU_UES);
  configured_channels.reserve(PRERESERVED_NOF_LCS_PER_UE * MAX_NOF_DU_UES);
  ue_contexts.reserve(MAX_NOF_DU_UES);
}

void dl_logical_channel_system::slot_indication()
{
  // Update the bit rates of the UE logical channels with tracked bit rates.
  for (auto row_view : qos_channels) {
    auto& item = row_view.at<0>();
    item.avg_bytes_per_slot.push(item.last_sched_bytes);
    item.last_sched_bytes = 0;
  }
}

ue_dl_logical_channel_repository
dl_logical_channel_system::create_ue(subcarrier_spacing              scs_common,
                                     bool                            starts_in_fallback,
                                     logical_channel_config_list_ptr log_channels_configs)
{
  soa::row_id ue_rid    = ue_contexts.insert(ue_context{});
  ue_context& ue_ctx    = ue_contexts.row(ue_rid).at<0>();
  ue_ctx.fallback_state = starts_in_fallback;
  ue_ctx.slots_per_msec = get_nof_slots_per_subframe(scs_common);
  if (log_channels_configs.has_value()) {
    configure(ue_rid, log_channels_configs);
  }
  return ue_dl_logical_channel_repository{*this, ue_rid};
}

void ue_dl_logical_channel_repository::set_fallback_state(bool enter_fallback)
{
  ue_context& ue_ctx = get_ue_ctx();
  if (ue_ctx.fallback_state == enter_fallback) {
    // no-op.
    return;
  }
  ue_ctx.fallback_state = enter_fallback;
}

static uint16_t get_lc_prio(const logical_channel_config& cfg)
{
  uint16_t prio = 0;
  if (is_srb(cfg.lcid)) {
    prio = cfg.lcid <= LCID_SRB1 ? 0 : 1;
  } else {
    prio = cfg.qos.has_value() ? cfg.qos->qos.priority.value() * cfg.qos->arp_priority.value()
                               : qos_prio_level_t::max() * arp_prio_level_t::max();
  }
  return prio;
}

void dl_logical_channel_system::remove_ue(soa::row_id ue_rid)
{
  deactivate(ue_rid);

  auto& ue_ctx = get_ue_ctx(ue_rid);

  // Delete configured logical channels.
  for (soa::row_id ch_row_id : ue_ctx.channels) {
    configured_channels.erase(ch_row_id);
  }
  ue_ctx.channels.clear();

  // Finally destroy UE context.
  ue_contexts.erase(ue_rid);
}

void dl_logical_channel_system::configure(soa::row_id ue_rid, logical_channel_config_list_ptr log_channels_configs)
{
  ue_context& ue_ctx     = get_ue_ctx(ue_rid);
  auto        old_cfgs   = ue_ctx.channel_configs;
  ue_ctx.channel_configs = log_channels_configs;

  // If a previously custom configured LC is not in the list of new configs, we delete it.
  // Note: LCID will be removed from sorted_channels later.
  if (old_cfgs.has_value()) {
    for (const auto& old_lc : *old_cfgs) {
      if (not ue_ctx.channel_configs->contains(old_lc->lcid)) {
        // LCID got removed.
        soa::row_id      ch_row_id = ue_ctx.channels[old_lc->lcid];
        channel_context& ch_ctx    = configured_channels.row(ch_row_id).at<0>();

        // Detach from QoS tracking.
        if (ch_ctx.stats_row.has_value()) {
          qos_channels.erase(*ch_ctx.stats_row);
        }

        // Detach from RAN slice.
        deregister_lc_ran_slice(ue_rid, old_lc->lcid);

        // Finally delete the LC context.
        configured_channels.erase(ue_ctx.channels[old_lc->lcid]);
        ue_ctx.channels.erase(old_lc->lcid);
      }
    }
  }

  // Set new LC configurations.
  for (logical_channel_config_ptr ch_cfg : *ue_ctx.channel_configs) {
    const bool       new_ch = not ue_ctx.channels.contains(ch_cfg->lcid);
    channel_context* ch_ctx;
    if (new_ch) {
      // New LCID, create context.
      auto ch_row_id = configured_channels.insert(channel_context{});
      ch_ctx         = &configured_channels.row(ch_row_id).at<0>();
      ue_ctx.channels.emplace(ch_cfg->lcid, ch_row_id);
    } else {
      // Fetch existing context.
      ch_ctx = &configured_channels.row(ue_ctx.channels[ch_cfg->lcid]).at<0>();
    }

    // Implicitly set channel as active when configured.
    if (not ch_ctx->active and ch_ctx->slice_id.has_value()) {
      // Update slice pending bytes.
      auto& lst_pair = ue_ctx.slice_lcid_list_lookup[*ch_ctx->slice_id];
      lst_pair.first += get_mac_sdu_required_bytes(ch_ctx->buf_st);
    }
    ch_ctx->active = true;

    // Handle GBR QoS tracking.
    const bool qos_is_tracked =
        not is_srb(ch_cfg->lcid) and ch_cfg->qos.has_value() and ch_cfg->qos.value().gbr_qos_info.has_value();
    if (qos_is_tracked) {
      const unsigned win_size_msec = ch_cfg->qos.value().qos.average_window_ms.value();
      if (not ch_ctx->stats_row.has_value()) {
        // New GBR QoS, initiate tracking.
        auto qos_rid      = qos_channels.insert(qos_context{});
        ch_ctx->stats_row = qos_rid;
      }
      qos_channels.row(*ch_ctx->stats_row).at<0>().avg_bytes_per_slot.resize(win_size_msec * ue_ctx.slots_per_msec);
    } else if (ch_ctx->stats_row.has_value()) {
      // Remove from QoS tracking.
      qos_channels.erase(*ch_ctx->stats_row);
      ch_ctx->stats_row.reset();
    }
  }

  // Refresh sorted channels list.
  ue_ctx.sorted_channels.clear();
  for (const auto& lc_cfg : *ue_ctx.channel_configs) {
    ue_ctx.sorted_channels.push_back(lc_cfg->lcid);
  }
  std::sort(ue_ctx.sorted_channels.begin(), ue_ctx.sorted_channels.end(), [&ue_ctx](lcid_t lhs, lcid_t rhs) {
    return get_lc_prio(*ue_ctx.channel_configs.value()[lhs]) < get_lc_prio(*ue_ctx.channel_configs.value()[rhs]);
  });
}

void dl_logical_channel_system::deactivate(soa::row_id ue_rid)
{
  ue_context& ue_ctx = get_ue_ctx(ue_rid);

  // Clear pending CEs.
  ue_ctx.pending_con_res_id = false;
  for (std::optional<soa::row_id> ce_rid = ue_ctx.pending_ces; ce_rid.has_value();) {
    auto next = pending_ces.row(*ce_rid).at<0>().next_ue_ce;
    pending_ces.erase(*ce_rid);
    ce_rid = next;
  }
  ue_ctx.pending_ces      = std::nullopt;
  ue_ctx.pending_ce_bytes = 0;

  // Deactivate all bearers.
  for (soa::row_id ch_row_id : ue_ctx.channels) {
    channel_context& ch_ctx = configured_channels.row(ch_row_id).at<0>();

    if (ch_ctx.stats_row.has_value()) {
      // Stop tracking QoS.
      qos_channels.erase(*ch_ctx.stats_row);
    }

    // Detach from RAN slice.
    if (ch_ctx.slice_id.has_value()) {
      // Erase LC from the slice lookup.
      auto& lst_pair = ue_ctx.slice_lcid_list_lookup[*ch_ctx.slice_id];
      lst_pair.first -= get_mac_sdu_required_bytes(ch_ctx.active ? ch_ctx.buf_st : 0);
      auto& lst = lst_pair.second;
      lst.erase(std::remove_if(lst.begin(), lst.end(), [ch_row_id](soa::row_id rid) { return rid == ch_row_id; }),
                lst.end());
      ch_ctx.slice_id.reset();
    }

    ch_ctx.active = false;
  }

  // Clear channel lookups.
  ue_ctx.sorted_channels.clear();
  ue_ctx.slice_lcid_list_lookup.clear();
}

void dl_logical_channel_system::set_lcid_ran_slice(soa::row_id ue_rid, lcid_t lcid, ran_slice_id_t slice_id)
{
  ue_context& ue_ctx = get_ue_ctx(ue_rid);
  srsran_assert(ue_ctx.slice_lcid_list_lookup.contains(slice_id), "Invalid slice ID");
  srsran_assert(ue_ctx.channels.contains(lcid), "LCID not configured");
  soa::row_id      ch_row_id = ue_ctx.channels[lcid];
  channel_context& ch_ctx    = configured_channels.row(ch_row_id).at<0>();
  if (ch_ctx.slice_id == slice_id) {
    // No-op.
    return;
  }

  // Remove LCID from previous slice.
  deregister_lc_ran_slice(ue_rid, lcid);

  // Add LCID to new slice.
  ue_ctx.slice_lcid_list_lookup[slice_id].second.push_back(ch_row_id);
  ch_ctx.slice_id = slice_id;
  ue_ctx.slice_lcid_list_lookup[slice_id].first += get_mac_sdu_required_bytes(ch_ctx.active ? ch_ctx.buf_st : 0);
}

void dl_logical_channel_system::register_ran_slice(soa::row_id ue_rid, ran_slice_id_t slice_id)
{
  ue_context& ue_ctx = get_ue_ctx(ue_rid);
  ue_ctx.slice_lcid_list_lookup.emplace(slice_id, 0, std::vector<soa::row_id>{});
}

void dl_logical_channel_system::deregister_lc_ran_slice(soa::row_id ue_rid, lcid_t lcid)
{
  auto& ue_ctx = get_ue_ctx(ue_rid);
  if (ue_ctx.channels.contains(lcid)) {
    soa::row_id      ch_row_id = ue_ctx.channels[lcid];
    channel_context& ch_ctx    = configured_channels.row(ch_row_id).at<0>();
    if (ch_ctx.slice_id.has_value()) {
      // Erase LC from the slice lookup.
      auto& lst_pair = ue_ctx.slice_lcid_list_lookup[*ch_ctx.slice_id];
      auto& lst      = lst_pair.second;
      lst_pair.first -= get_mac_sdu_required_bytes(ch_ctx.active ? ch_ctx.buf_st : 0);
      lst.erase(std::remove_if(lst.begin(), lst.end(), [ch_row_id](soa::row_id rid) { return rid == ch_row_id; }),
                lst.end());
    }
    ch_ctx.slice_id.reset();
  }
}

void dl_logical_channel_system::deregister_ran_slice(soa::row_id ue_rid, ran_slice_id_t slice_id)
{
  auto& ue_ctx = get_ue_ctx(ue_rid);
  if (ue_ctx.slice_lcid_list_lookup.contains(slice_id)) {
    for (soa::row_id ch_rid : ue_ctx.slice_lcid_list_lookup[slice_id].second) {
      configured_channels.row(ch_rid).at<0>().slice_id.reset();
    }
    ue_ctx.slice_lcid_list_lookup.erase(slice_id);
  }
}

void dl_logical_channel_system::handle_dl_buffer_status_indication(soa::row_id ue_row_id,
                                                                   lcid_t      lcid,
                                                                   unsigned    buffer_status,
                                                                   slot_point  hol_toa)
{
  // We apply this limit to avoid potential overflows.
  static constexpr unsigned max_buffer_status = 1U << 24U;
  srsran_sanity_check(lcid < MAX_NOF_RB_LCIDS, "Max LCID value 32 exceeded");
  auto& ue_ctx = get_ue_ctx(ue_row_id);
  if (ue_ctx.channels.contains(lcid)) {
    auto& ch_ctx      = configured_channels.row(ue_ctx.channels[lcid]).at<0>();
    auto  prev_buf_st = ch_ctx.buf_st;
    ch_ctx.buf_st     = std::min(buffer_status, max_buffer_status);
    ch_ctx.hol_toa    = hol_toa;
    if (ch_ctx.slice_id.has_value()) {
      ue_ctx.slice_lcid_list_lookup[*ch_ctx.slice_id].first +=
          ch_ctx.active ? (get_mac_sdu_required_bytes(ch_ctx.buf_st) - get_mac_sdu_required_bytes(prev_buf_st)) : 0;
    }
  }
}

bool dl_logical_channel_system::handle_mac_ce_indication(soa::row_id ue_row_id, const mac_ce_info& ce)
{
  ue_context& ue_ctx = get_ue_ctx(ue_row_id);
  if (ce.ce_lcid == lcid_dl_sch_t::UE_CON_RES_ID) {
    // CON RES is a special case, as it needs to be always scheduled first.
    ue_ctx.pending_con_res_id = true;
    return true;
  }
  std::optional<soa::row_id> prev_ce_it;
  for (auto it = ue_ctx.pending_ces; it.has_value();) {
    auto& ce_ctx = pending_ces.row(it.value()).at<0>();
    if (ce.ce_lcid == lcid_dl_sch_t::TA_CMD and ce_ctx.info.ce_lcid == lcid_dl_sch_t::TA_CMD) {
      // Overwrite previous TA CMD CE.
      // Note: Size of TA CMD CE is fixed, so no need to update pending CE bytes.
      ce_ctx.info.ce_payload = ce.ce_payload;
      return true;
    }
    prev_ce_it = it;
    it         = ce_ctx.next_ue_ce;
  }
  // New CE, append to the list.
  auto ce_rid = pending_ces.insert(mac_ce_context{ce, std::nullopt});
  if (prev_ce_it.has_value()) {
    // Append CE to the end of the list.
    pending_ces.row(prev_ce_it.value()).at<0>().next_ue_ce = ce_rid;
  } else {
    // New head of the list.
    ue_ctx.pending_ces = ce_rid;
  }
  // Update sum of pending CE bytes.
  ue_ctx.pending_ce_bytes += ce.ce_lcid.is_var_len_ce() ? get_mac_sdu_required_bytes(ce.ce_lcid.sizeof_ce())
                                                        : FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce.ce_lcid.sizeof_ce();
  return true;
}

unsigned dl_logical_channel_system::total_pending_bytes(const ue_context& ue_ctx) const
{
  unsigned bytes = pending_ce_bytes(ue_ctx);
  for (lcid_t lcid : ue_ctx.sorted_channels) {
    bytes += pending_bytes(ue_ctx, lcid);
  }
  return bytes;
}

unsigned dl_logical_channel_system::pending_ce_bytes(const ue_context& ue_ctx) const
{
  return pending_con_res_ce_bytes(ue_ctx) + ue_ctx.pending_ce_bytes;
}

lcid_t dl_logical_channel_system::get_max_prio_lcid(soa::row_id ue_rid) const
{
  const auto& ue_ctx = get_ue_ctx(ue_rid);
  for (const auto lcid : ue_ctx.sorted_channels) {
    if (has_pending_bytes(ue_ctx, lcid)) {
      return lcid;
    }
  }
  return INVALID_LCID;
}

unsigned
dl_logical_channel_system::allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes)
{
  srsran_sanity_check(lcid < MAX_NOF_RB_LCIDS, "Max LCID value 32 exceeded");
  if (rem_bytes <= MIN_MAC_SDU_SUBHEADER_SIZE) {
    return 0;
  }
  auto&    ue_ctx              = get_ue_ctx(ue_rid);
  unsigned lch_bytes_and_subhr = pending_bytes(ue_ctx, lcid);
  if (lch_bytes_and_subhr == 0) {
    return 0;
  }

  // Account for available space and MAC subheader to decide the number of bytes to allocate.
  unsigned alloc_bytes = std::min(rem_bytes, lch_bytes_and_subhr);

  // Allocate all leftover bytes in following cases:
  //  - If it is last PDU of the TBS.
  //  - [Implementation-defined] If \c leftover_bytes is < 5 bytes, as it results in small SDU size.
  unsigned leftover_bytes = rem_bytes - alloc_bytes;
  if (leftover_bytes > 0 and ((leftover_bytes <= MAX_MAC_SDU_SUBHEADER_SIZE + 1) or total_pending_bytes(ue_ctx) == 0)) {
    alloc_bytes += leftover_bytes;
  }
  if (alloc_bytes == MAC_SDU_SUBHEADER_LENGTH_THRES + MIN_MAC_SDU_SUBHEADER_SIZE) {
    // Avoid invalid combination of MAC subPDU and subheader size.
    alloc_bytes--;
  }
  unsigned sdu_size = get_mac_sdu_size(alloc_bytes);

  // Update DL Buffer Status to avoid reallocating the same LCID bytes.
  channel_context& ch_ctx           = configured_channels.row(ue_ctx.channels[lcid]).at<0>();
  const unsigned   last_sched_bytes = std::min(sdu_size, ch_ctx.buf_st);
  auto             prev_buf_st      = ch_ctx.buf_st;
  ch_ctx.buf_st -= last_sched_bytes;
  if (ch_ctx.stats_row.has_value()) {
    // QoS tracking.
    qos_channels.row(*ch_ctx.stats_row).at<0>().last_sched_bytes = last_sched_bytes;
  }
  if (ch_ctx.slice_id.has_value()) {
    auto& slice_pending_bytes = ue_ctx.slice_lcid_list_lookup[*ch_ctx.slice_id].first;
    srsran_sanity_check(slice_pending_bytes + get_mac_sdu_required_bytes(ch_ctx.buf_st) >=
                            get_mac_sdu_required_bytes(prev_buf_st),
                        "Invalid slice pending bytes");
    slice_pending_bytes += get_mac_sdu_required_bytes(ch_ctx.buf_st) - get_mac_sdu_required_bytes(prev_buf_st);
  }

  if (lcid != LCID_SRB0 and ch_ctx.buf_st > 0) {
    static constexpr unsigned RLC_SEGMENTATION_OVERHEAD = 4;
    // Allocation was not enough to empty the logical channel. In this specific case, we add some bytes to account
    // for the RLC segmentation overhead.
    // Note: This update is only relevant for PDSCH allocations for slots > slot_tx. For the case of PDSCH
    // slot==slot_tx, there will be an RLC Buffer Occupancy update right away, which will set a new buffer value.
    prev_buf_st = ch_ctx.buf_st;
    ch_ctx.buf_st += RLC_SEGMENTATION_OVERHEAD;
    if (ch_ctx.slice_id.has_value()) {
      auto& slice_pending_bytes = ue_ctx.slice_lcid_list_lookup[*ch_ctx.slice_id].first;
      slice_pending_bytes += get_mac_sdu_required_bytes(ch_ctx.buf_st) - get_mac_sdu_required_bytes(prev_buf_st);
    }
  }

  subpdu.lcid        = (lcid_dl_sch_t::options)lcid;
  subpdu.sched_bytes = sdu_size;

  return alloc_bytes;
}

unsigned
dl_logical_channel_system::allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& subpdu, unsigned rem_bytes, lcid_t lcid)
{
  subpdu.lcid        = lcid_dl_sch_t::MIN_RESERVED;
  subpdu.sched_bytes = 0;

  lcid_t lcid_with_prio = lcid == INVALID_LCID ? get_max_prio_lcid(ue_rid) : lcid;
  if (lcid_with_prio == INVALID_LCID) {
    return 0;
  }

  // Update Buffer Status of allocated LCID.
  return allocate_mac_sdu(ue_rid, subpdu, lcid_with_prio, rem_bytes);
}

unsigned dl_logical_channel_system::allocate_mac_ce(soa::row_id ue_rid, dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  subpdu.lcid        = lcid_dl_sch_t::MIN_RESERVED;
  subpdu.sched_bytes = 0;

  // Allocate ConRes CE ID if pending.
  unsigned alloc_bytes = allocate_ue_con_res_id_mac_ce(ue_rid, subpdu, rem_bytes);
  if (alloc_bytes > 0) {
    return alloc_bytes;
  }

  auto& ue_ctx = get_ue_ctx(ue_rid);
  if (not ue_ctx.pending_ces.has_value()) {
    return 0;
  }
  auto                ce_row_id = *ue_ctx.pending_ces;
  const auto&         ce_node   = pending_ces.row(ce_row_id).at<0>();
  const lcid_dl_sch_t lcid      = ce_node.info.ce_lcid;

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
  subpdu.ce_payload  = ce_node.info.ce_payload;

  // Pop MAC CE.
  ue_ctx.pending_ces = ce_node.next_ue_ce;
  // Update sum of pending CE bytes.
  ue_ctx.pending_ce_bytes -= ce_node.info.ce_lcid.is_var_len_ce()
                                 ? get_mac_sdu_required_bytes(ce_node.info.ce_lcid.sizeof_ce())
                                 : FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce_node.info.ce_lcid.sizeof_ce();
  pending_ces.erase(ce_row_id);

  return alloc_bytes;
}

unsigned
dl_logical_channel_system::allocate_ue_con_res_id_mac_ce(soa::row_id ue_rid, dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  auto& ue_ctx = get_ue_ctx(ue_rid);
  if (not ue_ctx.pending_con_res_id) {
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
    ue_ctx.pending_con_res_id = false;

    subpdu.lcid        = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID};
    subpdu.sched_bytes = ce_size;
  }

  return alloc_bytes;
}

// ** class logical_channel_system::ue_logical_channel_repository

void ue_dl_logical_channel_repository::configure(logical_channel_config_list_ptr log_channels_configs)
{
  parent->configure(ue_row_id, log_channels_configs);
}

void ue_dl_logical_channel_repository::deactivate()
{
  parent->deactivate(ue_row_id);
}

void ue_dl_logical_channel_repository::set_lcid_ran_slice(lcid_t lcid, ran_slice_id_t slice_id)
{
  parent->set_lcid_ran_slice(ue_row_id, lcid, slice_id);
}

void ue_dl_logical_channel_repository::register_ran_slice(ran_slice_id_t slice_id)
{
  parent->register_ran_slice(ue_row_id, slice_id);
}

void ue_dl_logical_channel_repository::reset_lcid_ran_slice(lcid_t lcid)
{
  parent->deregister_lc_ran_slice(ue_row_id, lcid);
}

void ue_dl_logical_channel_repository::deregister_ran_slice(ran_slice_id_t slice_id)
{
  parent->deregister_ran_slice(ue_row_id, slice_id);
}

bool ue_dl_logical_channel_repository::has_pending_bytes(ran_slice_id_t slice_id) const
{
  const ue_context& ue_ctx = get_ue_ctx();
  if (ue_ctx.fallback_state or not has_slice(slice_id)) {
    return false;
  }

  // Iterate through bearers of the slice until we find one with pending data.
  if (ue_ctx.slice_lcid_list_lookup[slice_id].first > 0) {
    return true;
  }

  // In case SRB slice was selected (but with no data) and there are pending CE bytes.
  if (slice_id == SRB_RAN_SLICE_ID and has_pending_ces()) {
    // Check if any other bearers have pending data. If they do, CE is not considered.
    // Note: This extra check is to avoid multiple slices report pending data.
    for (lcid_t lcid : ue_ctx.sorted_channels) {
      auto& ch_ctx = parent->configured_channels.row(ue_ctx.channels[lcid]).at<0>();
      if (ch_ctx.slice_id != SRB_RAN_SLICE_ID and has_pending_bytes(lcid)) {
        return false;
      }
    }
    return true;
  }

  return false;
}

unsigned ue_dl_logical_channel_repository::pending_bytes() const
{
  const ue_context& ue_ctx = get_ue_ctx();
  if (ue_ctx.fallback_state) {
    return pending_con_res_ce_bytes() + pending_bytes(LCID_SRB0) + pending_bytes(LCID_SRB1);
  }
  unsigned bytes = pending_ce_bytes();
  for (lcid_t lcid : ue_ctx.sorted_channels) {
    bytes += lcid != LCID_SRB0 ? pending_bytes(lcid) : 0;
  }
  return bytes;
}

unsigned ue_dl_logical_channel_repository::pending_bytes(ran_slice_id_t slice_id) const
{
  auto& ue_ctx = get_ue_ctx();
  if (ue_ctx.fallback_state or not has_slice(slice_id)) {
    return 0;
  }

  // Compute pending bytes for the given slice bearers.
  unsigned total_bytes = ue_ctx.slice_lcid_list_lookup[slice_id].first;

  unsigned ce_bytes = pending_ce_bytes();
  if (ce_bytes > 0) {
    // There are also pending CE bytes.
    if (total_bytes > 0) {
      // In case the UE has pending bearer bytes, we also include the CE bytes.
      total_bytes += ce_bytes;
    } else if (slice_id == SRB_RAN_SLICE_ID) {
      // In case SRB1 was selected, and there are no pending bytes in the selected bearers, we return the pending CE
      // bytes iff the UE has no pending data on the remaining, non-selected bearers.
      // This is to avoid the situation where a UE, for instance, has DRB data to transmit, but the CE is allocated in
      // the SRB slice instead.
      for (lcid_t lcid : ue_ctx.sorted_channels) {
        if (lcid > LCID_SRB1 and pending_bytes(lcid) > 0) {
          return 0;
        }
      }
      return ce_bytes;
    }
  }

  return total_bytes;
}

unsigned ue_dl_logical_channel_repository::allocate_mac_sdu(dl_msg_lc_info& lch_info, unsigned rem_bytes, lcid_t lcid)
{
  return parent->allocate_mac_sdu(ue_row_id, lch_info, rem_bytes, lcid);
}

unsigned ue_dl_logical_channel_repository::allocate_mac_ce(dl_msg_lc_info& lch_info, unsigned rem_bytes)
{
  return parent->allocate_mac_ce(ue_row_id, lch_info, rem_bytes);
}

unsigned ue_dl_logical_channel_repository::allocate_ue_con_res_id_mac_ce(dl_msg_lc_info& lch_info, unsigned rem_bytes)
{
  return parent->allocate_ue_con_res_id_mac_ce(ue_row_id, lch_info, rem_bytes);
}

unsigned srsran::allocate_mac_sdus(dl_msg_tb_info&                   tb_info,
                                   ue_dl_logical_channel_repository& lch_mng,
                                   unsigned                          total_tbs,
                                   lcid_t                            lcid)
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

unsigned
srsran::allocate_mac_ces(dl_msg_tb_info& tb_info, ue_dl_logical_channel_repository& lch_mng, unsigned total_tbs)
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

unsigned srsran::allocate_ue_con_res_id_mac_ce(dl_msg_tb_info&                   tb_info,
                                               ue_dl_logical_channel_repository& lch_mng,
                                               unsigned                          total_tbs)
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

unsigned srsran::build_dl_fallback_transport_block_info(dl_msg_tb_info&                   tb_info,
                                                        ue_dl_logical_channel_repository& lch_mng,
                                                        unsigned                          tb_size_bytes)
{
  unsigned total_subpdu_bytes = 0;
  total_subpdu_bytes += allocate_ue_con_res_id_mac_ce(tb_info, lch_mng, tb_size_bytes);
  // Since SRB0 PDU cannot be segmented, skip SRB0 if remaining TB size is not enough to fit entire PDU.
  if (lch_mng.has_pending_bytes(LCID_SRB0) and
      ((tb_size_bytes - total_subpdu_bytes) >= lch_mng.pending_bytes(LCID_SRB0))) {
    total_subpdu_bytes += allocate_mac_sdus(tb_info, lch_mng, tb_size_bytes - total_subpdu_bytes, LCID_SRB0);
    return total_subpdu_bytes;
  }
  total_subpdu_bytes += allocate_mac_sdus(tb_info, lch_mng, tb_size_bytes - total_subpdu_bytes, LCID_SRB1);
  return total_subpdu_bytes;
}

unsigned srsran::build_dl_transport_block_info(dl_msg_tb_info&                   tb_info,
                                               ue_dl_logical_channel_repository& lch_mng,
                                               unsigned                          tb_size_bytes,
                                               ran_slice_id_t                    slice_id)
{
  unsigned total_subpdu_bytes = 0;
  total_subpdu_bytes += allocate_mac_ces(tb_info, lch_mng, tb_size_bytes);
  for (const auto lcid : lch_mng.get_prioritized_logical_channels()) {
    if (lch_mng.get_slice_id(lcid) == slice_id) {
      total_subpdu_bytes += allocate_mac_sdus(tb_info, lch_mng, tb_size_bytes - total_subpdu_bytes, uint_to_lcid(lcid));
    }
  }
  return total_subpdu_bytes;
}
