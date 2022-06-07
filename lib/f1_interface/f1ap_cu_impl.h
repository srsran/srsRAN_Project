/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_CU_IMPL_H
#define SRSGNB_F1AP_CU_IMPL_H

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/f1ap_cu.h"
#include "srsgnb/support/async/async_queue.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

class f1ap_cu_impl final : public f1_interface
{
public:
  f1ap_cu_impl(f1ap_message_notifier& event_notifier_);
  ~f1ap_cu_impl();

  void handle_f1ap_setup_response(const f1_setup_response_message& msg) override;

  void handle_message(const asn1::f1ap::f1_ap_pdu_c& msg) override;

  void handle_connection_loss() override {}

private:
  class f1ap_event_manager;

  void handle_initiating_message(const asn1::f1ap::init_msg_s& msg);

  srslog::basic_logger&  logger;
  f1ap_message_notifier& event_notifier;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_F1AP_CU_IMPL_H
