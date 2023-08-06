/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "f1ap_ue_context.h"
#include "ue_bearer_manager.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/common/f1ap_types.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/ran/du_types.h"

namespace srsran {
namespace srs_du {

class f1ap_du_ue
{
public:
  f1ap_du_ue(du_ue_index_t          ue_index_,
             gnb_du_ue_f1ap_id_t    gnb_f1ap_du_ue_id_,
             f1ap_du_configurator&  du_handler_,
             f1ap_message_notifier& f1ap_msg_notifier_,
             task_executor&         ctrl_exec,
             task_executor&         ue_exec) :
    context(ue_index_, gnb_f1ap_du_ue_id_),
    f1ap_msg_notifier(f1ap_msg_notifier_),
    du_handler(du_handler_),
    bearers(context, f1ap_msg_notifier, ctrl_exec, ue_exec)
  {
  }

  f1ap_ue_context        context;
  f1ap_message_notifier& f1ap_msg_notifier;
  f1ap_du_configurator&  du_handler;
  ue_bearer_manager      bearers;

  /// \brief Handles UE CONTEXT MODIFICATION REQUEST as per TS38.473, Section 8.3.2.
  void handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg);
};

} // namespace srs_du
} // namespace srsran
