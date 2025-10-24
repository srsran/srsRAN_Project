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

#include "cell_harq_manager.h"
#include "srsran/scheduler/resource_grid_util.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/scheduler/result/pusch_info.h"

using namespace srsran;
using namespace harq_utils;

namespace {

class noop_harq_timeout_notifier : public harq_timeout_notifier
{
public:
  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override
  { // do nothing
  }
};

template <bool IsDl>
class base_ntn_harq_history
{
  using harq_impl_type = std::conditional_t<IsDl, dl_harq_process_impl, ul_harq_process_impl>;

public:
  base_ntn_harq_history(cell_harq_repository<IsDl>& parent_, unsigned ntn_cs_koffset_) :
    harq_pool(parent_), ntn_cs_koffset(ntn_cs_koffset_)
  {
    srsran_assert(ntn_cs_koffset > 0 and ntn_cs_koffset <= NTN_CELL_SPECIFIC_KOFFSET_MAX, "Invalid NTN koffset");
    unsigned ring_size = get_allocator_ring_size_gt_min(ntn_cs_koffset * 2 + SCHEDULER_MAX_K1);
    history.resize(ring_size);
  }

  void slot_indication(slot_point sl_tx)
  {
    last_sl_ind = sl_tx;
    // If there are no allocations for a long time, we may need to reset last_sl_ack.
    if (last_sl_ack.valid() and last_sl_ack < sl_tx - ntn_cs_koffset - SCHEDULER_MAX_K1) {
      last_sl_ack.clear();
    }

    // Clear old entries.
    unsigned idx = get_current_index(sl_tx - ntn_cs_koffset - SCHEDULER_MAX_K1);
    history[idx].clear();
  }

  void save_harq_newtx_info(const harq_impl_type& h)
  {
    unsigned idx = get_current_index(h.slot_ack);
    history[idx].emplace_back(h);

    if (not last_sl_ack.valid() or h.slot_ack > last_sl_ack) {
      last_sl_ack = h.slot_ack;
    }
  }

  void handle_ack(harq_impl_type& h)
  {
    auto& hist_entry = history[get_current_index(h.slot_ack)];
    if (&h < hist_entry.data() or &h >= hist_entry.data() + hist_entry.size()) {
      // Check if the HARQ Process being ACKed is the same as the one managed by the NTN history.
      srsran_assertion_failure("ACK Info handled for a non-NTN HARQ process");
      srslog::fetch_basic_logger("SCHED").error("ACK Info handled for a non-NTN HARQ process");
      return;
    }
    h.status = harq_state_t::empty;
  }

protected:
  unsigned get_current_index(slot_point sl) const { return mod(sl.to_uint()); }
  unsigned mod(unsigned idx) const { return idx % history.size(); }

  cell_harq_repository<IsDl>&              harq_pool;
  unsigned                                 ntn_cs_koffset;
  std::vector<std::vector<harq_impl_type>> history;

  slot_point last_sl_ind;
  slot_point last_sl_ack;
};

} // namespace

namespace srsran::harq_utils {

/// Handles the DL HARQ information in the case NTN mode.
class ntn_dl_harq_alloc_history : public base_ntn_harq_history<true>
{
public:
  using base_ntn_harq_history<true>::base_ntn_harq_history;

  std::optional<dl_harq_process_handle> find_dl_harq(du_ue_index_t ue_idx, slot_point uci_slot, unsigned harq_bit_idx)
  {
    unsigned idx = get_current_index(uci_slot);
    for (dl_harq_process_impl& h_impl : history[idx]) {
      if (h_impl.ue_idx == ue_idx and h_impl.status == harq_state_t::waiting_ack and h_impl.slot_ack == uci_slot and
          h_impl.harq_bit_idx == harq_bit_idx) {
        return dl_harq_process_handle{harq_pool, h_impl};
      }
    }
    return std::nullopt;
  }
};

/// Handles the UL HARQ information in the case NTN mode.
class ntn_ul_harq_alloc_history : public base_ntn_harq_history<false>
{
public:
  using base_ntn_harq_history<false>::base_ntn_harq_history;

  std::optional<ul_harq_process_handle> find_ul_harq(du_ue_index_t ue_idx, slot_point pusch_slot)
  {
    unsigned idx = get_current_index(pusch_slot);
    for (ul_harq_process_impl& h_impl : history[idx]) {
      if (h_impl.ue_idx == ue_idx and h_impl.status == harq_state_t::waiting_ack and h_impl.slot_ack == pusch_slot) {
        return ul_harq_process_handle{harq_pool, h_impl};
      }
    }
    return std::nullopt;
  }

  unsigned sum_pending_ul_tbs(du_ue_index_t ue_idx) const
  {
    if (not last_sl_ack.valid() or last_sl_ack < last_sl_ind) {
      return 0;
    }
    unsigned sum     = 0;
    unsigned end_idx = get_current_index(last_sl_ack + 1);
    for (unsigned idx = get_current_index(last_sl_ind); idx != end_idx; idx = mod(idx + 1)) {
      for (const ul_harq_process_impl& h : history[idx]) {
        if (h.ue_idx == ue_idx and h.status != harq_state_t::empty) {
          sum += h.prev_tx_params.tbs_bytes;
          break;
        }
      }
    }
    return sum;
  }
};

} // namespace srsran::harq_utils

// In NTN case, we timeout the HARQ since we need to reuse the process before the PUSCH arrives.
static const unsigned NTN_ACK_WAIT_TIMEOUT = 1;

