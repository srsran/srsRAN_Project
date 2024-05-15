/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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