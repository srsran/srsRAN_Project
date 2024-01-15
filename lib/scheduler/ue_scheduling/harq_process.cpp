/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "harq_process.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

/// \brief No-op handler for HARQ timeout.
class noop_harq_timeout_handler final : public harq_timeout_handler
{
public:
  void handle_harq_timeout(du_ue_index_t /**/, bool /**/) override
  {
    // do nothing
  }
};

static noop_harq_timeout_handler noop_handler;

ue_harq_timeout_notifier::ue_harq_timeout_notifier() : handler(&noop_handler), ue_index(INVALID_DU_UE_INDEX) {}

ue_harq_timeout_notifier::ue_harq_timeout_notifier(harq_timeout_handler& handler_, du_ue_index_t ue_index_) :
  handler(&handler_), ue_index(ue_index_)
{
}

template <bool IsDownlink>
detail::harq_process<IsDownlink>::harq_process(harq_id_t                h_id,
                                               harq_logger&             logger_,
                                               ue_harq_timeout_notifier timeout_notif,
                                               unsigned                 max_ack_wait_in_slots_) :
  id(h_id), logger(logger_), timeout_notifier(timeout_notif), max_ack_wait_in_slots(max_ack_wait_in_slots_)
{
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::slot_indication(slot_point slot_tx)
{
  last_slot_ind = slot_tx;
  for (transport_block& tb : tb_array) {
    if (tb.state == transport_block::state_t::empty) {
      continue;
    }
    if (slot_ack_timeout > last_slot_ind) {
      // Wait more slots for ACK/NACK to arrive.
      return;
    }

    if (tb.state == transport_block::state_t::pending_retx) {
      // [Implementation-defined] Maximum time we give to the scheduler policy to retransmit a HARQ process.
      const unsigned max_nof_slots_for_retx = last_slot_ack.nof_slots_per_system_frame() / 4;
      if (slot_tx >= (last_slot_ack + max_nof_slots_for_retx)) {
        // If a HARQ retx is never scheduled, the HARQ process will never be cleared. This is a safety mechanism to
        // account for a potential bug or limitation in the scheduler policy that is leaving HARQ processes with
        // pending reTxs for too long.
        tb.state = transport_block::state_t::empty;
        logger.warning(
            id,
            "Discarding HARQ. Cause: Too much time has passed since the last HARQ transmission. The scheduler "
            "policy is likely not prioritizing retransmissions of old HARQ processes.");
      }
      continue;
    }

    // HARQ-ACK is waiting for ACK.

    if (tb.ack_on_timeout) {
      // Case: Not all HARQ-ACKs were received, but at least one positive ACK was received.
      tb.state = transport_block::state_t::empty;
      logger.debug(id,
                   "Setting HARQ to \"ACKed\" state. Cause: HARQ-ACK wait timeout ({} slots) was reached with still "
                   "missing PUCCH HARQ-ACKs. However, one positive ACK was received.",
                   slot_ack_timeout - last_slot_ack);
      continue;
    }

    const bool max_retx_exceeded = tb.nof_retxs + 1 > tb.max_nof_harq_retxs;

    if (not max_retx_exceeded) {
      // ACK went missing, and we only have received NACK/DTX.
      tb.state = transport_block::state_t::pending_retx;
      logger.warning(id,
                     "Setting HARQ to \"pending reTx\" state. Cause: HARQ-ACK wait timeout ({} slots) was reached, "
                     "but there are still missing HARQ-ACKs and none of the received are positive.",
                     slot_ack_timeout - last_slot_ack);
    } else {
      // Max number of reTxs was exceeded. Clear HARQ process.
      tb.state = transport_block::state_t::empty;
      fmt::memory_buffer fmtbuf;
      fmt::format_to(fmtbuf,
                     "Discarding HARQ. Cause: HARQ-ACK wait timeout ({} slots) was reached, but there are still "
                     "missing HARQ-ACKs, none of the received so far are positive and the maximum number of reTxs {} "
                     "was exceeded",
                     slot_ack_timeout - last_slot_ack,
                     tb.max_nof_harq_retxs);

      if (max_ack_wait_in_slots == 1) {
        logger.info(id, to_c_str(fmtbuf));
      } else {
        logger.warning(id, to_c_str(fmtbuf));
      }
    }

    // Report timeout with NACK.
    timeout_notifier.notify_harq_timeout(IsDownlink);
  }
}

template <bool IsDownlink>
bool detail::harq_process<IsDownlink>::ack_info_common(unsigned tb_idx, bool ack)
{
  if (empty(tb_idx)) {
    return false;
  }

  if (ack) {
    tb_array[tb_idx].state = transport_block::state_t::empty;
  } else {
    tb_array[tb_idx].state = (tb_array[tb_idx].nof_retxs < tb_array[tb_idx].max_nof_harq_retxs)
                                 ? transport_block::state_t::pending_retx
                                 : transport_block::state_t::empty;
  }
  return true;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::reset()
{
  for (unsigned i = 0; i != tb_array.size(); ++i) {
    reset_tb(i);
  }
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::cancel_harq(unsigned tb_idx)
{
  if (empty(tb_idx)) {
    return;
  }
  tb_array[tb_idx].max_nof_harq_retxs = tb_array[tb_idx].nof_retxs;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::reset_tb(unsigned tb_idx)
{
  tb_array[tb_idx].state          = transport_block::state_t::empty;
  tb_array[tb_idx].nof_retxs      = 0;
  tb_array[tb_idx].ack_on_timeout = false;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::tx_common(slot_point slot_tx_, slot_point slot_ack_)
{
  last_slot_tx     = slot_tx_;
  last_slot_ack    = slot_ack_;
  slot_ack_timeout = slot_ack_ + max_ack_wait_in_slots;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::new_tx_tb_common(unsigned tb_idx,
                                                        unsigned max_nof_harq_retxs,
                                                        uint8_t  harq_bit_idx)
{
  srsran_assert(tb_idx < tb_array.size(), "TB index is out-of-bounds");
  srsran_assert(empty(tb_idx), "Cannot allocate newTx non-empty HARQ TB");
  tb_array[tb_idx].state              = transport_block::state_t::waiting_ack;
  tb_array[tb_idx].ndi                = !tb_array[tb_idx].ndi;
  tb_array[tb_idx].max_nof_harq_retxs = max_nof_harq_retxs;
  tb_array[tb_idx].nof_retxs          = 0;
  tb_array[tb_idx].harq_bit_idx       = harq_bit_idx;
  tb_array[tb_idx].ack_on_timeout     = false;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::new_retx_tb_common(unsigned tb_idx, uint8_t harq_bit_idx)
{
  srsran_assert(tb_idx < tb_array.size(), "TB index is out-of-bounds");
  srsran_assert(tb_array[tb_idx].state == transport_block::state_t::pending_retx,
                "Cannot allocate reTx in HARQ without a pending reTx");
  tb_array[tb_idx].state        = transport_block::state_t::waiting_ack;
  tb_array[tb_idx].harq_bit_idx = harq_bit_idx;
  tb_array[tb_idx].nof_retxs++;
  tb_array[tb_idx].ack_on_timeout = false;
}

// Explicit template instantiation.
template class detail::harq_process<true>;
template class detail::harq_process<false>;

void dl_harq_process::new_tx(slot_point pdsch_slot,
                             unsigned   k1,
                             unsigned   max_harq_nof_retxs,
                             uint8_t    harq_bit_idx,
                             cqi_value  cqi,
                             unsigned   nof_layers)
{
  base_type::tx_common(pdsch_slot, pdsch_slot + k1);
  base_type::new_tx_tb_common(0, max_harq_nof_retxs, harq_bit_idx);
  prev_tx_params            = {};
  prev_tx_params.cqi        = cqi;
  prev_tx_params.nof_layers = nof_layers;
  prev_tx_params.tb[0].emplace();
  prev_tx_params.tb[1].reset();
  pucch_ack_to_receive = 0;
  chosen_ack           = mac_harq_ack_report_status::dtx;
  last_pucch_snr       = nullopt;
}

void dl_harq_process::new_retx(slot_point pdsch_slot, unsigned k1, uint8_t harq_bit_idx)
{
  base_type::tx_common(pdsch_slot, pdsch_slot + k1);
  base_type::new_retx_tb_common(0, harq_bit_idx);
  pucch_ack_to_receive = 0;
  chosen_ack           = mac_harq_ack_report_status::dtx;
  last_pucch_snr       = nullopt;
}

void dl_harq_process::tx_2_tb(slot_point                pdsch_slot,
                              unsigned                  k1,
                              span<const tb_tx_request> tb_tx_req,
                              unsigned                  max_harq_nof_retxs,
                              uint8_t                   harq_bit_idx)
{
  srsran_assert(tb_tx_req.size() == 2, "This function should only be called when 2 TBs are active");
  srsran_assert(
      std::any_of(tb_tx_req.begin(), tb_tx_req.end(), [](const auto& tb) { return tb != tb_tx_request::disabled; }),
      "At least one TB must be enabled in a HARQ allocation");
  base_type::tx_common(pdsch_slot, pdsch_slot + k1);
  for (unsigned i = 0; i != tb_tx_req.size(); ++i) {
    if (tb_tx_req[i] == tb_tx_request::newtx) {
      base_type::new_tx_tb_common(i, max_harq_nof_retxs, harq_bit_idx);
      prev_tx_params.tb[i].emplace();
    } else if (tb_tx_req[i] == tb_tx_request::retx) {
      base_type::new_retx_tb_common(i, harq_bit_idx);
    } else {
      prev_tx_params.tb[i].reset();
    }
  }
}

dl_harq_process::status_update
dl_harq_process::ack_info(uint32_t tb_idx, mac_harq_ack_report_status ack, optional<float> pucch_snr)
{
  if (not is_waiting_ack(tb_idx)) {
    // If the HARQ process is not expected an HARQ-ACK anymore, it means that it has already been ACKed/NACKed.
    return status_update::error;
  }

  if (ack != mac_harq_ack_report_status::dtx and
      (not last_pucch_snr.has_value() or (pucch_snr.has_value() and last_pucch_snr.value() < pucch_snr.value()))) {
    // Case: If there was no previous HARQ-ACK decoded or the previous HARQ-ACK had lower SNR, this HARQ-ACK is chosen.
    chosen_ack     = ack;
    last_pucch_snr = pucch_snr;
  }

  if (pucch_ack_to_receive <= 1) {
    // Case: This is the last HARQ-ACK that is expected for this HARQ process.

    base_type::ack_info_common(tb_idx, chosen_ack == mac_harq_ack_report_status::ack);
    if (chosen_ack != mac_harq_ack_report_status::ack and empty(tb_idx)) {
      logger.info(id,
                  "Discarding HARQ process tb={} with tbs={}. Cause: Maximum number of reTxs {} exceeded",
                  tb_idx,
                  prev_tx_params.tb[tb_idx]->tbs_bytes,
                  max_nof_harq_retxs(tb_idx));
    }
    return chosen_ack == mac_harq_ack_report_status::ack ? status_update::acked : status_update::nacked;
  }

  // Case: This is not the last PUCCH HARQ-ACK that is expected for this HARQ process.
  pucch_ack_to_receive--;
  tb_array[tb_idx].ack_on_timeout = chosen_ack == mac_harq_ack_report_status::ack;
  // We reduce the HARQ process timeout to receive the next HARQ-ACK. This is done because the two HARQ-ACKs should
  // arrive almost simultaneously, and we in case the second goes missing, we don't want to block the HARQ for too long.
  slot_ack_timeout = last_slot_ind + SHORT_ACK_TIMEOUT_DTX;

  return status_update::no_update;
}

void dl_harq_process::save_alloc_params(dci_dl_rnti_config_type dci_cfg_type, const pdsch_information& pdsch)
{
  unsigned tb_idx = empty(0) ? 1 : 0;
  for (const pdsch_codeword& cw : pdsch.codewords) {
    srsran_assert(not empty(tb_idx), "Setting allocation parameters for empty DL HARQ process id={} TB", id);
    srsran_assert(tb(tb_idx).nof_retxs == 0 or dci_cfg_type == prev_tx_params.dci_cfg_type,
                  "DCI format and RNTI type cannot change during DL HARQ retxs");
    srsran_assert(tb(tb_idx).nof_retxs == 0 or prev_tx_params.tb[tb_idx]->tbs_bytes == cw.tb_size_bytes,
                  "TBS cannot change during DL HARQ retxs ({}!={}). Previous MCS={}, RBs={}. New MCS={}, RBs={}",
                  prev_tx_params.tb[tb_idx]->tbs_bytes,
                  cw.tb_size_bytes,
                  prev_tx_params.tb[tb_idx]->mcs,
                  prev_tx_params.rbs,
                  cw.mcs_index,
                  pdsch.rbs);
    prev_tx_params.tb[tb_idx]->mcs_table = cw.mcs_table;
    prev_tx_params.tb[tb_idx]->mcs       = cw.mcs_index;
    prev_tx_params.tb[tb_idx]->tbs_bytes = cw.tb_size_bytes;
    ++tb_idx;
  }
  prev_tx_params.dci_cfg_type = dci_cfg_type;
  prev_tx_params.rbs          = pdsch.rbs;
  prev_tx_params.nof_symbols  = pdsch.symbols.length();
}

void dl_harq_process::increment_pucch_counter()
{
  ++pucch_ack_to_receive;
}

void ul_harq_process::new_tx(slot_point pusch_slot, unsigned max_harq_retxs)
{
  harq_process::tx_common(pusch_slot, pusch_slot);
  // Note: For UL, DAI is not used, so we set it to zero.
  base_type::new_tx_tb_common(0, max_harq_retxs, 0);
  prev_tx_params = {};
}

void ul_harq_process::new_retx(slot_point pusch_slot)
{
  harq_process::tx_common(pusch_slot, pusch_slot);
  // Note: For UL, DAI is not used, so we set it to zero.
  base_type::new_retx_tb_common(0, 0);
}

int ul_harq_process::crc_info(bool ack)
{
  if (base_type::ack_info_common(0, ack)) {
    if (ack) {
      return (int)prev_tx_params.tbs_bytes;
    }
    if (empty()) {
      logger.info(id,
                  "Discarding HARQ with tbs={}. Cause: Maximum number of reTxs {} exceeded",
                  prev_tx_params.tbs_bytes,
                  max_nof_harq_retxs());
    }
    return 0;
  }
  logger.warning(id, "CRC arrived for inactive HARQ");
  return -1;
}

void ul_harq_process::save_alloc_params(dci_ul_rnti_config_type dci_cfg_type, const pusch_information& pusch)
{
  srsran_assert(not empty(), "Setting allocation parameters for empty DL HARQ process id={} TB", id);
  srsran_assert(tb().nof_retxs == 0 or dci_cfg_type == prev_tx_params.dci_cfg_type,
                "DCI format and RNTI type cannot change during DL HARQ retxs");
  srsran_assert(tb().nof_retxs == 0 or prev_tx_params.tbs_bytes == pusch.tb_size_bytes,
                "TBS cannot change during DL HARQ retxs");

  prev_tx_params.mcs_table    = pusch.mcs_table;
  prev_tx_params.mcs          = pusch.mcs_index;
  prev_tx_params.tbs_bytes    = pusch.tb_size_bytes;
  prev_tx_params.dci_cfg_type = dci_cfg_type;
  prev_tx_params.rbs          = pusch.rbs;
}

void ul_harq_process::cancel_harq()
{
  base_type::cancel_harq(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

harq_entity::harq_entity(rnti_t                   rnti_,
                         unsigned                 nof_dl_harq_procs,
                         unsigned                 nof_ul_harq_procs,
                         ue_harq_timeout_notifier timeout_notif,
                         unsigned                 ntn_cs_koffset,
                         unsigned                 max_ack_wait_in_slots) :
  rnti(rnti_),
  logger(srslog::fetch_basic_logger("SCHED")),
  dl_h_logger(logger, rnti_, to_du_cell_index(0), true),
  ul_h_logger(logger, rnti_, to_du_cell_index(0), false),
  nop_timeout_notifier(),
  ntn_harq(ntn_cs_koffset)
{
  // Create HARQ processes
  dl_harqs.reserve(nof_dl_harq_procs);
  ul_harqs.reserve(nof_ul_harq_procs);
  for (unsigned id = 0; id < nof_dl_harq_procs; ++id) {
    if (ntn_harq.active()) {
      dl_harqs.emplace_back(to_harq_id(id), dl_h_logger, nop_timeout_notifier, ntn_harq.ntn_harq_timeout);
    } else {
      dl_harqs.emplace_back(to_harq_id(id), dl_h_logger, timeout_notif, max_ack_wait_in_slots);
    }
  }

  for (unsigned id = 0; id != nof_ul_harq_procs; ++id) {
    if (ntn_harq.active()) {
      ul_harqs.emplace_back(to_harq_id(id), ul_h_logger, nop_timeout_notifier, ntn_harq.ntn_harq_timeout);
    } else {
      ul_harqs.emplace_back(to_harq_id(id), ul_h_logger, timeout_notif, max_ack_wait_in_slots);
    }
  }
}

void harq_entity::slot_indication(slot_point slot_tx_)
{
  slot_tx = slot_tx_;
  for (dl_harq_process& dl_h : dl_harqs) {
    if (ntn_harq.active()) {
      ntn_harq.save_dl_harq_info(dl_h, slot_tx_);
    }
    dl_h.slot_indication(slot_tx);
  }
  for (ul_harq_process& ul_h : ul_harqs) {
    if (ntn_harq.active()) {
      ntn_harq.save_ul_harq_info(ul_h, slot_tx_);
    }
    ul_h.slot_indication(slot_tx);
  }
}

dl_harq_process::dl_ack_info_result harq_entity::dl_ack_info(slot_point                 uci_slot,
                                                             mac_harq_ack_report_status ack,
                                                             uint8_t                    harq_bit_idx,
                                                             optional<float>            pucch_snr)
{
  // For the time being, we assume 1 TB only.
  static const size_t tb_index = 0;

  for (dl_harq_process& h_dl : dl_harqs) {
    if (h_dl.is_waiting_ack(tb_index) and h_dl.slot_ack() == uci_slot and
        h_dl.tb(tb_index).harq_bit_idx == harq_bit_idx) {
      // Update HARQ state.
      dl_harq_process::status_update status_upd = h_dl.ack_info(tb_index, ack, pucch_snr);
      return {h_dl.id,
              h_dl.last_alloc_params().tb[0]->mcs_table,
              h_dl.last_alloc_params().tb[0]->mcs,
              h_dl.last_alloc_params().tb[0]->tbs_bytes,
              status_upd};
    } else if (ntn_harq.active()) {
      return ntn_harq.pop_ack_info(uci_slot, ack);
    }
  }
  logger.warning("DL HARQ for rnti={}, uci slot={} not found.", rnti, uci_slot);
  return {INVALID_HARQ_ID, pdsch_mcs_table::qam64, sch_mcs_index{0}, 0, dl_harq_process::status_update::error};
}

int harq_entity::ul_crc_info(harq_id_t h_id, bool ack, slot_point pusch_slot)
{
  ul_harq_process& h_ul = ul_harq(h_id);
  if (h_ul.empty() or h_ul.slot_ack() != pusch_slot) {
    if (ntn_harq.active()) {
      if (ack == true) {
        return ntn_harq.pop_tbs(pusch_slot);
      } else {
        ntn_harq.clear_tbs(pusch_slot);
      }
    }
    ul_h_logger.warning(h_id, "Discarding CRC. Cause: No active UL HARQ expecting a CRC at slot={}", pusch_slot);
    return -1;
  }
  return h_ul.crc_info(ack);
}

void harq_entity::dl_ack_info_cancelled(slot_point uci_slot)
{
  // Maximum number of UCI indications that a HARQ process can expect. We set this conservative limit to account for
  // the state when SR, HARQ-ACK+SR, common PUCCH are scheduled.
  static constexpr size_t MAX_ACKS_PER_HARQ = 4;

  for (dl_harq_process& h_dl : dl_harqs) {
    if (not h_dl.empty() and h_dl.slot_ack() == uci_slot) {
      dl_harq_process::status_update ret = dl_harq_process::status_update::no_update;
      for (unsigned count = 0; count != MAX_ACKS_PER_HARQ and ret == dl_harq_process::status_update::no_update;
           ++count) {
        ret = h_dl.ack_info(0, mac_harq_ack_report_status::dtx, nullopt);
      }
      if (ret == dl_harq_process::status_update::no_update) {
        dl_h_logger.warning(h_dl.id, "DL HARQ in an invalid state. Resetting it...");
        h_dl.reset();
      }
    }
  }
}

harq_entity::ntn_tbs_history::ntn_tbs_history(unsigned ntn_cs_koffset_) : ntn_cs_koffset(ntn_cs_koffset_)
{
  if (ntn_cs_koffset > 0) {
    slot_tbs.resize(NTN_CELL_SPECIFIC_KOFFSET_MAX);
    slot_ack_info.resize(NTN_CELL_SPECIFIC_KOFFSET_MAX);
  }
}