template <bool IsDl>
cell_harq_repository<IsDl>::cell_harq_repository(unsigned               max_ues,
                                                 unsigned               max_ack_wait_timeout,
                                                 unsigned               harq_retx_timeout_,
                                                 unsigned               max_harqs_per_ue_,
                                                 unsigned               ntn_cs_koffset_,
                                                 bool                   harq_mode_b,
                                                 harq_timeout_notifier& timeout_notifier_,
                                                 srslog::basic_logger&  logger_) :
  max_ack_wait_in_slots(ntn_cs_koffset_ > 0 and harq_mode_b ? NTN_ACK_WAIT_TIMEOUT : max_ack_wait_timeout),
  harq_retx_timeout(harq_retx_timeout_),
  max_harqs_per_ue(max_harqs_per_ue_),
  timeout_notifier(timeout_notifier_),
  logger(logger_),
  ntn_cs_koffset(ntn_cs_koffset_),
  alloc_hist(ntn_cs_koffset_ > 0 and harq_mode_b ? std::make_unique<harq_alloc_history>(*this, ntn_cs_koffset_)
                                                 : nullptr)
{
  // Reserve space in advance for UEs and their HARQs.
  ues.resize(max_ues);
  for (unsigned i = 0; i != max_ues; i++) {
    ues[i].free_harq_ids.reserve(max_harqs_per_ue);
    ues[i].harqs.reserve(max_harqs_per_ue);
  }

  harq_timeout_wheel.resize(
      get_allocator_ring_size_gt_min(max_ack_wait_timeout + get_max_slot_ul_alloc_delay(ntn_cs_koffset_)));
}

template <bool IsDl>
cell_harq_repository<IsDl>::~cell_harq_repository()
{
}

template <bool IsDl>
void cell_harq_repository<IsDl>::slot_indication(slot_point sl_tx)
{
  last_sl_ind = sl_tx;

  if (is_ntn_harq_mode_b_enabled()) {
    alloc_hist->slot_indication(sl_tx);
  }

  // Handle HARQs that timed out.
  auto& harqs_timing_out = harq_timeout_wheel[sl_tx.to_uint() % harq_timeout_wheel.size()];
  while (not harqs_timing_out.empty()) {
    handle_harq_ack_timeout(harqs_timing_out.front(), sl_tx);
  }

  // Handle HARQs with pending reTx that are trapped (never scheduled). This is a safety mechanism to account for a
  // potential bug or limitation in the scheduler policy that is leaving HARQ processes with pending reTxs for too long.
  // Note: we assume that the list is ordered based on when they were last ACKed.
  while (not harq_pending_retx_list.empty()) {
    auto& h = harq_pending_retx_list.front();
    srsran_sanity_check(h.status == harq_state_t::pending_retx, "HARQ process in wrong state");
    // [Implementation-defined] Maximum time we give to the scheduler policy to retransmit a HARQ process.
    const unsigned max_nof_slots_for_retx = last_sl_ind.nof_slots_per_hyper_system_frame() / 4;
    if (last_sl_ind < (h.slot_ack + max_nof_slots_for_retx)) {
      break;
    }

    // HARQ retransmission is trapped. Deallocate HARQ process.
    logger.warning("rnti={} h_id={}: Discarding {} HARQ. Cause: Too much time has passed since the last HARQ "
                   "transmission. The scheduler policy is likely not prioritizing retransmissions of old HARQ "
                   "processes. Parameters: dci={} rbs={} mcs={} nof_symbols={} nof_layers={}",
                   h.rnti,
                   fmt::underlying(h.h_id),
                   IsDl ? std::string_view{"DL"} : std::string_view{"UL"},
                   dci_format_to_string(get_dci_format(h.prev_tx_params.dci_cfg_type)),
                   h.prev_tx_params.rbs,
                   h.prev_tx_params.mcs,
                   h.prev_tx_params.nof_symbols,
                   h.prev_tx_params.nof_layers);
    dealloc_harq(h);

    // Report timeout after the HARQ gets deleted to avoid reentrancy.
    timeout_notifier.on_harq_timeout(h.ue_idx, IsDl, false);
  }
}

template <bool IsDl>
void cell_harq_repository<IsDl>::stop()
{
  for (auto& u : ues) {
    for (auto& h : u.harqs) {
      if (h.status != harq_state_t::empty) {
        dealloc_harq(h);
      }
    }
  }
  last_sl_ind = {};
}

template <bool IsDl>
void cell_harq_repository<IsDl>::handle_harq_ack_timeout(harq_type& h, slot_point sl_tx)
{
  srsran_sanity_check(h.status == harq_state_t::waiting_ack or h.status == harq_state_t::pending_retx,
                      "HARQ process in wrong state");

  if (is_ntn_harq_mode_b_enabled()) {
    // Deallocate HARQ.
    dealloc_harq(h);
    return;
  }

  bool ack_val = h.ack_on_timeout;
  if (h.status == harq_state_t::pending_retx) {
    // This HARQ is being starved by the scheduler.
    logger.info("rnti={} h_id={}: Discarding {} HARQ. Cause: The scheduler took too long to reschedule this HARQ "
                "process ({} slots elapsed since last NOK).",
                h.rnti,
                fmt::underlying(h.h_id),
                IsDl ? std::string_view{"DL"} : std::string_view{"UL"},
                h.slot_timeout - h.slot_ack);
  } else {
    if (ack_val) {
      // Case: Not all HARQ-ACKs were received, but at least one positive ACK was received.
      logger.debug("rnti={} h_id={}: Setting {} HARQ to \"ACKed\" state. Cause: Timeout was reached ({} slots), but "
                   "there are still missing PUCCH HARQ-ACK indications. However, one positive ACK was received.",
                   h.rnti,
                   fmt::underlying(h.h_id),
                   IsDl ? std::string_view{"DL"} : std::string_view{"UL"},
                   h.slot_timeout - h.slot_ack);
    } else {
      // At least one of the expected ACKs went missing and we haven't received any positive ACK.
      logger.warning("rnti={} h_id={}: Discarding {} HARQ. Cause: Timeout was reached ({} slots) to receive the "
                     "respective HARQ-ACK indication from lower layers (HARQ-ACK slot={})",
                     h.rnti,
                     fmt::underlying(h.h_id),
                     IsDl ? std::string_view{"DL"} : std::string_view{"UL"},
                     h.slot_timeout - h.slot_ack,
                     h.slot_ack);
    }
  }

  // Deallocate HARQ.
  dealloc_harq(h);

  // Report timeout with NACK after we delete the HARQ to avoid reentrancy.
  timeout_notifier.on_harq_timeout(h.ue_idx, IsDl, ack_val);
}

