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

/// (Implementation-defined) Estimation of how much space the MAC should leave for RLC segmentation header overhead.
static constexpr unsigned RLC_SEGMENTATION_OVERHEAD = 3;

static constexpr unsigned get_mac_sdu_with_subhdr_and_rlc_hdr_estim(lcid_t lcid, unsigned payload)
{
  const unsigned rlc_ovh = (lcid != LCID_SRB0 && payload > 0) ? RLC_SEGMENTATION_OVERHEAD : 0;
  return get_mac_sdu_required_bytes(payload + rlc_ovh);
}

/// Determine the MAC SDU size from the total size of SDU + subheader.
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
  static constexpr unsigned PRERESERVED_NOF_DRBS_PER_UE = 2;
  static constexpr unsigned PRERESERVED_NOF_CES         = MAX_NOF_DU_UES;

  // Pre-reserve space to avoid allocations in latency-critical path.
  pending_ces.reserve(PRERESERVED_NOF_CES);
  qos_channels.reserve(PRERESERVED_NOF_DRBS_PER_UE * MAX_NOF_DU_UES);
  ues.reserve(MAX_NOF_DU_UES);
}

void dl_logical_channel_system::slot_indication()
{
  // Update the bit rates of the UE logical channels with tracked bit rates.
  for (auto row_view : qos_channels) {
    auto& item = row_view.at<qos_context>();
    item.avg_bytes_per_slot.push(item.last_sched_bytes);
    item.last_sched_bytes = 0;
  }
}

ue_dl_logical_channel_repository
dl_logical_channel_system::create_ue(du_ue_index_t                   ue_index,
                                     subcarrier_spacing              scs_common,
                                     bool                            starts_in_fallback,
                                     logical_channel_config_list_ptr log_channels_configs)
{
  soa::row_id ue_rid                         = ues.insert(ue_config_context{}, ue_context{}, ue_channel_context{});
  auto        row                            = ues.row(ue_rid);
  row.at<ue_context>().fallback_state        = starts_in_fallback;
  row.at<ue_config_context>().ue_index       = ue_index;
  row.at<ue_config_context>().slots_per_msec = get_nof_slots_per_subframe(scs_common);
  if (log_channels_configs.has_value()) {
    configure(ue_rid, log_channels_configs);
  }
  return ue_dl_logical_channel_repository{*this, ue_rid};
}

size_t dl_logical_channel_system::nof_logical_channels() const
{
  size_t count = 0;
  for (const_ue_row u : ues) {
    count += u.at<ue_channel_context>().channels.size();
  }
  return count;
}

void ue_dl_logical_channel_repository::set_fallback_state(bool enter_fallback)
{
  ue_context& ue_ctx = get_ue_row().at<ue_context>();
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
  // Disable any QoS and CE tracking.
  deactivate(ue_rid);

  // Destroy UE context.
  ues.erase(ue_rid);
}

