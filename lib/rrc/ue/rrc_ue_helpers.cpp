/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../ran/gnb_format.h"
#include "rrc_ue_impl.h"

using namespace srsgnb;
using namespace srs_cu_cp;

void rrc_ue_impl::on_ue_delete_request()
{
  // FIXME: this enqueues a new CORO on top of an existing one.
  cu_cp_ue_context_release_command msg = {};
  msg.ue_index                         = context.ue_index;
  // TODO: Set cause
  du_processor_notifier.on_ue_context_release_command(msg);
}