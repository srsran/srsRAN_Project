/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../ran/gnb_format.h"
#include "rrc_ue_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

void rrc_ue_impl::on_ue_delete_request(const cause_t& cause)
{
  // FIXME: this enqueues a new CORO on top of an existing one.
  rrc_ue_context_release_command msg = {};
  msg.ue_index                       = context.ue_index;
  msg.cause                          = cause;
  du_processor_notifier.on_ue_context_release_command(msg);
}