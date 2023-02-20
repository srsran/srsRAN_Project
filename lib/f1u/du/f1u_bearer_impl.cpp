/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_bearer_impl.h"

using namespace srsran;
using namespace srs_du;

f1u_bearer_impl::f1u_bearer_impl(uint32_t             ue_index,
                                 drb_id_t             drb_id_,
                                 f1u_rx_sdu_notifier& rx_sdu_notifier_,
                                 f1u_tx_pdu_notifier& tx_pdu_notifier_) :
  logger("F1-U", {ue_index, drb_id_}), rx_sdu_notifier(rx_sdu_notifier_), tx_pdu_notifier(tx_pdu_notifier_)
{
}

void f1u_bearer_impl::handle_sdu(byte_buffer_slice_chain sdu)
{
  logger.log_debug("F1-U bearer received SDU with size={}", sdu.length());
  nru_ul_message msg = {};
  msg.t_pdu          = std::move(sdu);
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::handle_pdu(nru_dl_message msg)
{
  logger.log_debug("F1-U bearer received PDU");
  // handle T-PDU
  if (!msg.t_pdu.empty()) {
    logger.log_debug("Delivering T-PDU of size={}, pdcp_sn={}", msg.t_pdu.length(), msg.pdcp_sn);
    pdcp_tx_pdu tx_sdu = {};
    tx_sdu.buf         = std::move(msg.t_pdu);
    tx_sdu.pdcp_sn     = msg.pdcp_sn;
    rx_sdu_notifier.on_new_sdu(tx_sdu);
  }
  // handle discard notifications
  if (msg.dl_user_data.discard_blocks.has_value()) {
    nru_pdcp_sn_discard_blocks& blocks     = msg.dl_user_data.discard_blocks.value();
    size_t                      nof_blocks = blocks.size();
    for (size_t block_idx = 0; block_idx < nof_blocks; block_idx++) {
      const nru_pdcp_sn_discard_block& block       = blocks[block_idx];
      uint32_t                         pdcp_sn_end = block.pdcp_sn_start + block.block_size;
      for (uint32_t pdcp_sn = block.pdcp_sn_start; pdcp_sn < pdcp_sn_end; pdcp_sn++) {
        logger.log_debug("Notifying PDU discard for pdcp_sn={}", pdcp_sn);
        rx_sdu_notifier.on_discard_sdu(pdcp_sn);
      }
    }
  }
}

void f1u_bearer_impl::handle_transmit_notification(uint32_t highest_pdcp_sn)
{
  logger.log_debug("Notifying highest transmitted pdcp_sn={}", highest_pdcp_sn);
  nru_dl_data_delivery_status status = {};
  status.highest_transmitted_pdcp_sn = highest_pdcp_sn;
  nru_ul_message msg                 = {};
  msg.data_delivery_status           = std::move(status);
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::handle_delivery_notification(uint32_t highest_pdcp_sn)
{
  logger.log_debug("Notifying highest successfully delivered pdcp_sn={}", highest_pdcp_sn);
  nru_dl_data_delivery_status status = {};
  status.highest_delivered_pdcp_sn   = highest_pdcp_sn;
  nru_ul_message msg                 = {};
  msg.data_delivery_status           = std::move(status);
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}