template <bool IsDl>
typename cell_harq_repository<IsDl>::harq_type* cell_harq_repository<IsDl>::alloc_harq(du_ue_index_t ue_idx,
                                                                                       slot_point    sl_tx,
                                                                                       slot_point    sl_ack,
                                                                                       unsigned      max_nof_harq_retxs)
{
  ue_harq_entity_impl& ue_harq_entity = ues[ue_idx];
  if (ue_harq_entity.free_harq_ids.empty()) {
    return nullptr;
  }

  // Allocation of free HARQ-id for the UE.
  const harq_id_t h_id = ue_harq_entity.free_harq_ids.back();
  ue_harq_entity.free_harq_ids.pop_back();
  harq_type& h = ue_harq_entity.harqs[h_id];
  srsran_sanity_check(h.ue_idx == ue_idx and h.h_id == h_id, "Invalid state of HARQ");

  // Set allocated HARQ common params.
  h.status             = harq_state_t::waiting_ack;
  h.slot_tx            = sl_tx;
  h.slot_ack           = sl_ack;
  h.nof_retxs          = 0;
  h.ndi                = !h.ndi;
  h.max_nof_harq_retxs = max_nof_harq_retxs;
  h.ack_on_timeout     = false;
  h.retxs_cancelled    = false;

  // Set UE HARQ entity common params.
  ue_harq_entity.last_slot_tx =
      ue_harq_entity.last_slot_tx.valid() ? std::max(ue_harq_entity.last_slot_tx, sl_tx) : sl_tx;
  ue_harq_entity.last_slot_ack =
      ue_harq_entity.last_slot_ack.valid() ? std::max(ue_harq_entity.last_slot_ack, sl_ack) : sl_ack;

  // Add HARQ to the timeout list.
  h.slot_timeout = sl_ack + max_ack_wait_in_slots - ntn_cs_koffset;
  harq_timeout_wheel[h.slot_timeout.to_uint() % harq_timeout_wheel.size()].push_front(&h);

  return &h;
}

template <bool IsDl>
void cell_harq_repository<IsDl>::dealloc_harq(harq_type& h)
{
  if (h.status == harq_state_t::empty) {
    // No-op
    return;
  }

  // Update HARQ process state.
  if (h.status == harq_state_t::pending_retx) {
    // Remove the HARQ from the pending Retx list.
    harq_pending_retx_list.pop(&h);
  }
  // Remove HARQ from the timeout list.
  harq_timeout_wheel[h.slot_timeout.to_uint() % harq_timeout_wheel.size()].pop(&h);
  h.slot_timeout = {};
  h.status       = harq_state_t::empty;

  // Check if common HARQ entity params need to be updated.
  ue_harq_entity_impl& ue_harq_entity = ues[h.ue_idx];
  if (ue_harq_entity.last_slot_tx.valid() and h.slot_tx >= ue_harq_entity.last_slot_tx) {
    // If the HARQ being reset corresponds to the last recorded Tx, we also reset "last_slot_tx". This avoids
    // encountering ambiguities with the slot wrap-around, when the UE stays for very long without being scheduled.
    ue_harq_entity.last_slot_tx = {};
  }
  if (ue_harq_entity.last_slot_ack.valid() and h.slot_ack >= ue_harq_entity.last_slot_ack) {
    // We do the same with last_slot_ack as we did with last_slot_tx.
    ue_harq_entity.last_slot_ack = {};
  }

  // Mark HARQ as available again.
  ue_harq_entity.free_harq_ids.push_back(h.h_id);
}

template <bool IsDl>
void cell_harq_repository<IsDl>::handle_ack(harq_type& h, bool ack)
{
  if (not ack and h.nof_retxs >= h.max_nof_harq_retxs) {
    if (h.retxs_cancelled) {
      logger.debug("rnti={} h_id={}: Discarding {} HARQ process TB with tbs={}. Cause: Retxs for this HARQ process "
                   "were cancelled",
                   h.rnti,
                   fmt::underlying(h.h_id),
                   IsDl ? std::string_view{"DL"} : std::string_view{"UL"},
                   h.prev_tx_params.tbs_bytes);
    } else {
      logger.info(
          "rnti={} h_id={}: Discarding {} HARQ process TB with tbs={}. Cause: Maximum number of reTxs {} exceeded",
          h.rnti,
          fmt::underlying(h.h_id),
          IsDl ? std::string_view{"DL"} : std::string_view{"UL"},
          h.prev_tx_params.tbs_bytes,
          h.max_nof_harq_retxs);
    }
  }

  if (is_ntn_harq_mode_b_enabled()) {
    // In NTN mode, ACK info does not affect the timeout/retx containers.
    alloc_hist->handle_ack(h);
    return;
  }

  if (ack or h.nof_retxs >= h.max_nof_harq_retxs) {
    // If the HARQ process is ACKed or the maximum number of retransmissions has been reached, we can deallocate the
    // HARQ process.
    dealloc_harq(h);
  } else {
    set_pending_retx(h);
  }
}

