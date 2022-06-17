/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_DU_SETUP_PROCEDURE_H
#define SRSGNB_F1AP_DU_SETUP_PROCEDURE_H

#include "common/f1ap_asn1_utils.h"
#include "f1ap_du_event_manager.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_du {

class f1ap_du_setup_procedure
{
public:
  f1ap_du_setup_procedure(const f1_setup_request_message& request_,
                          f1c_message_notifier&           cu_notif_,
                          f1ap_event_manager&             ev_mng_,
                          srslog::basic_logger&           logger_);

  void operator()(coro_context<async_task<f1_setup_response_message> >& ctx);

private:
  /// Send F1 SETUP REQUEST to CU.
  void send_f1_setup_request();

  /// Checks whether the DU should attempt again to connect to CU.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  f1_setup_response_message create_f1_setup_result();

  srs_du::f1_setup_request_message request;
  f1c_message_notifier&            cu_notifier;
  f1ap_event_manager&              ev_mng;
  srslog::basic_logger&            logger;

  f1ap_event_manager::f1ap_setup_outcome_t f1_resp;
  unsigned                                 f1_setup_retry_no = 0;
  unsigned                                 time_to_wait      = 0;
}; // namespace srs_du

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_SETUP_PROCEDURE_H
