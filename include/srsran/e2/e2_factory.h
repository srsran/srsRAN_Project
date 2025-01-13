/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/e2/e2.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/e2/subscription/e2_subscription.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {

/// Creates an instance of an E2 interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e2_interface> create_e2(e2ap_configuration&      e2ap_cfg_,
                                        e2ap_e2agent_notifier&   agent_notifier_,
                                        timer_factory            timers_,
                                        e2_connection_client&    e2_client_,
                                        e2_subscription_manager& e2_subscriber_,
                                        e2sm_manager&            e2sm_manager_,
                                        task_executor&           task_exec_);

/// Creates an instance of an E2AP ASN1 packer.
std::unique_ptr<e2ap_packer>
create_e2ap_asn1_packer(sctp_network_gateway_data_handler& gw_, e2_message_handler& e2_, dlt_pcap& pcap_);
} // namespace srsran
