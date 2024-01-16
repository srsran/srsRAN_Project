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

#include "../f1ap_cu_impl.h"
#include "common/f1ap_asn1_utils.h"
#include "cu_cp/ue_context/f1ap_cu_ue_transaction_manager.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ue_context_setup_procedure
{
public:
  ue_context_setup_procedure(const f1ap_ue_context_setup_request& request_,
                             f1ap_ue_context_list&                ue_ctxt_list_,
                             f1ap_du_processor_notifier&          du_processor_notifier_,
                             f1ap_message_notifier&               f1ap_notif_,
                             srslog::basic_logger&                logger_,
                             optional<rrc_ue_transfer_context>    rrc_context);

  void operator()(coro_context<async_task<f1ap_ue_context_setup_response>>& ctx);

  static const char* name() { return "UE Context Setup Procedure"; }

private:
  /// Allocate F1AP CU UE ID.
  bool allocate_cu_ue_id();

  bool create_ue_context(const f1ap_ue_context_setup_response& ue_ctxt_setup_resp);

  /// Send F1 UE Context Setup Request to DU.
  void send_ue_context_setup_request();

  /// Creates procedure result to send back to procedure caller.
  void create_ue_context_setup_result();

  /// Deletes UE context in CU-CP and removes F1AP UE context.
  void delete_ue_context(gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id);

  const f1ap_ue_context_setup_request request;
  f1ap_ue_context_list&               ue_ctxt_list;
  f1ap_du_processor_notifier&         du_processor_notifier;
  f1ap_message_notifier&              f1ap_notifier;
  srslog::basic_logger&               logger;
  optional<rrc_ue_transfer_context>   rrc_context; // Initialize new RRC with existing context.

  // The CU-allocated identifiers of the new UE (only valid if the DU response is positive).
  gnb_cu_ue_f1ap_id_t new_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
  ue_index_t          new_ue_index      = ue_index_t::invalid;

  f1ap_ue_context_setup_response response;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_setup_resp_s, asn1::f1ap::ue_context_setup_fail_s>
      transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran