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

using namespace srsgnb;
using namespace srs_du;

f1u_bearer_impl::f1u_bearer_impl(drb_id_t             drb_id_,
                                 f1u_rx_sdu_notifier& rx_sdu_notifier_,
                                 f1u_tx_pdu_notifier& tx_pdu_notifier_) :
  drb_id(drb_id_), rx_sdu_notifier(rx_sdu_notifier_), tx_pdu_notifier(tx_pdu_notifier_)
{
}

void f1u_bearer_impl::handle_sdu(byte_buffer_slice_chain sdu)
{
  fmt::print("F1-U bearer with DRB id={} received SDU!\n", drb_id);
  nru_ul_message msg = {};
  msg.t_pdu          = std::move(sdu);
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::handle_pdu(nru_dl_message msg)
{
  fmt::print("F1-U bearer with DRB id={} received PDU!\n", drb_id);
  if (!msg.t_pdu.empty()) {
    rx_sdu_notifier.on_new_sdu(std::move(msg.t_pdu), msg.pdcp_count);
  }
  if (msg.dl_user_data.dl_discard_blocks) {
    uint8_t nof_blocks = std::min(msg.dl_user_data.dl_discard_nof_blocks, nru_max_nof_pdcp_sn_discard_blocks);
    for (uint8_t block_idx = 0; block_idx < nof_blocks; block_idx++) {
      const nru_pdcp_sn_discard_block& block       = msg.dl_user_data.discard_blocks[block_idx];
      uint32_t                         pdcp_sn_end = block.dl_discard_pdcp_sn_start + block.discarded_block_size;
      for (uint32_t pdcp_sn = block.dl_discard_pdcp_sn_start; pdcp_sn < pdcp_sn_end; pdcp_sn++) {
        rx_sdu_notifier.on_discard_sdu(pdcp_sn);
      }
    }
  }
}

void f1u_bearer_impl::handle_delivered_sdu(uint32_t count)
{
  // TODO: Do not forget to switch execution context.
  fmt::print("F1-U bearer with DRB id={} delivered Tx SDU with count={}!\n", drb_id, count);
}
