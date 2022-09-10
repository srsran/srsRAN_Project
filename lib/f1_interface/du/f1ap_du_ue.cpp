/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_ue.h"
#include "procedures/f1ap_du_ue_context_modification_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

void f1ap_du_ue::handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg)
{
  du_handler.get_ue_handler(ue_index).schedule_async_task(
      launch_async<f1ap_du_ue_context_modification_procedure>(msg, *this));
}