void dl_logical_channel_system::configure(soa::row_id ue_rid, logical_channel_config_list_ptr log_channels_configs)
{
  auto  u      = ues.row(ue_rid);
  auto& ue_cfg = u.at<ue_config_context>();
  auto& ue_ch  = u.at<ue_channel_context>();
  auto& ue_ctx = u.at<ue_context>();

  // Exchange new config.
  auto old_cfgs          = ue_cfg.channel_configs;
  ue_cfg.channel_configs = log_channels_configs;

  // If a previously custom configured LC is not in the list of new configs, we delete it.
  // Note: LCID will be removed from sorted_channels later.
  if (old_cfgs.has_value()) {
    for (const auto& old_lc : *old_cfgs) {
      if (not ue_cfg.channel_configs->contains(old_lc->lcid)) {
        // LCID got removed.
        channel_context& ch_ctx = ue_ch.channels[old_lc->lcid];

        // Detach from QoS tracking.
        if (ch_ctx.qos_row.has_value()) {
          qos_channels.erase(*ch_ctx.qos_row);
        }

        // Detach from RAN slice.
        deregister_lc_ran_slice(ue_rid, old_lc->lcid);

        // Finally delete the LC context.
        ue_ch.channels.erase(old_lc->lcid);
      }
    }
  }

  // Set new LC configurations.
  for (logical_channel_config_ptr ch_cfg : *ue_cfg.channel_configs) {
    const bool new_ch = not ue_ch.channels.contains(ch_cfg->lcid);
    if (new_ch) {
      // New LCID, create context.
      ue_ch.channels.emplace(ch_cfg->lcid);
    }
    channel_context& ch_ctx = ue_ch.channels[ch_cfg->lcid];

    // Implicitly set channel as active when configured.
    if (not ch_ctx.active and ch_ctx.slice_id.has_value()) {
      // Update slice pending bytes, because we are activating the logical channel.
      auto& pending_bytes = ue_ctx.find_slice(*ch_ctx.slice_id)->second;
      pending_bytes += get_mac_sdu_required_bytes(ch_ctx.buf_st);
    }
    ch_ctx.active = true;

    // Handle GBR QoS tracking.
    const bool qos_is_tracked =
        not is_srb(ch_cfg->lcid) and ch_cfg->qos.has_value() and ch_cfg->qos.value().gbr_qos_info.has_value();
    if (qos_is_tracked) {
      const unsigned win_size_msec = ch_cfg->qos.value().qos.average_window_ms.value();
      if (not ch_ctx.qos_row.has_value()) {
        // New GBR QoS, initiate tracking.
        auto qos_rid   = qos_channels.insert(qos_context{});
        ch_ctx.qos_row = qos_rid;
      }
      qos_channels.row(*ch_ctx.qos_row)
          .at<qos_context>()
          .avg_bytes_per_slot.resize(win_size_msec * ue_cfg.slots_per_msec);
    } else if (ch_ctx.qos_row.has_value()) {
      // Remove from QoS tracking.
      qos_channels.erase(*ch_ctx.qos_row);
      ch_ctx.qos_row.reset();
    }
  }

  // Refresh sorted channels list.
  ue_ch.sorted_channels.clear();
  for (const auto& lc_cfg : *ue_cfg.channel_configs) {
    ue_ch.sorted_channels.push_back(lc_cfg->lcid);
  }
  std::sort(ue_ch.sorted_channels.begin(), ue_ch.sorted_channels.end(), [&ue_cfg](lcid_t lhs, lcid_t rhs) {
    return get_lc_prio(*ue_cfg.channel_configs.value()[lhs]) < get_lc_prio(*ue_cfg.channel_configs.value()[rhs]);
  });
}

void dl_logical_channel_system::deactivate(soa::row_id ue_rid)
{
  auto                u      = ues.row(ue_rid);
  ue_channel_context& ue_ch  = u.at<ue_channel_context>();
  ue_context&         ue_ctx = u.at<ue_context>();

  // Clear UE pending CEs.
  ue_ctx.pending_con_res_id = false;
  for (std::optional<soa::row_id> ce_rid = ue_ch.pending_ces; ce_rid.has_value();) {
    auto next = pending_ces.at<mac_ce_context>(*ce_rid).next_ue_ce;
    pending_ces.erase(*ce_rid);
    ce_rid = next;
  }
  ue_ch.pending_ces       = std::nullopt;
  ue_ctx.pending_ce_bytes = 0;

  // Deactivate (not erase) all bearers.
  for (channel_context& ch_ctx : ue_ch.channels) {
    if (ch_ctx.qos_row.has_value()) {
      // Eliminate associated QoS being tracked.
      qos_channels.erase(*ch_ctx.qos_row);
    }

    // Detach from RAN slice.
    ch_ctx.slice_id.reset();

    // Disable bearer.
    ch_ctx.active = false;
  }

  // Clear channel lookups.
  ue_ch.sorted_channels.clear();
  ue_ctx.pending_bytes_per_slice.clear();
}

