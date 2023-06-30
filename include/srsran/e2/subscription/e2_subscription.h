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

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"

namespace srsran {

struct ric_action_t {
  srsran::byte_buffer           action_definition;
  uint16_t                      ric_action_id;
  asn1::e2ap::ri_caction_type_e ric_action_type;
};

// here we define a subscription struct
struct e2_subscription_info_t {
  asn1::e2ap::ri_crequest_id_s request_id;
  std::vector<ric_action_t>    action_list;
  uint64_t                     report_period;
};

struct e2_subscription_t {
  e2_subscription_info_t subscription_info;
  eager_async_task<void> indication_task;
};

class e2_subscription_manager
{
public:
  virtual ~e2_subscription_manager() = default;

  /// \brief Handle the incoming subscription message.
  virtual e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg) = 0;
  /// \brief start the subscription request
  virtual int start_subscription(int ric_instance_id, e2_event_manager& ev_mng, uint16_t ran_func_id) = 0;

  virtual void add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface) = 0;

  virtual e2sm_interface* get_e2sm_interface(std::string oid) = 0;

  virtual void add_ran_function_oid(uint16_t ran_func_id, std::string oid) = 0;
};

} // namespace srsran
