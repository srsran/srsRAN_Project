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
#include "du/procedures/f1ap_du_ue_context_modification_procedure.h"
#include "du/procedures/f1ap_du_ue_context_setup_procedure.h"
#include "srsran/f1ap/du/f1ap_du_ue_config.h"

using namespace srsran;
using namespace srsran::srs_du;

void f1ap_du_ue::handle_ue_context_setup_request(const asn1::f1ap::ue_context_setup_request_s& msg)
{
  du_handler.get_ue_handler(context.ue_index)
      .schedule_async_task(launch_async<f1ap_du_ue_context_setup_procedure>(msg, *this));
}

void f1ap_du_ue::handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg)
{
  du_handler.get_ue_handler(context.ue_index)
      .schedule_async_task(launch_async<f1ap_du_ue_context_modification_procedure>(msg, *this));
}
