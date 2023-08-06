/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/e2/e2_factory.h"
#include "e2_entity.h"
#include "e2_impl.h"
#include "e2ap_asn1_packer.h"

using namespace srsran;

std::unique_ptr<e2_interface> srsran::create_e2(e2ap_configuration&      e2ap_cfg_,
                                                timer_factory            timers_,
                                                e2_message_notifier&     e2_pdu_notifier_,
                                                e2_subscription_manager& e2_subscription_mngr_)
{
  auto e2 = std::make_unique<e2_impl>(e2ap_cfg_, timers_, e2_pdu_notifier_, e2_subscription_mngr_);
  return e2;
}

std::unique_ptr<e2_interface> srsran::create_e2_with_task_exec(e2ap_configuration&      e2ap_cfg_,
                                                               timer_factory            timers_,
                                                               e2_message_notifier&     e2_pdu_notifier_,
                                                               e2_subscription_manager& e2_subscription_mngr_,
                                                               task_executor&           e2_exec_)
{
  auto e2     = std::make_unique<e2_impl>(e2ap_cfg_, timers_, e2_pdu_notifier_, e2_subscription_mngr_);
  auto e2_ext = std::make_unique<e2_entity>(e2ap_cfg_, std::move(e2), e2_exec_);
  return e2_ext;
}

std::unique_ptr<e2_interface> srsran::create_e2_entity(e2ap_configuration&      e2ap_cfg_,
                                                       e2_connection_client*    e2_client_,
                                                       e2_du_metrics_interface& e2_du_metrics_,
                                                       timer_factory            timers_,
                                                       task_executor&           e2_exec_)
{
  auto e2_ext = std::make_unique<e2_entity>(e2ap_cfg_, e2_client_, e2_du_metrics_, timers_, e2_exec_);
  return e2_ext;
}

std::unique_ptr<e2ap_packer>
srsran::create_e2ap_asn1_packer(sctp_network_gateway_data_handler& gw_, e2_message_handler& e2_, dlt_pcap& pcap_)
{
  auto packer = std::make_unique<e2ap_asn1_packer>(gw_, e2_, pcap_);
  return packer;
}