void dl_logical_channel_system::set_lcid_ran_slice(soa::row_id ue_rid, lcid_t lcid, ran_slice_id_t slice_id)
{
  auto        u      = ues.row(ue_rid);
  auto&       ue_ch  = u.at<ue_channel_context>();
  ue_context& ue_ctx = u.at<ue_context>();
  srsran_assert(ue_ch.channels.contains(lcid), "LCID not configured");

  channel_context& ch_ctx = ue_ch.channels[lcid];
  if (ch_ctx.slice_id == slice_id) {
    // No-op.
    return;
  }

  // Remove LCID from previous slice.
  deregister_lc_ran_slice(ue_rid, lcid);

  // If slice does not exist yet for the UE, create it.
  auto slice_it = ue_ctx.find_slice(slice_id);
  if (slice_it == ue_ctx.pending_bytes_per_slice.end()) {
    // New slice, add to the list.
    ue_ctx.pending_bytes_per_slice.emplace_back(slice_id, 0);
  }

  // Add LCID to new slice.
  ch_ctx.slice_id = slice_id;
  ue_ctx.find_slice(slice_id)->second += get_mac_sdu_required_bytes(ch_ctx.active ? ch_ctx.buf_st : 0);
}

void dl_logical_channel_system::deregister_lc_ran_slice(soa::row_id ue_rid, lcid_t lcid)
{
  auto  u      = get_ue(ue_rid);
  auto& ue_ctx = u.at<ue_context>();
  auto& ue_ch  = u.at<ue_channel_context>();
  if (ue_ch.channels.contains(lcid)) {
    channel_context& ch_ctx = ue_ch.channels[lcid];
    if (ch_ctx.slice_id.has_value()) {
      // Erase LC from the slice lookup.
      auto  slice_it    = ue_ctx.find_slice(*ch_ctx.slice_id);
      auto& slice_bytes = slice_it->second;
      slice_bytes -= get_mac_sdu_required_bytes(ch_ctx.active ? ch_ctx.buf_st : 0);
      // Remove slice from LC.
      auto prev_slice_id = *ch_ctx.slice_id;
      ch_ctx.slice_id.reset();
      // If it is the last LC attached to the slice, remove slice.
      if (std::none_of(ue_ch.channels.begin(), ue_ch.channels.end(), [prev_slice_id](const channel_context& ch) {
            return ch.slice_id == prev_slice_id;
          })) {
        // No other LC is still attached to the slice. Remove slice from UE.
        ue_ctx.pending_bytes_per_slice.erase(slice_it);
      }
    }
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
  auto  u      = get_ue(ue_row_id);
  auto& ue_ctx = u.at<ue_context>();
  auto& ue_ch  = u.at<ue_channel_context>();
  if (ue_ch.channels.contains(lcid)) {
    auto& ch_ctx      = ue_ch.channels[lcid];
    auto  prev_buf_st = ch_ctx.buf_st;
    ch_ctx.buf_st     = std::min(buffer_status, max_buffer_status);
    ch_ctx.hol_toa    = hol_toa;
    if (ch_ctx.slice_id.has_value()) {
      ue_ctx.find_slice(*ch_ctx.slice_id)->second +=
          ch_ctx.active ? (get_mac_sdu_required_bytes(ch_ctx.buf_st) - get_mac_sdu_required_bytes(prev_buf_st)) : 0;
    }
  }
}

bool dl_logical_channel_system::handle_mac_ce_indication(soa::row_id ue_row_id, const mac_ce_info& ce)
{
  auto& ue_ctx = get_ue(ue_row_id).at<ue_context>();
  if (ce.ce_lcid == lcid_dl_sch_t::UE_CON_RES_ID) {
    // CON RES is a special case, as it needs to be always scheduled first.
    ue_ctx.pending_con_res_id = true;
    return true;
  }
  auto&                      ue_ch_ctx = get_ue(ue_row_id).at<ue_channel_context>();
  std::optional<soa::row_id> prev_ce_it;
  for (auto it = ue_ch_ctx.pending_ces; it.has_value();) {
    auto& ce_ctx = pending_ces.at<mac_ce_context>(it.value());
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
    pending_ces.at<mac_ce_context>(prev_ce_it.value()).next_ue_ce = ce_rid;
  } else {
    // New head of the list.
    ue_ch_ctx.pending_ces = ce_rid;
  }
  // Update sum of pending CE bytes.
  ue_ctx.pending_ce_bytes += ce.ce_lcid.is_var_len_ce() ? get_mac_sdu_required_bytes(ce.ce_lcid.sizeof_ce())
                                                        : FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce.ce_lcid.sizeof_ce();
  return true;
}

unsigned dl_logical_channel_system::total_pending_bytes(const const_ue_row& u) const
{
  unsigned bytes = pending_ce_bytes(u);
  for (lcid_t lcid : u.at<ue_channel_context>().sorted_channels) {
    bytes += pending_bytes(u, lcid);
  }
  return bytes;
}

lcid_t dl_logical_channel_system::get_max_prio_lcid(const const_ue_row& u) const
{
  for (const auto lcid : u.at<ue_channel_context>().sorted_channels) {
    if (has_pending_bytes(u, lcid)) {
      return lcid;
    }
  }
  return INVALID_LCID;
}

unsigned
dl_logical_channel_system::allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes)
{
  srsran_sanity_check(lcid < MAX_NOF_RB_LCIDS, "Max LCID value 32 exceeded");
  const unsigned min_bytes_needed = get_mac_sdu_with_subhdr_and_rlc_hdr_estim(lcid, 1);
  if (rem_bytes < min_bytes_needed) {
    // There is no space even for a minimal MAC SDU.
    return 0;
  }
  auto  u         = get_ue(ue_rid);
  auto& ue_ch_ctx = u.at<ue_channel_context>();
  auto& ue_ctx    = u.at<ue_context>();

  unsigned lch_bytes_and_subhr = pending_bytes(u, lcid);
  if (lch_bytes_and_subhr == 0) {
    return 0;
  }
  // This ensures that regardless of the pending bytes, enough space is allocated for the RLC overhead.
  lch_bytes_and_subhr = std::max(lch_bytes_and_subhr, min_bytes_needed);

  // Account for available space and MAC subheader to decide the number of bytes to allocate.
  unsigned alloc_bytes = std::min(rem_bytes, lch_bytes_and_subhr);

  // Allocate all leftover bytes in following cases:
  // - [Implementation-defined] If \c leftover_bytes is less than 5 bytes, as it is unlikely they will be used for
  // another SDU.
  const unsigned            leftover_bytes     = rem_bytes - alloc_bytes;
  constexpr static unsigned MIN_LEFTOVER_BYTES = 5;
  if (leftover_bytes > 0 and (leftover_bytes <= MIN_LEFTOVER_BYTES)) {
    alloc_bytes += leftover_bytes;
  }
  if (alloc_bytes == MAC_SDU_SUBHEADER_LENGTH_THRES + MIN_MAC_SDU_SUBHEADER_SIZE) {
    // Avoid invalid combination of MAC subPDU and subheader size.
    alloc_bytes--;
  }

  // Compute MAC SDU size without MAC subheader.
  const unsigned sdu_size = get_mac_sdu_size(alloc_bytes);

  // Update DL Buffer Status to avoid reallocating the same LCID bytes.
  channel_context& ch_ctx           = ue_ch_ctx.channels[lcid];
  const unsigned   last_sched_bytes = std::min(sdu_size, ch_ctx.buf_st);
  auto             prev_buf_st      = ch_ctx.buf_st;
  ch_ctx.buf_st -= last_sched_bytes;
  if (ch_ctx.qos_row.has_value()) {
    // QoS tracking.
    qos_channels.at<qos_context>(*ch_ctx.qos_row).last_sched_bytes = last_sched_bytes;
  }
  if (ch_ctx.slice_id.has_value()) {
    auto& slice_pending_bytes = ue_ctx.find_slice(*ch_ctx.slice_id)->second;
    srsran_sanity_check(slice_pending_bytes + get_mac_sdu_required_bytes(ch_ctx.buf_st) >=
                            get_mac_sdu_required_bytes(prev_buf_st),
                        "Invalid slice pending bytes");
    slice_pending_bytes += get_mac_sdu_required_bytes(ch_ctx.buf_st) - get_mac_sdu_required_bytes(prev_buf_st);
  }

  if (lcid != LCID_SRB0 and ch_ctx.buf_st > 0) {
    // Allocation was not enough to empty the logical channel. In this specific case, we add some bytes to account
    // for the RLC segmentation overhead.
    // Note: This update is only relevant for PDSCH allocations for slots > slot_tx. For the case of PDSCH
    // slot==slot_tx, there will be an RLC Buffer Occupancy update right away, which will set a new buffer value.
    prev_buf_st = ch_ctx.buf_st;
    ch_ctx.buf_st += RLC_SEGMENTATION_OVERHEAD;
    if (ch_ctx.slice_id.has_value()) {
      auto& slice_pending_bytes = ue_ctx.find_slice(*ch_ctx.slice_id)->second;
      slice_pending_bytes += get_mac_sdu_required_bytes(ch_ctx.buf_st) - get_mac_sdu_required_bytes(prev_buf_st);
    }
  }

  subpdu.lcid        = lcid;
  subpdu.sched_bytes = sdu_size;

  return alloc_bytes;
}

