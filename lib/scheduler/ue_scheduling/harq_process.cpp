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

void harq_process::slot_indication(slot_point slot_rx)
{
  if (empty()) {
    return;
  }
  if (slot_rx < slot_ack) {
    // Wait more slots for ACK/NACK to arrive
    return;
  }
  if (tb[0].state == tb_t::state_t::waiting_ack) {
    // ACK went missing.
    tb[0].state = tb_t::state_t::pending_retx;
  }
  if (nof_retx() + 1 > max_nof_retx()) {
    // Max number of reTxs was exceeded. Clear HARQ process
    tb[0].state = tb_t::state_t::empty;
  }
}

int harq_process::ack_info(uint32_t tb_idx, bool ack)
{
  if (empty(tb_idx)) {
    return -1;
  }
  tb[tb_idx].ack_state = ack;
  if (ack) {
    tb[tb_idx].state = tb_t::state_t::empty;
  } else {
    tb[tb_idx].state = tb_t::state_t::pending_retx;
  }
  return ack ? tb[tb_idx].tbs : 0;
}

void harq_process::reset()
{
  tb[0].ack_state = false;
  tb[0].state     = tb_t::state_t::empty;
  tb[0].n_rtx     = 0;
  tb[0].mcs       = std::numeric_limits<uint32_t>::max();
  tb[0].tbs       = std::numeric_limits<uint32_t>::max();
}

bool harq_process::new_tx_common(slot_point       slot_tx_,
                                 slot_point       slot_ack_,
                                 const prb_grant& grant,
                                 uint32_t         mcs,
                                 uint32_t         max_retx_)
{
  if (not empty()) {
    return false;
  }
  reset();
  max_retx    = max_retx_;
  slot_tx     = slot_tx_;
  slot_ack    = slot_ack_;
  prbs_       = grant;
  tb[0].ndi   = !tb[0].ndi;
  tb[0].mcs   = mcs;
  tb[0].tbs   = 0;
  tb[0].state = tb_t::state_t::waiting_ack;
  return true;
}

bool harq_process::set_tbs(uint32_t tbs)
{
  if (empty() or nof_retx() > 0) {
    return false;
  }
  tb[0].tbs = tbs;
  return true;
}

bool harq_process::set_mcs(uint32_t mcs)
{
  if (empty() or nof_retx() > 0) {
    return false;
  }
  tb[0].mcs = mcs;
  return true;
}

bool harq_process::new_retx_common(slot_point slot_tx_, slot_point slot_ack_, const prb_grant& grant)
{
  if (grant.is_alloc_type0() != prbs_.is_alloc_type0() or
      (grant.is_alloc_type0() and grant.rbgs().count() != prbs_.rbgs().count()) or
      (grant.is_alloc_type1() and grant.prbs().length() != prbs_.prbs().length())) {
    return false;
  }
  if (new_retx_common(slot_tx_, slot_ack_)) {
    prbs_ = grant;
    return true;
  }
  return false;
}

bool harq_process::new_retx_common(slot_point slot_tx_, slot_point slot_ack_)
{
  if (tb[0].state != tb_t::state_t::pending_retx) {
    return false;
  }
  slot_tx         = slot_tx_;
  slot_ack        = slot_ack_;
  tb[0].state     = tb_t::state_t::waiting_ack;
  tb[0].ack_state = false;
  tb[0].n_rtx++;
  return true;
}

dl_harq_process::dl_harq_process(harq_id_t id_) : harq_process(id_) {}

bool dl_harq_process::new_tx(slot_point       slot_tx,
                             slot_point       slot_ack,
                             const prb_grant& grant,
                             uint32_t         mcs,
                             uint32_t         max_retx)
{
  return harq_process::new_tx_common(slot_tx, slot_ack, grant, mcs, max_retx);
}

bool dl_harq_process::new_retx(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant)
{
  return harq_process::new_retx_common(slot_tx, slot_ack, grant);
}

bool ul_harq_process::new_tx(slot_point slot_tx, const prb_grant& grant, uint32_t mcs_, uint32_t max_retx)
{
  return harq_process::new_tx_common(slot_tx, slot_tx, grant, mcs_, max_retx);
}

bool ul_harq_process::new_retx(slot_point slot_tx, const prb_grant& grant)
{
  return harq_process::new_retx_common(slot_tx, slot_tx, grant);
}

harq_entity::harq_entity(rnti_t rnti_, uint32_t nprb, uint32_t nof_harq_procs, srslog::basic_logger& logger_) :
  rnti(rnti_), logger(logger_)
{
  // Create HARQs
  dl_harqs.reserve(nof_harq_procs);
  ul_harqs.reserve(nof_harq_procs);
  for (uint32_t pid = 0; pid < nof_harq_procs; ++pid) {
    dl_harqs.emplace_back(to_harq_id(pid));
    ul_harqs.emplace_back(to_harq_id(pid));
  }
}

void harq_entity::slot_indication(slot_point slot_rx_)
{
  slot_rx = slot_rx_;
  for (harq_process& dl_h : dl_harqs) {
    bool was_empty = dl_h.empty();
    dl_h.slot_indication(slot_rx);
    if (not was_empty and dl_h.empty()) {
      // Toggle in HARQ state means that the HARQ was discarded
      logger.info("SCHED: discarding rnti=0x{:x}, DL TB pid={}. Cause: Maximum number of retx exceeded ({})",
                  rnti,
                  dl_h.pid,
                  dl_h.max_nof_retx());
    }
  }
  for (harq_process& ul_h : ul_harqs) {
    bool was_empty = ul_h.empty();
    ul_h.slot_indication(slot_rx);
    if (not was_empty and ul_h.empty()) {
      // Toggle in HARQ state means that the HARQ was discarded
      logger.info("SCHED: discarding rnti=0x{:x}, UL TB pid={}. Cause: Maximum number of retx exceeded ({})",
                  rnti,
                  ul_h.pid,
                  ul_h.max_nof_retx());
    }
  }
}

} // namespace srsgnb