template <bool IsDl>
void cell_harq_repository<IsDl>::set_pending_retx(harq_type& h)
{
  srsran_sanity_check(h.status != harq_state_t::empty, "HARQ process in wrong state");
  if (h.status == harq_state_t::pending_retx) {
    // No-op
    return;
  }

  // Remove the HARQ from the ACK timeout list and re-add it with a new timeout.
  harq_timeout_wheel[h.slot_timeout.to_uint() % harq_timeout_wheel.size()].pop(&h);
  h.slot_timeout = last_sl_ind + harq_retx_timeout;
  harq_timeout_wheel[h.slot_timeout.to_uint() % harq_timeout_wheel.size()].push_front(&h);

  // Add HARQ to pending Retx list.
  harq_pending_retx_list.push_back(&h);

  // Update HARQ process state.
  h.status = harq_state_t::pending_retx;
}

template <bool IsDl>
bool cell_harq_repository<IsDl>::handle_new_retx(harq_type& h, slot_point sl_tx, slot_point sl_ack)
{
  if (h.status != harq_state_t::pending_retx) {
    logger.warning(
        "rnti={} h_id={}: Attempt of retx in a HARQ process that has no pending retx", h.rnti, fmt::underlying(h.h_id));
    return false;
  }

  // Remove HARQ from pending Retx list.
  harq_pending_retx_list.pop(&h);
  harq_timeout_wheel[h.slot_timeout.to_uint() % harq_timeout_wheel.size()].pop(&h);
  h.slot_timeout = {};

  // Update HARQ common parameters.
  h.status         = harq_state_t::waiting_ack;
  h.slot_tx        = sl_tx;
  h.slot_ack       = sl_ack;
  h.ack_on_timeout = false;
  h.nof_retxs++;

  // Set UE HARQ entity common params.
  ue_harq_entity_impl& ue_harq_entity = ues[h.ue_idx];
  ue_harq_entity.last_slot_tx =
      ue_harq_entity.last_slot_tx.valid() ? std::max(ue_harq_entity.last_slot_tx, sl_tx) : sl_tx;
  ue_harq_entity.last_slot_ack =
      ue_harq_entity.last_slot_ack.valid() ? std::max(ue_harq_entity.last_slot_ack, sl_ack) : sl_ack;

  // Add HARQ to the timeout list.
  h.slot_timeout = sl_ack + max_ack_wait_in_slots;
  harq_timeout_wheel[h.slot_timeout.to_uint() % harq_timeout_wheel.size()].push_front(&h);

  return true;
}

template <bool IsDl>
void cell_harq_repository<IsDl>::reserve_ue_harqs(du_ue_index_t ue_idx, rnti_t rnti, unsigned nof_harqs)
{
  ues[ue_idx].harqs.resize(nof_harqs);
  ues[ue_idx].free_harq_ids.resize(nof_harqs);
  for (unsigned count = 0; count != nof_harqs; ++count) {
    harq_id_t h_id                                   = to_harq_id(count);
    ues[ue_idx].free_harq_ids[nof_harqs - count - 1] = h_id; // add in reverse order.
    ues[ue_idx].harqs[h_id].ue_idx                   = ue_idx;
    ues[ue_idx].harqs[h_id].rnti                     = rnti;
    ues[ue_idx].harqs[h_id].h_id                     = h_id;
    ues[ue_idx].harqs[h_id].ndi                      = false;
  }
}

template <bool IsDl>
void cell_harq_repository<IsDl>::destroy_ue(du_ue_index_t ue_idx)
{
  // Remove HARQ from list of pending retxs or timeout wheel.
  for (harq_type& h : ues[ue_idx].harqs) {
    dealloc_harq(h);
  }
  ues[ue_idx].harqs.clear();
  ues[ue_idx].free_harq_ids.clear();
}

template <bool IsDl>
void cell_harq_repository<IsDl>::cancel_retxs(harq_type& h)
{
  if (h.status == harq_state_t::empty) {
    return;
  }
  if (h.status == harq_state_t::pending_retx) {
    // If a retx is pending, do not allow it to take place.
    dealloc_harq(h);
    return;
  }
  // If the HARQ is still waiting for an ACK to arrive, just mark it as max retxs have been exceeded. The ack_info
  // function will automatically update the HARQ state.
  h.max_nof_harq_retxs = h.nof_retxs;
  h.retxs_cancelled    = true;
}

template <bool IsDl>
const typename cell_harq_repository<IsDl>::harq_type*
cell_harq_repository<IsDl>::find_ue_harq_in_state(du_ue_index_t ue_idx, harq_utils::harq_state_t state) const
{
  for (const harq_type& h : ues[ue_idx].harqs) {
    if (h.status == state) {
      return &h;
    }
  }
  return nullptr;
}

template <bool IsDl>
bool cell_harq_repository<IsDl>::is_ntn_harq_mode_b_enabled() const
{
  return alloc_hist != nullptr;
}

template <bool IsDl>
typename cell_harq_repository<IsDl>::harq_type*
cell_harq_repository<IsDl>::find_ue_harq_in_state(du_ue_index_t ue_idx, harq_utils::harq_state_t state)
{
  for (harq_type& h : ues[ue_idx].harqs) {
    if (h.status == state) {
      return &h;
    }
  }
  return nullptr;
}

template struct harq_utils::cell_harq_repository<true>;
template struct harq_utils::cell_harq_repository<false>;

template <bool IsDl>
void harq_utils::base_harq_process_handle<IsDl>::cancel_retxs()
{
  harq_repo->cancel_retxs(*impl);
}

template <bool IsDl>
void harq_utils::base_harq_process_handle<IsDl>::reset()
{
  harq_repo->dealloc_harq(*impl);
}

template class harq_utils::base_harq_process_handle<true>;
template class harq_utils::base_harq_process_handle<false>;

// Cell HARQ manager.

