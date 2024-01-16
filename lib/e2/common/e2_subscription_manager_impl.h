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

#include "../procedures/e2_indication_procedure.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include <map>

namespace srsran {

class e2_subscription_manager_impl : public e2_subscription_manager
{
public:
  explicit e2_subscription_manager_impl(e2_message_notifier& notif_, e2sm_manager& e2sm_mngr_);
  virtual ~e2_subscription_manager_impl() = default;

  /// \brief  Handles the subscription request message.
  /// \param[in] msg  The subscription request message.
  /// \return The subscription response message.
  e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg) override;

  /// \brief  Handles the subscription delete request message.
  /// \param[in] msg The subscription delete request message.
  /// \return The subscription delete response message.
  e2_subscribe_delete_response_message
  handle_subscription_delete(const asn1::e2ap::ricsubscription_delete_request_s& msg) override;

  /// \brief  Starts the subscription procedure associated with the given ric instance id.
  /// \param[in] ric_request_id  The ric request id.
  /// \param[in] ev_mng The event manager that will be used to start the subscription procedure.
  void start_subscription(const asn1::e2ap::ri_crequest_id_s& ric_request_id,
                          e2_event_manager&                   ev_mng,
                          uint16_t                            ran_func_id) override;

  /// @brief  Stops the subscription procedure associated with the given ric instance id.
  /// @param[in] ric_request_id  The ric request id.
  /// \param[in] ev_mng The event manager that will be used to end the subscription procedure.
  void stop_subscription(const asn1::e2ap::ri_crequest_id_s&                 ric_request_id,
                         e2_event_manager&                                   ev_mng,
                         const asn1::e2ap::ricsubscription_delete_request_s& msg) override;

  /// \brief checks whether the given action is supported.
  /// \param[in] action The action to check.
  /// \param[in] ran_func_id The ran function id.
  /// \param[in] ri_crequest_id_s associated with the subscription.
  /// \return true if the action is supported, false otherwise.
  bool action_supported(const asn1::e2ap::ri_caction_to_be_setup_item_s& action,
                        uint16_t                                         ran_func_id,
                        const asn1::e2ap::ri_crequest_id_s&              ric_request_id);

  /// \brief  Gets the subscription outcome based on the subscription.
  /// \param[in]  ran_func_id The ran function id.
  /// \param[out] outcome The subscription response message.
  /// \param[in]  subscription  The subscription to use.
  /// \param[in]  actions The actions that have been requested.
  void get_subscription_result(uint16_t                                          ran_func_id,
                               e2_subscribe_reponse_message&                     outcome,
                               e2_subscription_t&                                subscription,
                               const asn1::e2ap::ri_cactions_to_be_setup_list_l& actions);

  /// \brief Adds an e2sm service to the list of services that can be used to unpack e2sm messages.
  /// \param[in] oid The oid of the e2sm service.
  /// \param[in] e2sm_packer The interface that will be used to unpack the e2sm messages for this service
  void add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface) override;

  /// \brief Gets the e2sm service interface associated with the given oid.
  /// \param[in] oid The oid of the e2sm service.
  /// \return The e2sm service interface associated with the given oid.
  e2sm_interface* get_e2sm_interface(std::string oid) override;

  /// @brief Adds a supported ran function oid.
  /// @param ran_func_id  The ran function id.
  /// @param oid  The oid of the e2sm service associated with the ran function.
  void add_ran_function_oid(uint16_t ran_func_id, std::string oid) override;

private:
  std::map<uint32_t, e2_subscription_t>                  subscriptions;
  std::map<std::string, std::unique_ptr<e2sm_interface>> e2sm_iface_list;
  std::map<uint16_t, std::string>                        supported_ran_functions;
  e2_message_notifier&                                   notif;
  e2sm_manager&                                          e2sm_mngr;
  srslog::basic_logger&                                  logger;
};

} // namespace srsran
