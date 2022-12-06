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

void f1u_bearer_impl::handle_pdu(byte_buffer pdu)
{
  // TODO.
  fmt::print("F1-U bearer with DRB id={} received PDU!", drb_id);
}

void f1u_bearer_impl::handle_sdu(byte_buffer sdu, uint32_t count)
{
  // TODO.
  fmt::print("F1-U bearer with DRB id={} received SDU with count={}!", drb_id, count);
}

void f1u_bearer_impl::discard_sdu(uint32_t count)
{
  // TODO.
  fmt::print("F1-U bearer with DRB id={} received order to discard SDU with count={}", drb_id, count);
}