cell_harq_manager::cell_harq_manager(unsigned                               max_ues,
                                     unsigned                               max_harqs_per_ue_,
                                     std::unique_ptr<harq_timeout_notifier> dl_notifier,
                                     std::unique_ptr<harq_timeout_notifier> ul_notifier,
                                     unsigned                               dl_harq_retx_timeout,
                                     unsigned                               ul_harq_retx_timeout,
                                     unsigned                               max_ack_wait_timeout,
                                     unsigned                               ntn_cs_koffset,
                                     bool                                   dl_harq_mode_b,
                                     bool                                   ul_harq_mode_b) :
  max_harqs_per_ue(max_harqs_per_ue_),
  dl_timeout_notifier(dl_notifier != nullptr and not dl_harq_mode_b ? std::move(dl_notifier)
                                                                    : std::make_unique<noop_harq_timeout_notifier>()),
  ul_timeout_notifier(not ul_harq_mode_b ? std::move(ul_notifier) : std::make_unique<noop_harq_timeout_notifier>()),
  logger(srslog::fetch_basic_logger("SCHED")),
  dl(max_ues,
     max_ack_wait_timeout,
     dl_harq_retx_timeout,
     max_harqs_per_ue,
     ntn_cs_koffset,
     dl_harq_mode_b,
     *dl_timeout_notifier,
     logger),
  ul(max_ues,
     max_ack_wait_timeout,
     ul_harq_retx_timeout,
     max_harqs_per_ue,
     ntn_cs_koffset,
     ul_harq_mode_b,
     ul_timeout_notifier != nullptr ? *ul_timeout_notifier : *dl_timeout_notifier,
     logger)
{
}

void cell_harq_manager::slot_indication(slot_point sl_tx)
{
  dl.slot_indication(sl_tx);
  ul.slot_indication(sl_tx);
}

void cell_harq_manager::stop()
{
  dl.stop();
  ul.stop();
}

bool cell_harq_manager::contains(du_ue_index_t ue_idx) const
{
  return ue_idx < dl.ues.size() and not dl.ues[ue_idx].harqs.empty();
}

dl_harq_pending_retx_list cell_harq_manager::pending_dl_retxs()
{
  return dl_harq_pending_retx_list{dl};
}

ul_harq_pending_retx_list cell_harq_manager::pending_ul_retxs()
{
  return ul_harq_pending_retx_list{ul};
}

unique_ue_harq_entity
cell_harq_manager::add_ue(du_ue_index_t ue_idx, rnti_t crnti, unsigned nof_dl_harq_procs, unsigned nof_ul_harq_procs)
{
  srsran_assert(nof_dl_harq_procs <= max_harqs_per_ue and nof_dl_harq_procs > 0, "Invalid number of DL HARQs");
  srsran_assert(nof_ul_harq_procs <= max_harqs_per_ue and nof_ul_harq_procs > 0, "Invalid number of DL HARQs");
  srsran_assert(ue_idx < dl.ues.size(), "Invalid ue_index");
  srsran_assert(not contains(ue_idx), "Creating UE with duplicate ue_index");
  dl.reserve_ue_harqs(ue_idx, crnti, nof_dl_harq_procs);
  ul.reserve_ue_harqs(ue_idx, crnti, nof_ul_harq_procs);
  return {this, ue_idx, crnti};
}

void cell_harq_manager::destroy_ue(du_ue_index_t ue_idx)
{
  dl.destroy_ue(ue_idx);
  ul.destroy_ue(ue_idx);
}

harq_utils::dl_harq_process_impl* cell_harq_manager::new_dl_tx(du_ue_index_t ue_idx,
                                                               rnti_t        rnti,
                                                               slot_point    pdsch_slot,
                                                               unsigned      ack_delay,
                                                               unsigned      max_harq_nof_retxs,
                                                               uint8_t       harq_bit_idx)
{
  dl_harq_process_impl* h = dl.alloc_harq(ue_idx, pdsch_slot, pdsch_slot + ack_delay, max_harq_nof_retxs);
  if (h == nullptr) {
    return nullptr;
  }

  // Save DL-specific parameters.
  h->prev_tx_params       = {};
  h->harq_bit_idx         = harq_bit_idx;
  h->pucch_ack_to_receive = 0;
  h->chosen_ack           = mac_harq_ack_report_status::dtx;
  h->last_pucch_snr       = std::nullopt;

  return h;
}

harq_utils::ul_harq_process_impl*
cell_harq_manager::new_ul_tx(du_ue_index_t ue_idx, rnti_t rnti, slot_point pusch_slot, unsigned max_harq_nof_retxs)
{
  ul_harq_process_impl* h = ul.alloc_harq(ue_idx, pusch_slot, pusch_slot, max_harq_nof_retxs);
  if (h == nullptr) {
    return nullptr;
  }

  // Save UL-specific parameters.
  h->prev_tx_params = {};

  return h;
}

bool dl_harq_process_handle::new_retx(slot_point pdsch_slot, unsigned ack_delay, uint8_t harq_bit_idx)
{
  if (not harq_repo->handle_new_retx(*impl, pdsch_slot, pdsch_slot + ack_delay)) {
    return false;
  }
  // Reset DL-only HARQ parameters.
  impl->harq_bit_idx         = harq_bit_idx;
  impl->pucch_ack_to_receive = 0;
  impl->chosen_ack           = mac_harq_ack_report_status::dtx;
  impl->last_pucch_snr       = std::nullopt;
  return true;
}

