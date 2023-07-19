/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/e2/e2_factory.h"
#include "e2_external_impl.h"
#include "e2_impl.h"
#include "e2_subscription_manager_impl.h"

using namespace srsran;

std::unique_ptr<e2_interface> srsran::create_e2(e2ap_configuration&      e2ap_cfg_,
                                                timer_factory            timers_,
                                                e2_message_notifier&     e2_pdu_notifier_,
                                                e2_subscription_manager& e2_subscription_mngr_)
{
  auto e2 = std::make_unique<e2_impl>(e2ap_cfg_, timers_, e2_pdu_notifier_, e2_subscription_mngr_);
  return e2;
}

std::unique_ptr<e2_interface> srsran::create_e2_external(e2ap_configuration&      e2ap_cfg_,
                                                         timer_factory            timers_,
                                                         e2_message_notifier&     e2_pdu_notifier_,
                                                         e2_subscription_manager& e2_subscription_mngr_,
                                                         task_executor&           e2_exec_)
{
  auto e2     = std::make_unique<e2_impl>(e2ap_cfg_, timers_, e2_pdu_notifier_, e2_subscription_mngr_);
  auto e2_ext = std::make_unique<e2_external_impl>(std::move(e2), e2_exec_);
  return e2_ext;
}

std::unique_ptr<e2_interface> srsran::create_e2_entity(e2ap_configuration&   e2ap_cfg_,
                                                       timer_factory         timers_,
                                                       e2_connection_client* e2_client,
                                                       task_executor&        e2_exec_)
{
  auto e2_pdu_notifier = e2_client->handle_connection_request();
  auto mngr            = std::make_unique<e2_subscription_manager_impl>(*e2_pdu_notifier);
  auto e2              = std::make_unique<e2_impl>(e2ap_cfg_, timers_, *e2_pdu_notifier, *mngr);
  auto e2_ext          = std::make_unique<e2_external_impl>(std::move(e2), e2_exec_);

  e2_client->connect_e2ap(std::move(e2_pdu_notifier), e2.get(), e2.get());

  return e2_ext;
}
