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
#include "e2_impl.h"

using namespace srsran;

std::unique_ptr<e2_interface> srsran::create_e2(timer_factory            timers_,
                                                e2_message_notifier&     e2_pdu_notifier_,
                                                e2_subscriber&           e2_sub_notif_,
                                                e2_du_metrics_interface& e2_du_metrics_)
{
  auto e2 = std::make_unique<e2_impl>(timers_, e2_pdu_notifier_, e2_sub_notif_, e2_du_metrics_);
  return e2;
}
