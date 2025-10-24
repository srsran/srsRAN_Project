/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

/// This coroutines handles the E1AP CU UP release procedure as per TS 38.483, 8.2.7.2.2.
class e1ap_cu_up_reset_procedure
{
public:
  e1ap_cu_up_reset_procedure(asn1::e1ap::reset_s          reset_msg_,
                             e1ap_ue_context_list&        ue_ctxt_list_,
                             e1ap_cu_up_manager_notifier& cu_up_notifier_,
                             e1ap_message_notifier&       tx_pdu_notifier_,
                             srslog::basic_logger&        logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  static const char* name() { return "E1AP CU-UP Reset Procedure"; }
  async_task<void>   handle_e1_interface_reset(const asn1::e1ap::reset_all_e& reset_all);
  async_task<void>
       handle_part_of_e1_interface_reset(const asn1::e1ap::ue_associated_lc_e1_conn_list_res_l& ue_reset_list);
  void send_e1ap_reset_ack();

  asn1::e1ap::reset_s          reset_msg;
  e1ap_ue_context_list&        ue_ctxt_list;
  e1ap_cu_up_manager_notifier& cu_up_notifier;
  e1ap_message_notifier&       tx_pdu_notifier;
  srslog::basic_logger&        logger;
};

} // namespace srs_cu_up
} // namespace srsran