dl_harq_process_handle::status_update dl_harq_process_handle::dl_ack_info(mac_harq_ack_report_status ack,
                                                                          std::optional<float>       pucch_snr)
{
  if (impl->status != harq_state_t::waiting_ack) {
    // If the HARQ process is not expecting an HARQ-ACK, it means that it has already been ACKed/NACKed.
    harq_repo->logger.warning(
        "rnti={} h_id={}: ACK arrived for inactive DL HARQ", impl->rnti, fmt::underlying(impl->h_id));
    return status_update::error;
  }

  if (ack != mac_harq_ack_report_status::dtx and
      (not impl->last_pucch_snr.has_value() or
       (pucch_snr.has_value() and impl->last_pucch_snr.value() < pucch_snr.value()))) {
    // Case: If there was no previous HARQ-ACK decoded or the previous HARQ-ACK had lower SNR, this HARQ-ACK is chosen.
    impl->chosen_ack     = ack;
    impl->last_pucch_snr = pucch_snr;
  }

  if (impl->pucch_ack_to_receive <= 1) {
    // Case: This is the last HARQ-ACK that is expected for this HARQ process.

    // Update HARQ state
    bool final_ack = impl->chosen_ack == mac_harq_ack_report_status::ack;
    harq_repo->handle_ack(*impl, final_ack);

    return final_ack ? status_update::acked : status_update::nacked;
  }

  // Case: This is not the last PUCCH HARQ-ACK that is expected for this HARQ process.
  impl->pucch_ack_to_receive--;
  impl->ack_on_timeout = impl->chosen_ack == mac_harq_ack_report_status::ack;

  if (harq_repo->is_ntn_harq_mode_b_enabled()) {
    // Timeouts don't need to be updated in NTN mode.
    return status_update::no_update;
  }

  // We reduce the HARQ process timeout to receive the next HARQ-ACK. This is done because the two HARQ-ACKs should
  // arrive almost simultaneously, and in case the second goes missing, we don't want to block the HARQ for too long.
  auto& wheel = harq_repo->harq_timeout_wheel;
  wheel[impl->slot_timeout.to_uint() % wheel.size()].pop(impl);
  impl->slot_timeout = harq_repo->last_sl_ind + SHORT_ACK_TIMEOUT_DTX;
  wheel[impl->slot_timeout.to_uint() % wheel.size()].push_front(impl);

  return status_update::no_update;
}

void dl_harq_process_handle::increment_pucch_counter()
{
  ++impl->pucch_ack_to_receive;
}

void dl_harq_process_handle::save_grant_params(const dl_harq_alloc_context& ctx, const dl_msg_alloc& ue_pdsch)
{
  static constexpr size_t CW_INDEX = 0;

  const pdsch_information& pdsch = ue_pdsch.pdsch_cfg;
  srsran_assert(pdsch.codewords.size() == 1, "Only one codeword supported");
  srsran_sanity_check(pdsch.rnti == impl->rnti, "RNTI mismatch");
  srsran_sanity_check(pdsch.harq_id == impl->h_id, "HARQ-id mismatch");
  srsran_assert(impl->status == harq_utils::harq_state_t::waiting_ack,
                "Setting allocation parameters for DL HARQ process id={} in invalid state",
                fmt::underlying(id()));

  const pdsch_codeword&               cw          = pdsch.codewords[CW_INDEX];
  dl_harq_process_impl::alloc_params& prev_params = impl->prev_tx_params;

  if (impl->nof_retxs == 0) {
    prev_params.tbs_bytes    = cw.tb_size_bytes;
    prev_params.dci_cfg_type = ctx.dci_cfg_type;
    prev_params.nof_layers   = pdsch.nof_layers;
    prev_params.olla_mcs     = ctx.olla_mcs;
    prev_params.slice_id     = ctx.slice_id;
    prev_params.cqi          = ctx.cqi.has_value() ? ctx.cqi.value() : cqi_value{1};
    prev_params.is_fallback  = ctx.is_fallback;
    prev_params.lc_sched_info.clear();
    for (const dl_msg_lc_info& lc : ue_pdsch.tb_list[CW_INDEX].lc_chs_to_sched) {
      prev_params.lc_sched_info.push_back({lc.lcid, units::bytes{lc.sched_bytes}});
    }
  } else {
    srsran_assert(ctx.dci_cfg_type == prev_params.dci_cfg_type,
                  "DCI format and RNTI type cannot change during DL HARQ retxs");
    srsran_assert(prev_params.tbs_bytes == cw.tb_size_bytes,
                  "TBS cannot change during DL HARQ retxs ({}!={}). Previous MCS={}, RBs={}. New MCS={}, RBs={}",
                  prev_params.tbs_bytes,
                  cw.tb_size_bytes,
                  prev_params.mcs,
                  prev_params.rbs,
                  cw.mcs_index,
                  pdsch.rbs);
    srsran_assert(prev_params.nof_layers == pdsch.nof_layers, "Number of layers cannot change during HARQ retxs");
    srsran_assert(prev_params.is_fallback == ctx.is_fallback, "Fallback state cannot change across DL HARQ retxs");
  }
  prev_params.mcs_table   = cw.mcs_table;
  prev_params.mcs         = cw.mcs_index;
  prev_params.rbs         = pdsch.rbs;
  prev_params.nof_symbols = pdsch.symbols.length();

  if (harq_repo->is_ntn_harq_mode_b_enabled()) {
    // In NTN mode, save the HARQ info in history.
    harq_repo->alloc_hist->save_harq_newtx_info(*impl);
  }
}

bool ul_harq_process_handle::new_retx(slot_point pusch_slot)
{
  return harq_repo->handle_new_retx(*impl, pusch_slot, pusch_slot);
}

int ul_harq_process_handle::ul_crc_info(bool ack)
{
  if (impl->status != harq_state_t::waiting_ack) {
    // HARQ is not expecting CRC info.
    harq_repo->logger.warning("rnti={} h_id={}: Discarding CRC. Cause: UL HARQ process is not expecting any CRC",
                              impl->rnti,
                              fmt::underlying(impl->h_id));
    return -1;
  }

  harq_repo->handle_ack(*impl, ack);

  return ack ? (int)impl->prev_tx_params.tbs_bytes : 0;
}

