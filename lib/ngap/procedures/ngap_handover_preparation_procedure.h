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

#pragma once

#include "../ngap_asn1_utils.h"
#include "../ngap_context.h"
#include "ngap_transaction_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/async_timer.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_handover_preparation_procedure
{
public:
  ngap_handover_preparation_procedure(const ngap_handover_preparation_request& req_,
                                      ngap_context_t&                          context_,
                                      ngap_message_notifier&                   amf_notif_,
                                      ngap_transaction_manager&                ev_mng_,
                                      timer_factory                            timers,
                                      srslog::basic_logger&                    logger_);

  void operator()(coro_context<async_task<ngap_handover_preparation_response>>& ctx);

private:
  const ngap_handover_preparation_request request;
  ngap_context_t&                         context;
  ngap_message_notifier&                  amf_notifier;
  ngap_transaction_manager&               ev_mng;
  srslog::basic_logger&                   logger;

  unique_timer tng_reloc_prep_timer;

  protocol_transaction_outcome_observer<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> transaction_sink;

  void send_handover_required();
};

} // namespace srs_cu_cp
} // namespace srsran
