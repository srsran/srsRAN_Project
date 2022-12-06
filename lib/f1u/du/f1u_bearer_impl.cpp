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

void f1u_bearer_impl::handle_pdu(byte_buffer_slice_chain pdu)
{
  // TODO.
  fmt::print("F1u bearer with DRB id={} received PDU!");
}

void f1u_bearer_impl::handle_sdu(byte_buffer sdu)
{
  // TODO.
  fmt::print("F1u bearer with DRB id={} received SDU!");
}

void f1u_bearer_impl::handle_delivered_tx_pdu(uint32_t count)
{
  // TODO: Do not forget to switch execution context.
  fmt::print("F1u bearer with DRB id={} delivered Tx PDU with count={}!", count);
}
