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

f1u_bearer_impl::f1u_bearer_impl(drb_id_t drb_id_, f1u_rx_sdu_notifier& rx_sdu_notifier_) :
  drb_id(drb_id_), rx_sdu_notifier(rx_sdu_notifier_)
{
}

void f1u_bearer_impl::handle_sdu(byte_buffer_slice_chain sdu)
{
  // TODO.
  fmt::print("F1-U bearer with DRB id={} received SDU!\n", drb_id);
}

void f1u_bearer_impl::handle_pdu(nru_dl_message msg)
{
  // TODO.
  fmt::print("F1-U bearer with DRB id={} received PDU!\n", drb_id);
  rx_sdu_notifier.on_new_sdu(std::move(msg.t_pdu), 0);
}

void f1u_bearer_impl::handle_delivered_sdu(uint32_t count)
{
  // TODO: Do not forget to switch execution context.
  fmt::print("F1-U bearer with DRB id={} delivered Tx SDU with count={}!\n", drb_id, count);
}
