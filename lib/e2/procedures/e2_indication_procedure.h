/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "../common/e2ap_asn1_utils.h"
#include "e2_event_manager.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"

using namespace asn1::e2sm_kpm;
class e2_event_manager;

namespace srsran {

class e2_indication_procedure
{
public:
  e2_indication_procedure(e2_message_notifier&    notif_,
                          e2sm_interface&         e2sm_,
                          e2_event_manager&       ev_mng_,
                          e2_subscription_info_t& subscription_info_,
                          srslog::basic_logger&   logger_);

  void operator()(coro_context<eager_async_task<void>>& ctx);

private:
  /// Send E2 INDICATION.
  void send_e2_indication(e2_indication_message& e2_ind);

  /// Checks whether the indication procedure should be ended.
  bool send_incidation_procedure();

  /// Creates procedure result to send back to procedure caller.
  e2_indication_message create_e2_indication_result();

  bool                               check_measurement_name(meas_type_c meas_type, const char* meas);
  e2_message_notifier&               notifier;
  e2_event_manager&                  ev_mng;
  e2sm_interface&                    e2sm;
  e2_subscription_info_t&            subscription;
  srslog::basic_logger&              logger;
  std::vector<e2_indication_message> e2_ind_vec = {};
  e2_sm_kpm_ind_hdr_s                ric_ind_header;
  e2_sm_kpm_ind_msg_s                ric_ind_message;

  protocol_transaction_outcome_observer<asn1::e2ap::ricsubscription_delete_request_s> transaction_sink;

  unsigned             e2_indication_retry_no = 0;
  std::chrono::seconds time_to_wait{1};
  scheduler_ue_metrics _metrics = {};
  bool                 running  = true;
};

} // namespace srsran
