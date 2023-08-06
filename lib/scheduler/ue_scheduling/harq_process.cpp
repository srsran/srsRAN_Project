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
  id(h_id),
  logger(logger_),
  timeout_notifier(timeout_notif),
  max_ack_wait_in_slots(max_ack_wait_in_slots_),
  ack_wait_in_slots(max_ack_wait_in_slots_)
{
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::slot_indication(slot_point slot_tx)
{
  for (transport_block& tb : tb_array) {
    if (tb.state == transport_block::state_t::empty) {
      continue;
    }
    if (last_slot_ack + ack_wait_in_slots > slot_tx) {
      // Wait more slots for ACK/NACK to arrive.
      return;
    }
    if (tb.state == transport_block::state_t::waiting_ack) {
      // ACK went missing.
      tb.state = transport_block::state_t::pending_retx;
      if (ack_wait_in_slots == max_ack_wait_in_slots) {
        logger.warning(id,
                       "Setting HARQ to \"pending reTx\" state. Cause: HARQ-ACK wait timeout ({} slots) was reached "
                       "but no HARQ-ACK report was received.",
                       ack_wait_in_slots);
      } else {
        logger.debug(id,
                     "Setting HARQ to \"pending reTx\" state. Cause: HARQ-ACK wait timeout ({} slots) was reached "
                     "but only invalid HARQ-ACKs were received so far.",
                     ack_wait_in_slots);
      }
      timeout_notifier.notify_harq_timeout(IsDownlink);
    }
    if (tb.nof_retxs + 1 > tb.max_nof_harq_retxs) {
      // Max number of reTxs was exceeded. Clear HARQ process
      tb.state = transport_block::state_t::empty;
      if (ack_wait_in_slots == max_ack_wait_in_slots) {
        logger.warning(id,
                       "Discarding HARQ. Cause: HARQ-ACK wait timeout ({} slots) was reached without a HARQ-ACK report "
                       "being received and the maximum number of reTxs {} was exceeded",
                       ack_wait_in_slots,
                       max_nof_harq_retxs(0));
      } else {
        logger.debug(id,
                     "Discarding HARQ. Cause: HARQ-ACK wait timeout ({} slots) was reached but only invalid HARQ-ACKs "
                     "were received and the maximum number of reTxs {} was exceeded",
                     ack_wait_in_slots,
                     max_nof_harq_retxs(0));
      }
    }
    // Reset the ACK wait time.
    ack_wait_in_slots = max_ack_wait_in_slots;
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
void detail::harq_process<IsDownlink>::stop_retransmissions(unsigned tb_idx)
{
  if (empty(tb_idx)) {
    return;
  }
  tb_array[tb_idx].max_nof_harq_retxs = tb_array[tb_idx].nof_retxs;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::reset_tb(unsigned tb_idx)
{
  tb_array[tb_idx].state     = transport_block::state_t::empty;
  tb_array[tb_idx].nof_retxs = 0;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::tx_common(slot_point slot_tx_, slot_point slot_ack_)
{
  last_slot_tx      = slot_tx_;
  last_slot_ack     = slot_ack_;
  ack_wait_in_slots = max_ack_wait_in_slots;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::new_tx_tb_common(unsigned tb_idx, unsigned max_nof_harq_retxs, uint8_t dai)
{
  srsran_assert(tb_idx < tb_array.size(), "TB index is out-of-bounds");
  srsran_assert(empty(tb_idx), "Cannot allocate newTx non-empty HARQ TB");
  tb_array[tb_idx].state              = transport_block::state_t::waiting_ack;
  tb_array[tb_idx].ndi                = !tb_array[tb_idx].ndi;
  tb_array[tb_idx].max_nof_harq_retxs = max_nof_harq_retxs;
  tb_array[tb_idx].nof_retxs          = 0;
  tb_array[tb_idx].dai                = dai;
}

template <bool IsDownlink>
void detail::harq_process<IsDownlink>::new_retx_tb_common(unsigned tb_idx, uint8_t dai)
{
  srsran_assert(tb_idx < tb_array.size(), "TB index is out-of-bounds");
  srsran_assert(tb_array[tb_idx].state == transport_block::state_t::pending_retx,
                "Cannot allocate reTx in HARQ without a pending reTx");
  tb_array[tb_idx].state = transport_block::state_t::waiting_ack;
  tb_array[tb_idx].dai   = dai;
  tb_array[tb_idx].nof_retxs++;
}

// Explicit template instantiation.
template class detail::harq_process<true>;
template class detail::harq_process<false>;

void dl_harq_process::new_tx(slot_point pdsch_slot, unsigned k1, unsigned max_harq_nof_retxs, uint8_t dai)
{
  base_type::tx_common(pdsch_slot, pdsch_slot + k1);
  base_type::new_tx_tb_common(0, max_harq_nof_retxs, dai);
  prev_tx_params = {};
  prev_tx_params.tb[0].emplace();
  prev_tx_params.tb[1].reset();
}

void dl_harq_process::new_retx(slot_point pdsch_slot, unsigned k1, uint8_t dai)
{
  base_type::tx_common(pdsch_slot, pdsch_slot + k1);
  base_type::new_retx_tb_common(0, dai);
}

void dl_harq_process::tx_2_tb(slot_point                pdsch_slot,
                              unsigned                  k1,
                              span<const tb_tx_request> tb_tx_req,
                              unsigned                  max_harq_nof_retxs,
                              uint8_t                   dai)
{
  srsran_assert(tb_tx_req.size() == 2, "This function should only be called when 2 TBs are active");
  srsran_assert(
      std::any_of(tb_tx_req.begin(), tb_tx_req.end(), [](const auto& tb) { return tb != tb_tx_request::disabled; }),
      "At least one TB must be enabled in a HARQ allocation");
  base_type::tx_common(pdsch_slot, pdsch_slot + k1);
  for (unsigned i = 0; i != tb_tx_req.size(); ++i) {
    if (tb_tx_req[i] == tb_tx_request::newtx) {
      base_type::new_tx_tb_common(i, max_harq_nof_retxs, dai);
      prev_tx_params.tb[i].emplace();
    } else if (tb_tx_req[i] == tb_tx_request::retx) {
      base_type::new_retx_tb_common(i, dai);
    } else {
      prev_tx_params.tb[i].reset();
    }
  }
}

bool dl_harq_process::ack_info(uint32_t tb_idx, mac_harq_ack_report_status ack)
{
  if (ack == mac_harq_ack_report_status::dtx) {
    // When receing a DTX for the TB_idx 0 that is waiting for an ACK, reduce the ack_wait_in_slots.
    if (tb_idx == 0 and not empty(tb_idx) and tb(tb_idx).state == transport_block::state_t::waiting_ack) {
      ack_wait_in_slots = SHORT_ACK_TIMEOUT_DTX;
    }
    return true;
  }

  // If it is an ACK or NACK, check if the TB_idx is active.
  if (empty(tb_idx)) {
    logger.info(id,
                "Discarding HARQ-ACK tb={} ack={}. Cause: HARQ process is not active",
                tb_idx,
                ack == mac_harq_ack_report_status::ack ? 1 : 0);
    return false;
  }

  // When receiving an ACK or NACK, reset the ack_wait_in_slots to the maximum value.
  ack_wait_in_slots = max_ack_wait_in_slots;

  // From this point on, ack is either mac_harq_ack_report_status::ack or mac_harq_ack_report_status::nack;
  base_type::ack_info_common(tb_idx, ack == mac_harq_ack_report_status::ack);
  if (ack == mac_harq_ack_report_status::nack and empty(tb_idx)) {
    logger.info(id,
                "Discarding HARQ process tb={} with tbs={}. Cause: Maximum number of reTxs {} exceeded",
                tb_idx,
                prev_tx_params.tb[tb_idx]->tbs_bytes,
                max_nof_harq_retxs(tb_idx),
                ack_wait_in_slots);
  }
  return true;
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
                  max_nof_harq_retxs(),
                  ack_wait_in_slots);
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

void ul_harq_process::stop_retransmissions()
{
  base_type::stop_retransmissions(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

harq_entity::harq_entity(rnti_t                   rnti_,
                         unsigned                 nof_dl_harq_procs,
                         unsigned                 nof_ul_harq_procs,
                         ue_harq_timeout_notifier timeout_notif,
                         unsigned                 max_ack_wait_in_slots) :
  rnti(rnti_),
  logger(srslog::fetch_basic_logger("SCHED")),
  dl_h_logger(logger, rnti_, to_du_cell_index(0), true),
  ul_h_logger(logger, rnti_, to_du_cell_index(0), false)
{
  // Create HARQ processes
  dl_harqs.reserve(nof_dl_harq_procs);
  ul_harqs.reserve(nof_ul_harq_procs);
  for (unsigned id = 0; id < nof_dl_harq_procs; ++id) {
    dl_harqs.emplace_back(to_harq_id(id), dl_h_logger, timeout_notif, max_ack_wait_in_slots);
  }
  for (unsigned id = 0; id != nof_ul_harq_procs; ++id) {
    ul_harqs.emplace_back(to_harq_id(id), ul_h_logger, timeout_notif, max_ack_wait_in_slots);
  }
}

void harq_entity::slot_indication(slot_point slot_tx_)
{
  slot_tx = slot_tx_;
  for (dl_harq_process& dl_h : dl_harqs) {
    dl_h.slot_indication(slot_tx);
  }
  for (ul_harq_process& ul_h : ul_harqs) {
    ul_h.slot_indication(slot_tx);
  }
}

const dl_harq_process* harq_entity::dl_ack_info(slot_point uci_slot, mac_harq_ack_report_status ack, uint8_t dai)
{
  srsran_assert(dai < 4, "DAI must be in range [0, 3]");
  // For the time being, we assume 1 TB only.
  static const size_t tb_index = 0;

  dl_harq_process* harq_candidate     = nullptr;
  bool             harq_update_needed = false;
  for (dl_harq_process& h_dl : dl_harqs) {
    if (h_dl.slot_ack() == uci_slot) {
      if (h_dl.is_waiting_ack(tb_index) and h_dl.tb(0).dai == dai) {
        // Update HARQ state and stop search.
        h_dl.ack_info(tb_index, ack);
        return &h_dl;
      }
      if (ack == mac_harq_ack_report_status::ack and not h_dl.empty(tb_index) and h_dl.tb(0).dai == dai) {
        // The HARQ might have been NACKed (false alarm from PHY) before. In this case, there is some ambiguity
        // on whether this HARQ bit is for this HARQ or another. We save this candidate and continue searching. If no
        // better candidate is found, we will update this HARQ.
        harq_candidate     = &h_dl;
        harq_update_needed = true;
        continue;
      }
      if (harq_candidate == nullptr) {
        // Handle case when two HARQ-ACKs arrive for the same HARQ, and the first ACK empties the HARQ.
        harq_candidate = &h_dl;
      }
    }
  }
  if (harq_candidate != nullptr and harq_update_needed) {
    harq_candidate->ack_info(tb_index, ack);
  }
  if (harq_candidate == nullptr and ack != mac_harq_ack_report_status::dtx) {
    // Note: In the situations when two PUCCH PDUs are scheduled for the same slot, it can happen that the first PDU
    // empties the HARQ with an ACK and the HARQ cannot be found anymore by the second HARQ PDU. In such situation,
    // avoid this warning.
    logger.warning("DL HARQ for rnti={:#x}, uci slot={} not found.", rnti, uci_slot);
  }
  return harq_candidate;
}

int harq_entity::ul_crc_info(harq_id_t h_id, bool ack, slot_point pusch_slot)
{
  ul_harq_process& h_ul = ul_harq(h_id);
  if (h_ul.empty() or h_ul.slot_ack() != pusch_slot) {
    ul_h_logger.warning(h_id, "Discarding CRC. Cause: No active UL HARQ expecting a CRC at slot={}", pusch_slot);
    return -1;
  }
  return h_ul.crc_info(ack);
}
