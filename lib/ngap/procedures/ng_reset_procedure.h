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

#include "ngap_transaction_manager.h"
#include "ue_context/ngap_ue_context.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ng_reset_procedure
{
public:
  ng_reset_procedure(const cu_cp_reset&        msg_,
                     ngap_message_notifier&    amf_notif_,
                     ngap_transaction_manager& ev_mng_,
                     ngap_ue_context_list&     ue_ctxt_list_,
                     srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "NG Reset Procedure"; }

  bool send_ng_reset();

private:
  const cu_cp_reset         msg;
  ngap_message_notifier&    amf_notifier;
  ngap_transaction_manager& ev_mng;
  ngap_ue_context_list&     ue_ctxt_list;
  srslog::basic_logger&     logger;

  protocol_transaction_outcome_observer<asn1::ngap::ng_reset_ack_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran
