/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "harq_process.h"

namespace srsgnb {

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::slot_indication(slot_point slot_tx)
{
  for (transport_block& tb : tb_array) {
    if (tb.state == transport_block::state_t::empty) {
      continue;
    }
    if (tb.slot_ack + max_ack_wait_in_slots > slot_tx) {
      // Wait more slots for ACK/NACK to arrive.
      return;
    }
    if (tb.state == transport_block::state_t::waiting_ack) {
      // ACK went missing.
      tb.state = transport_block::state_t::pending_retx;
    }
    if (tb.nof_retxs + 1 > max_nof_retxs()) {
      // Max number of reTxs was exceeded. Clear HARQ process
      tb.state = transport_block::state_t::empty;
    }
  }
}

template <bool IsDownlink>
int detail::harq_process<IsDownlink>::ack_info(unsigned tb_idx, bool ack)
{
  if (empty(tb_idx)) {
    return -1;
  }
  tb_array[tb_idx].ack_state = ack;
  if (ack) {
    tb_array[tb_idx].state = transport_block::state_t::empty;
  } else {
    tb_array[tb_idx].state = (tb_array[tb_idx].nof_retxs < max_nof_retxs()) ? transport_block::state_t::pending_retx
                                                                            : transport_block::state_t::empty;
  }
  return ack ? tb_array[tb_idx].grant.tbs_bytes : 0;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::reset(unsigned tb_idx)
{
  tb_array[tb_idx].ack_state = false;
  tb_array[tb_idx].state     = transport_block::state_t::empty;
  tb_array[tb_idx].nof_retxs = 0;
  tb_array[tb_idx].grant     = {};
}

template <bool IsDownlink>
auto detail::harq_process<IsDownlink>::new_tx_common(unsigned   tb_idx,
                                                     slot_point slot_tx_,
                                                     slot_point slot_ack_,
                                                     unsigned   max_harq_retxs_) -> grant_params*
{
  if (not empty(tb_idx)) {
    return nullptr;
  }
  reset(tb_idx);
  max_retx                  = max_harq_retxs_;
  tb_array[tb_idx].slot_tx  = slot_tx_;
  tb_array[tb_idx].slot_ack = slot_ack_;
  tb_array[tb_idx].ndi      = !tb_array[tb_idx].ndi;
  tb_array[tb_idx].state    = transport_block::state_t::waiting_ack;
  return &tb_array[tb_idx].grant;
}

template <bool IsDownlink>
auto detail::harq_process<IsDownlink>::new_retx_common(unsigned tb_idx, slot_point slot_tx_, slot_point slot_ack_)
    -> grant_params*
{
  if (tb_array[tb_idx].state != transport_block::state_t::pending_retx) {
    return nullptr;
  }
  tb_array[tb_idx].slot_tx   = slot_tx_;
  tb_array[tb_idx].slot_ack  = slot_ack_;
  tb_array[tb_idx].state     = transport_block::state_t::waiting_ack;
  tb_array[tb_idx].ack_state = false;
  tb_array[tb_idx].nof_retxs++;
  return &tb_array[tb_idx].grant;
}

// Explicit template instantiation.
template class detail::harq_process<true>;
template class detail::harq_process<false>;

dl_harq_info_params*
dl_harq_process::new_tx(unsigned tb_idx, slot_point pdsch_slot, unsigned k1, unsigned max_harq_retxs)
{
  return harq_process::new_tx_common(tb_idx, pdsch_slot, pdsch_slot + k1, max_harq_retxs);
}

dl_harq_info_params* dl_harq_process::new_retx(unsigned tb_idx, slot_point pdsch_slot, unsigned k1)
{
  return harq_process::new_retx_common(tb_idx, pdsch_slot, pdsch_slot + k1);
}

ul_harq_info_params* ul_harq_process::new_tx(slot_point pusch_slot, unsigned max_harq_retxs)
{
  return harq_process::new_tx_common(0, pusch_slot, pusch_slot, max_harq_retxs);
}

ul_harq_info_params* ul_harq_process::new_retx(slot_point pusch_slot)
{
  return harq_process::new_retx_common(0, pusch_slot, pusch_slot);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

harq_entity::harq_entity(rnti_t rnti_, unsigned nof_harq_procs, unsigned max_ack_wait_in_slots) :
  rnti(rnti_), logger(srslog::fetch_basic_logger("MAC"))
{
  // Create HARQs
  dl_harqs.reserve(nof_harq_procs);
  ul_harqs.reserve(nof_harq_procs);
  for (unsigned pid = 0; pid < nof_harq_procs; ++pid) {
    dl_harqs.emplace_back(to_harq_id(pid), max_ack_wait_in_slots);
    ul_harqs.emplace_back(to_harq_id(pid), max_ack_wait_in_slots);
  }
}

void harq_entity::slot_indication(slot_point slot_tx_)
{
  slot_tx = slot_tx_;
  for (dl_harq_process& dl_h : dl_harqs) {
    bool was_empty = dl_h.empty();
    dl_h.slot_indication(slot_tx);
    if (not was_empty and dl_h.empty()) {
      // Toggle in HARQ state means that the HARQ was discarded
      logger.info("SCHED: discarding rnti=0x{:x}, DL TB pid={}. Cause: Maximum number of retx exceeded ({})",
                  rnti,
                  dl_h.id,
                  dl_h.max_nof_retxs());
    }
  }
  for (ul_harq_process& ul_h : ul_harqs) {
    bool was_empty = ul_h.empty();
    ul_h.slot_indication(slot_tx);
    if (not was_empty and ul_h.empty()) {
      // Toggle in HARQ state means that the HARQ was discarded
      logger.info("SCHED: discarding rnti=0x{:x}, UL TB pid={}. Cause: Maximum number of retx exceeded ({})",
                  rnti,
                  ul_h.id,
                  ul_h.max_nof_retxs());
    }
  }
}

} // namespace srsgnb