unsigned
dl_logical_channel_system::allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& subpdu, unsigned rem_bytes, lcid_t lcid)
{
  subpdu.lcid        = lcid_dl_sch_t::MIN_RESERVED;
  subpdu.sched_bytes = 0;

  lcid_t lcid_with_prio = lcid == INVALID_LCID ? get_max_prio_lcid(get_ue(ue_rid)) : lcid;
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

  auto  u         = get_ue(ue_rid);
  auto& ue_ch_ctx = u.at<ue_channel_context>();
  if (not ue_ch_ctx.pending_ces.has_value()) {
    return 0;
  }
  soa::row_id         ce_row_id = *ue_ch_ctx.pending_ces;
  const auto&         ce_node   = pending_ces.at<mac_ce_context>(ce_row_id);
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
  ue_ch_ctx.pending_ces = ce_node.next_ue_ce;
  // Update sum of pending CE bytes.
  u.at<ue_context>().pending_ce_bytes -= alloc_bytes;
  pending_ces.erase(ce_row_id);

  return alloc_bytes;
}

unsigned
dl_logical_channel_system::allocate_ue_con_res_id_mac_ce(soa::row_id ue_rid, dl_msg_lc_info& subpdu, unsigned rem_bytes)
{
  auto& ue_ctx = get_ue(ue_rid).at<ue_context>();
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

// class logical_channel_system::ue_logical_channel_repository

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

void ue_dl_logical_channel_repository::reset_lcid_ran_slice(lcid_t lcid)
{
  parent->deregister_lc_ran_slice(ue_row_id, lcid);
}

unsigned ue_dl_logical_channel_repository::pending_bytes() const
{
  auto u = get_ue_row();
  if (u.at<ue_context>().fallback_state) {
    return pending_con_res_ce_bytes() + pending_bytes(LCID_SRB0) + pending_bytes(LCID_SRB1);
  }
  unsigned bytes = pending_ce_bytes();
  for (lcid_t lcid : u.at<ue_channel_context>().sorted_channels) {
    bytes += lcid != LCID_SRB0 ? pending_bytes(lcid) : 0;
  }
  return bytes;
}

double ue_dl_logical_channel_repository::average_bit_rate(lcid_t lcid) const
{
  auto  u      = get_ue_row();
  auto* ch_ctx = dl_logical_channel_system::find_active_ch_ctx(u, lcid);
  auto* qos_ch = (ch_ctx != nullptr and ch_ctx->qos_row.has_value())
                     ? &parent->qos_channels.row(ch_ctx->qos_row.value()).at<dl_logical_channel_system::qos_context>()
                     : nullptr;
  return qos_ch != nullptr ? qos_ch->avg_bytes_per_slot.average() * 8 * u.at<ue_config_context>().slots_per_msec * 1000
                           : 0.0;
}

void ue_dl_logical_channel_repository::handle_dl_buffer_status_indication(lcid_t     lcid,
                                                                          unsigned   buffer_status,
                                                                          slot_point hol_toa)
{
  parent->handle_dl_buffer_status_indication(ue_row_id, lcid, buffer_status, hol_toa);
}

bool ue_dl_logical_channel_repository::handle_mac_ce_indication(const mac_ce_info& ce)
{
  return parent->handle_mac_ce_indication(ue_row_id, ce);
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