void ul_harq_process_handle::save_grant_params(const ul_harq_alloc_context& ctx, const pusch_information& pusch)
{
  srsran_sanity_check(pusch.rnti == impl->rnti, "RNTI mismatch");
  srsran_sanity_check(pusch.harq_id == impl->h_id, "HARQ-id mismatch");
  srsran_assert(impl->status == harq_utils::harq_state_t::waiting_ack,
                "Setting allocation parameters for DL HARQ process id={} in invalid state",
                fmt::underlying(id()));

  ul_harq_process_impl::alloc_params& prev_tx_params = impl->prev_tx_params;

  if (impl->nof_retxs == 0) {
    prev_tx_params.dci_cfg_type = ctx.dci_cfg_type;
    prev_tx_params.olla_mcs     = ctx.olla_mcs;
    prev_tx_params.tbs_bytes    = pusch.tb_size_bytes;
    prev_tx_params.slice_id     = ctx.slice_id;
  } else {
    srsran_assert(ctx.dci_cfg_type == prev_tx_params.dci_cfg_type,
                  "DCI format and RNTI type cannot change during HARQ retxs");
    srsran_assert(prev_tx_params.tbs_bytes == pusch.tb_size_bytes, "TBS cannot change during HARQ retxs");
  }
  prev_tx_params.mcs_table   = pusch.mcs_table;
  prev_tx_params.mcs         = pusch.mcs_index;
  prev_tx_params.rbs         = pusch.rbs;
  prev_tx_params.nof_symbols = pusch.symbols.length();
  prev_tx_params.nof_layers  = pusch.nof_layers;

  if (harq_repo->is_ntn_harq_mode_b_enabled()) {
    // In NTN mode, save the HARQ info in history.
    harq_repo->alloc_hist->save_harq_newtx_info(*impl);
  }
}

// UE HARQ entity.

unique_ue_harq_entity::unique_ue_harq_entity(cell_harq_manager* mgr, du_ue_index_t ue_idx, rnti_t crnti_) :
  cell_harq_mgr(mgr), ue_index(ue_idx), crnti(crnti_)
{
}

unique_ue_harq_entity::unique_ue_harq_entity(unique_ue_harq_entity&& other) noexcept :
  cell_harq_mgr(other.cell_harq_mgr), ue_index(other.ue_index), crnti(other.crnti)
{
  other.cell_harq_mgr = nullptr;
  other.ue_index      = INVALID_DU_UE_INDEX;
  other.crnti         = rnti_t::INVALID_RNTI;
}

unique_ue_harq_entity& unique_ue_harq_entity::operator=(unique_ue_harq_entity&& other) noexcept
{
  if (cell_harq_mgr != nullptr) {
    cell_harq_mgr->destroy_ue(ue_index);
  }
  cell_harq_mgr       = other.cell_harq_mgr;
  ue_index            = other.ue_index;
  crnti               = other.crnti;
  other.cell_harq_mgr = nullptr;
  other.ue_index      = INVALID_DU_UE_INDEX;
  other.crnti         = rnti_t::INVALID_RNTI;
  return *this;
}

unique_ue_harq_entity::~unique_ue_harq_entity()
{
  if (cell_harq_mgr != nullptr) {
    cell_harq_mgr->destroy_ue(ue_index);
  }
}

void unique_ue_harq_entity::reset()
{
  if (cell_harq_mgr != nullptr) {
    cell_harq_mgr->destroy_ue(ue_index);
    cell_harq_mgr = nullptr;
  }
}

void unique_ue_harq_entity::cancel_retxs()
{
  for (auto& h_dl : get_dl_ue().harqs) {
    if (h_dl.status != harq_state_t::empty) {
      dl_harq_process_handle{cell_harq_mgr->dl, h_dl}.cancel_retxs();
    }
  }

  for (auto& h_ul : get_ul_ue().harqs) {
    if (h_ul.status != harq_state_t::empty) {
      ul_harq_process_handle{cell_harq_mgr->ul, h_ul}.cancel_retxs();
    }
  }
}

std::optional<ul_harq_process_handle> unique_ue_harq_entity::ul_harq(harq_id_t h_id, slot_point slot)
{
  if (cell_harq_mgr->ul.is_ntn_harq_mode_b_enabled()) {
    return cell_harq_mgr->ul.alloc_hist->find_ul_harq(ue_index, slot);
  }
  return ul_harq(h_id);
}

std::optional<const ul_harq_process_handle> unique_ue_harq_entity::ul_harq(harq_id_t h_id, slot_point slot) const
{
  if (cell_harq_mgr->ul.is_ntn_harq_mode_b_enabled()) {
    return cell_harq_mgr->ul.alloc_hist->find_ul_harq(ue_index, slot);
  }
  return ul_harq(h_id);
}

std::optional<dl_harq_process_handle> unique_ue_harq_entity::alloc_dl_harq(slot_point sl_tx,
                                                                           unsigned   ack_delay,
                                                                           unsigned   max_harq_nof_retxs,
                                                                           unsigned   harq_bit_idx)
{
  dl_harq_process_impl* h =
      cell_harq_mgr->new_dl_tx(ue_index, crnti, sl_tx, ack_delay, max_harq_nof_retxs, harq_bit_idx);
  if (h == nullptr) {
    return std::nullopt;
  }
  return dl_harq_process_handle(cell_harq_mgr->dl, *h);
}

std::optional<ul_harq_process_handle> unique_ue_harq_entity::alloc_ul_harq(slot_point sl_tx,
                                                                           unsigned   max_harq_nof_retxs)
{
  ul_harq_process_impl* h = cell_harq_mgr->new_ul_tx(ue_index, crnti, sl_tx, max_harq_nof_retxs);
  if (h == nullptr) {
    return std::nullopt;
  }
  return ul_harq_process_handle(cell_harq_mgr->ul, *h);
}

