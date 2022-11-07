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
    if (last_slot_ack + max_ack_wait_in_slots > slot_tx) {
      // Wait more slots for ACK/NACK to arrive.
      return;
    }
    if (tb.state == transport_block::state_t::waiting_ack) {
      // ACK went missing.
      tb.state = transport_block::state_t::pending_retx;
    }
    if (tb.nof_retxs + 1 > tb.max_nof_harq_retxs) {
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
    tb_array[tb_idx].state = (tb_array[tb_idx].nof_retxs < tb_array[tb_idx].max_nof_harq_retxs)
                                 ? transport_block::state_t::pending_retx
                                 : transport_block::state_t::empty;
  }
  return ack ? tb_array[tb_idx].tbs_bytes : 0;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::reset()
{
  for (unsigned i = 0; i != tb_array.size(); ++i) {
    reset_tb(i);
  }
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::reset_tb(unsigned tb_idx)
{
  tb_array[tb_idx].ack_state = false;
  tb_array[tb_idx].state     = transport_block::state_t::empty;
  tb_array[tb_idx].nof_retxs = 0;
  tb_array[tb_idx].mcs       = 0;
  tb_array[tb_idx].tbs_bytes = 0;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::tx_common(dci_rnti_config_type               dci_type_,
                                                 bwp_id_t                           bwp_id_,
                                                 slot_point                         slot_tx_,
                                                 slot_point                         slot_ack_,
                                                 const prb_grant&                   prbs,
                                                 const optional<harq_tb_tx_params>& tb1_params,
                                                 const optional<harq_tb_tx_params>& tb2_params)
{
  srsgnb_assert(tb1_params.has_value() or tb2_params.has_value(), "The TB params must not be empty");
  dci_type      = dci_type_;
  bwp_id        = bwp_id_;
  last_slot_tx  = slot_tx_;
  last_slot_ack = slot_ack_;
  last_prbs     = prbs;
  if (tb1_params.has_value()) {
    tx_tb_common(0, *tb1_params);
  }
  if (tb2_params.has_value()) {
    tx_tb_common(1, *tb2_params);
  }
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::tx_tb_common(unsigned tb_idx, const harq_tb_tx_params& tb_params)
{
  if (tb_params.is_newtx) {
    srsgnb_assert(empty(tb_idx), "Cannot allocate newTx non-empty HARQ TB");
    srsgnb_assert(tb_params.tbs_bytes > 0, "The TB size cannot be zero");
    tb_array[tb_idx].state              = transport_block::state_t::waiting_ack;
    tb_array[tb_idx].ndi                = !tb_array[tb_idx].ndi;
    tb_array[tb_idx].max_nof_harq_retxs = tb_params.max_harq_retxs;
    tb_array[tb_idx].mcs                = tb_params.mcs;
    tb_array[tb_idx].tbs_bytes          = tb_params.tbs_bytes;
    tb_array[tb_idx].ack_state          = false;
    tb_array[tb_idx].nof_retxs          = 0;
  } else {
    srsgnb_assert(tb_array[tb_idx].state == transport_block::state_t::pending_retx,
                  "Cannot allocate reTx in HARQ without a pending reTx");
    tb_array[tb_idx].state     = transport_block::state_t::waiting_ack;
    tb_array[tb_idx].mcs       = tb_params.mcs;
    tb_array[tb_idx].ack_state = false;
    tb_array[tb_idx].nof_retxs++;
  }
}

// Explicit template instantiation.
template class detail::harq_process<true>;
template class detail::harq_process<false>;

void dl_harq_process::new_tx(dci_dl_rnti_config_type dci_cfg_type,
                             bwp_id_t                bwp_id,
                             slot_point              pdsch_slot,
                             unsigned                k1,
                             const prb_grant&        prbs,
                             sch_mcs_index           mcs,
                             unsigned                tbs_bytes,
                             unsigned                max_harq_nof_retxs)
{
  harq_process::tx_common(dci_cfg_type,
                          bwp_id,
                          pdsch_slot,
                          pdsch_slot + k1,
                          prbs,
                          harq_tb_tx_params{true, mcs, tbs_bytes, max_harq_nof_retxs});
}

void dl_harq_process::new_retx(bwp_id_t         bwp_id,
                               slot_point       pdsch_slot,
                               unsigned         k1,
                               const prb_grant& prbs,
                               sch_mcs_index    mcs)
{
  harq_process::tx_common(
      dci_config_type(), bwp_id, pdsch_slot, pdsch_slot + k1, prbs, harq_tb_tx_params{false, mcs, 0, 0});
}

void dl_harq_process::tx_2_tb(dci_dl_rnti_config_type            dci_cfg_type_,
                              bwp_id_t                           bwp_id,
                              slot_point                         pdsch_slot,
                              unsigned                           k1,
                              const prb_grant&                   prbs,
                              const optional<harq_tb_tx_params>& tb1,
                              const optional<harq_tb_tx_params>& tb2)
{
  srsgnb_assert(dci_cfg_type_ == dci_type or empty(), "DCI format cannot change during TB retxs");
  harq_process::tx_common(dci_cfg_type_, bwp_id, pdsch_slot, pdsch_slot + k1, prbs, tb1, tb2);
}

void ul_harq_process::new_tx(dci_ul_rnti_config_type dci_cfg_type,
                             bwp_id_t                bwp_id,
                             slot_point              pusch_slot,
                             const prb_grant&        prbs,
                             sch_mcs_index           mcs_idx,
                             unsigned                tbs_bytes,
                             unsigned                max_harq_retxs)
{
  harq_process::tx_common(
      dci_cfg_type, bwp_id, pusch_slot, pusch_slot, prbs, harq_tb_tx_params{true, mcs_idx, tbs_bytes, max_harq_retxs});
}

void ul_harq_process::new_retx(bwp_id_t bwp_id, slot_point pusch_slot, const prb_grant& prbs, sch_mcs_index mcs_idx)
{
  harq_process::tx_common(
      dci_config_type(), bwp_id, pusch_slot, pusch_slot, prbs, harq_tb_tx_params{false, mcs_idx, 0, 0});
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
      logger.info(
          "SCHED: rnti={:#x}, DL HARQ id={}, TB={} - Discarding HARQ. Cause: Maximum number of retx exceeded ({})",
          rnti,
          dl_h.id,
          dl_h.max_nof_harq_retxs(0));
    }
  }
  for (ul_harq_process& ul_h : ul_harqs) {
    bool was_empty = ul_h.empty();
    ul_h.slot_indication(slot_tx);
    if (not was_empty and ul_h.empty()) {
      // Toggle in HARQ state means that the HARQ was discarded
      logger.info(
          "SCHED: rnti={:#x}, UL HARQ id={}, TB={} - Discarding HARQ. Cause: Maximum number of retx exceeded ({})",
          rnti,
          ul_h.id,
          ul_h.max_nof_harq_retxs());
    }
  }
}

} // namespace srsgnb