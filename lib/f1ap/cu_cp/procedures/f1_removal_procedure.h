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

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class f1ap_du_processor_notifier;
class f1ap_ue_context_list;

/// \brief Handle the F1 Removal Procedure as per TS 38.473 section 8.2.8.
class f1_removal_procedure
{
public:
  f1_removal_procedure(const asn1::f1ap::f1_removal_request_s& request,
                       f1ap_message_notifier&                  pdu_notifier,
                       f1ap_du_processor_notifier&             cu_cp_notifier,
                       f1ap_ue_context_list&                   ue_list,
                       srslog::basic_logger&                   logger);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char*      name() const { return "F1 Removal"; }
  async_task<void> handle_ue_transaction_info_loss();
  void             send_f1_removal_response();

  const asn1::f1ap::f1_removal_request_s request;
  f1ap_message_notifier&                 pdu_notifier;
  f1ap_du_processor_notifier&            cu_cp_notifier;
  f1ap_ue_context_list&                  ue_list;
  srslog::basic_logger&                  logger;
};

} // namespace srs_cu_cp
} // namespace srsran