std::optional<dl_harq_process_handle> unique_ue_harq_entity::find_pending_dl_retx()
{
  dl_harq_process_impl* h = cell_harq_mgr->dl.find_ue_harq_in_state(ue_index, harq_state_t::pending_retx);
  if (h == nullptr) {
    return std::nullopt;
  }
  return dl_harq_process_handle(cell_harq_mgr->dl, *h);
}

std::optional<const dl_harq_process_handle> unique_ue_harq_entity::find_pending_dl_retx() const
{
  dl_harq_process_impl* h = cell_harq_mgr->dl.find_ue_harq_in_state(ue_index, harq_state_t::pending_retx);
  if (h == nullptr) {
    return std::nullopt;
  }
  return dl_harq_process_handle(cell_harq_mgr->dl, *h);
}

std::optional<ul_harq_process_handle> unique_ue_harq_entity::find_pending_ul_retx()
{
  ul_harq_process_impl* h = cell_harq_mgr->ul.find_ue_harq_in_state(ue_index, harq_state_t::pending_retx);
  if (h == nullptr) {
    return std::nullopt;
  }
  return ul_harq_process_handle(cell_harq_mgr->ul, *h);
}

std::optional<const ul_harq_process_handle> unique_ue_harq_entity::find_pending_ul_retx() const
{
  ul_harq_process_impl* h = cell_harq_mgr->ul.find_ue_harq_in_state(ue_index, harq_state_t::pending_retx);
  if (h == nullptr) {
    return std::nullopt;
  }
  return ul_harq_process_handle(cell_harq_mgr->ul, *h);
}

std::optional<dl_harq_process_handle> unique_ue_harq_entity::find_dl_harq_waiting_ack()
{
  dl_harq_process_impl* h = cell_harq_mgr->dl.find_ue_harq_in_state(ue_index, harq_state_t::waiting_ack);
  if (h == nullptr) {
    return std::nullopt;
  }
  return dl_harq_process_handle(cell_harq_mgr->dl, *h);
}

std::optional<const dl_harq_process_handle> unique_ue_harq_entity::find_dl_harq_waiting_ack() const
{
  dl_harq_process_impl* h = cell_harq_mgr->dl.find_ue_harq_in_state(ue_index, harq_state_t::waiting_ack);
  if (h == nullptr) {
    return std::nullopt;
  }
  return dl_harq_process_handle(cell_harq_mgr->dl, *h);
}

std::optional<ul_harq_process_handle> unique_ue_harq_entity::find_ul_harq_waiting_ack()
{
  ul_harq_process_impl* h = cell_harq_mgr->ul.find_ue_harq_in_state(ue_index, harq_state_t::waiting_ack);
  if (h == nullptr) {
    return std::nullopt;
  }
  return ul_harq_process_handle(cell_harq_mgr->ul, *h);
}

std::optional<dl_harq_process_handle> unique_ue_harq_entity::find_dl_harq_waiting_ack(slot_point uci_slot,
                                                                                      uint8_t    harq_bit_idx)
{
  if (cell_harq_mgr->dl.is_ntn_harq_mode_b_enabled()) {
    // NTN mode.
    return cell_harq_mgr->dl.alloc_hist->find_dl_harq(ue_index, uci_slot, harq_bit_idx);
  }

  std::vector<dl_harq_process_impl>& dl_harqs = cell_harq_mgr->dl.ues[ue_index].harqs;
  for (dl_harq_process_impl& h : dl_harqs) {
    if (h.status == harq_utils::harq_state_t::waiting_ack and h.slot_ack == uci_slot and
        h.harq_bit_idx == harq_bit_idx) {
      return dl_harq_process_handle(cell_harq_mgr->dl, h);
    }
  }
  return std::nullopt;
}

std::optional<ul_harq_process_handle> unique_ue_harq_entity::find_ul_harq_waiting_ack(slot_point pusch_slot)
{
  if (cell_harq_mgr->ul.is_ntn_harq_mode_b_enabled()) {
    // NTN mode.
    return cell_harq_mgr->ul.alloc_hist->find_ul_harq(ue_index, pusch_slot);
  }

  std::vector<ul_harq_process_impl>& ul_harqs = cell_harq_mgr->ul.ues[ue_index].harqs;
  for (ul_harq_process_impl& h : ul_harqs) {
    if (h.status == harq_utils::harq_state_t::waiting_ack and h.slot_tx == pusch_slot) {
      return ul_harq_process_handle(cell_harq_mgr->ul, h);
    }
  }
  return std::nullopt;
}

void unique_ue_harq_entity::uci_sched_failed(slot_point uci_slot)
{
  std::vector<dl_harq_process_impl>& dl_harqs = cell_harq_mgr->dl.ues[ue_index].harqs;
  for (dl_harq_process_impl& h : dl_harqs) {
    if (h.status == harq_utils::harq_state_t::waiting_ack and h.slot_ack == uci_slot) {
      unsigned nof_nacks = std::max(h.pucch_ack_to_receive, 1U);
      auto     h_handle  = dl_harq_process_handle(cell_harq_mgr->dl, h);
      for (unsigned i = 0; i != nof_nacks; ++i) {
        h_handle.dl_ack_info(mac_harq_ack_report_status::dtx, std::nullopt);
      }
    }
  }
}

unsigned unique_ue_harq_entity::total_ul_bytes_waiting_ack() const
{
  if (cell_harq_mgr->ul.is_ntn_harq_mode_b_enabled()) {
    return cell_harq_mgr->ul.alloc_hist->sum_pending_ul_tbs(ue_index);
  }

  unsigned harq_bytes = 0;
  for (unsigned i = 0, e = nof_ul_harqs(); i != e; ++i) {
    if (get_ul_ue().harqs[i].status != harq_utils::harq_state_t::empty) {
      harq_bytes += get_ul_ue().harqs[i].prev_tx_params.tbs_bytes;
    }
  }

  return harq_bytes;
}
