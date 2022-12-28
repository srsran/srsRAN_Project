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
using namespace srs_cu_up;

void f1u_bearer_impl::handle_pdu(nru_ul_message msg)
{
  fmt::print("F1-U bearer with DRB id={} received PDU!", drb_id);
  if (!msg.t_pdu.empty()) {
    rx_sdu_notifier.on_new_sdu(std::move(msg.t_pdu));
  }
  // TODO: notify successfully delivered PDCP SNs
}

void f1u_bearer_impl::handle_sdu(pdcp_tx_pdu sdu)
{
  fmt::print("F1-U bearer with DRB id={} received SDU with count={}!", drb_id, sdu.pdcp_count);
  nru_dl_message msg = {};
  msg.t_pdu          = std::move(sdu.buf);
  msg.has_pdcp_count = sdu.has_pdcp_count;
  msg.pdcp_count     = sdu.pdcp_count;
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::discard_sdu(uint32_t count)
{
  fmt::print("F1-U bearer with DRB id={} received order to discard SDU with count={}", drb_id, count);
  nru_dl_message msg                               = {};
  msg.dl_user_data.dl_discard_blocks               = true;
  msg.dl_user_data.dl_discard_nof_blocks           = 1;
  msg.dl_user_data.discard_blocks[0].pdcp_sn_start = count;
  msg.dl_user_data.discard_blocks[0].block_size    = 1;
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}
