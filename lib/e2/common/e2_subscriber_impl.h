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

#include "../procedures/e2_indication_procedure.h"
#include "e2sm_kpm_asn1_packer.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include <map>
using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;

namespace srsran {

class e2_subscriber_impl : public e2_subscriber
{
public:
  explicit e2_subscriber_impl(e2sm_kpm_handler&        e2sm_kpm,
                              e2_message_notifier&     notif_,
                              timer_factory            timers_,
                              e2_du_metrics_interface& du_metrics_interface_);
  virtual ~e2_subscriber_impl() = default;

  e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg) override;

  // in this function we will start a timer with a lambda function create and send the indication message, check whether
  // the subscription is still active and then call the function again
  int start_subscription(int ric_instance_id, e2_event_manager& ev_mng) override;

  bool action_supported(ric_action_t action);

  void get_subscription_result(e2_subscribe_reponse_message& outcome, e2_subscription_t& subscription);

private:
  std::map<int, e2_subscription_t> subscriptions;
  e2sm_kpm_handler&                e2sm_handler;
  e2_message_notifier&             notif;
  timer_factory                    timers;
  e2_du_metrics_interface&         du_metrics_interface;
  srslog::basic_logger&            logger;
};

} // namespace srsran
