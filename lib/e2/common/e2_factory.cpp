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

using namespace srsran;

std::unique_ptr<e2_interface> srsran::create_e2(timer_factory            timers_,
                                                e2_message_notifier&     e2_pdu_notifier_,
                                                e2_subscription_manager& e2_subscription_mngr_)
{
  auto e2 = std::make_unique<e2_impl>(timers_, e2_pdu_notifier_, e2_subscription_mngr_);
  return e2;
}

std::unique_ptr<e2_interface> srsran::create_e2_external(timer_factory            timers_,
                                                         e2_message_notifier&     e2_pdu_notifier_,
                                                         e2_subscription_manager& e2_subscription_mngr_,
                                                         task_executor&           e2_exec_)
{
  auto e2     = std::make_unique<e2_impl>(timers_, e2_pdu_notifier_, e2_subscription_mngr_);
  auto e2_ext = std::make_unique<e2_external_impl>(std::move(e2), e2_exec_);
  return e2_ext;
}
