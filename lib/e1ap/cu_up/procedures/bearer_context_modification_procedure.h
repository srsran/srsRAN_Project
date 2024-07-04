/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "cu_up/ue_context/e1ap_cu_up_ue_context.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_cu_up {

/// E1 Setup Procedure for the CU-UP as per TS 38.463, section TODO.
class bearer_context_modification_procedure
{
public:
  bearer_context_modification_procedure(const e1ap_ue_context&                          ue_ctxt_,
                                        const asn1::e1ap::bearer_context_mod_request_s& request_,
                                        e1ap_message_notifier&                          pdu_notifier_,
                                        e1ap_cu_up_manager_notifier&                    cu_up_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E1AP CU-UP Bearer Context Modification Procedure"; }

private:
  const e1ap_ue_context                          ue_ctxt;
  const asn1::e1ap::bearer_context_mod_request_s request;
  e1ap_message_notifier&                         pdu_notifier;
  e1ap_cu_up_manager_notifier&                   cu_up_notifier;

  // local variables
  e1ap_message                              e1ap_msg                        = {};
  e1ap_bearer_context_modification_request  bearer_context_mod              = {};
  e1ap_bearer_context_modification_response bearer_context_mod_response_msg = {};

  void prepare_failure_message();
  bool validate_request();
};

} // namespace srsran::srs_cu_up
