/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

    const bool max_retx_exceeded = tb.nof_retxs + 1 > tb.max_nof_harq_retxs;
    if (tb.state == transport_block::state_t::waiting_ack and not max_retx_exceeded) {
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
    } else if (max_retx_exceeded) {
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
      timeout_notifier.notify_harq_timeout(IsDownlink);
    } else if (slot_tx >= (last_slot_ack + last_slot_ack.nof_slots_per_system_frame() / 4)) {
      // If a HARQ retx is never scheduled, the HARQ process will never be cleared. This is a safety mechanism to
      // account a potential bug or limitation in the scheduler policy.
      tb.state = transport_block::state_t::empty;
      logger.warning(id,
                     "Discarding HARQ. Cause: Too much time has passed since the last HARQ transmission. The scheduler "
                     "policy is likely not prioritizing retransmissions of old HARQ processes.");
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
}

void dl_harq_process::new_retx(slot_point pdsch_slot, unsigned k1, uint8_t harq_bit_idx)
{
  base_type::tx_common(pdsch_slot, pdsch_slot + k1);
  base_type::new_retx_tb_common(0, harq_bit_idx);
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

bool dl_harq_process::ack_info(uint32_t tb_idx, mac_harq_ack_report_status ack)
{
  if (ack == mac_harq_ack_report_status::dtx) {
    // In case of PUCCH F1 SR+HARQ multiplexing, two HARQ-ACK reports are received for the same HARQ process.

    if (not is_waiting_ack(tb_idx)) {
      // If the HARQ process is not expected an HARQ-ACK anymore, it means that it has already been ACKed/NACKed. In
      // such case, we ignore the DTX.
      return true;
    }

    // If this is the first DTX for the HARQ process, we reduce the HARQ process timeout to receive the second HARQ-ACK.
    // This is done because the two HARQ-ACKs should arrive almost simultaneously, and we in case the second goes
    // missing, we don't want to block the HARQ for too long.
    const bool is_first_dtx = ack_wait_in_slots != SHORT_ACK_TIMEOUT_DTX;
    if (is_first_dtx) {
      ack_wait_in_slots = SHORT_ACK_TIMEOUT_DTX;
      return true;
    }

    // If this is the second DTX for the HARQ process transport block, we treat it as a NACK.
    ack = mac_harq_ack_report_status::nack;
  }

  // If it is an ACK or NACK, check if the TB_idx is active.
  if (empty(tb_idx)) {
    logger.info(id,
                "Discarding HARQ-ACK tb={} ack={}. Cause: HARQ process is not active",
                tb_idx,
                ack == mac_harq_ack_report_status::ack ? 1 : 0);
    return false;
  }

  // From this point on, ack is either mac_harq_ack_report_status::ack or mac_harq_ack_report_status::nack;
  base_type::ack_info_common(tb_idx, ack == mac_harq_ack_report_status::ack);
  if (ack == mac_harq_ack_report_status::nack and empty(tb_idx)) {
    logger.info(id,
                "Discarding HARQ process tb={} with tbs={}. Cause: Maximum number of reTxs {} exceeded",
                tb_idx,
                prev_tx_params.tb[tb_idx]->tbs_bytes,
                max_nof_harq_retxs(tb_idx));
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
  prev_tx_params    = {};
  ack_wait_in_slots = max_ack_wait_in_slots;
}

void ul_harq_process::new_retx(slot_point pusch_slot)
{
  harq_process::tx_common(pusch_slot, pusch_slot);
  // Note: For UL, DAI is not used, so we set it to zero.
  base_type::new_retx_tb_common(0, 0);
  ack_wait_in_slots = max_ack_wait_in_slots;
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

void ul_harq_process::cancel_harq()
{
  base_type::cancel_harq(0);
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

const dl_harq_process*
harq_entity::dl_ack_info(slot_point uci_slot, mac_harq_ack_report_status ack, uint8_t harq_bit_idx)
{
  // For the time being, we assume 1 TB only.
  static const size_t tb_index = 0;

  for (dl_harq_process& h_dl : dl_harqs) {
    if (h_dl.slot_ack() == uci_slot and h_dl.tb(tb_index).harq_bit_idx == harq_bit_idx) {
      if (h_dl.is_waiting_ack(tb_index)) {
        // Update HARQ state.
        h_dl.ack_info(tb_index, ack);
      } else {
        // In case of PUCCH F1 SR+HARQ, the HARQ process might have already been set to "pending_retx" or "empty" state
        // by the first ACK/NACK.
        switch (ack) {
          case mac_harq_ack_report_status::ack: {
            if (h_dl.has_pending_retx(tb_index)) {
              // In PUCCH F1, NACK + ACK case, we let the HARQ as ACKed.
              h_dl.ack_info(tb_index, ack);
            }
          } break;
          case mac_harq_ack_report_status::nack:
            // In PUCCH F1, ACK/NACK + NACK case, we ignore the second NACK.
            break;
          case mac_harq_ack_report_status::dtx:
            // In PUCCH F1, ACK/NACK + DTX case, we ignore the DTX.
            break;
          default:
            break;
        }
      }
      return &h_dl;
    }
  }
  logger.warning("DL HARQ for rnti={:#x}, uci slot={} not found.", rnti, uci_slot);
  return nullptr;
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
