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

#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class e1ap_cu_cp_notifier;
class e1ap_ue_context_list;

/// \brief Handle the E1 Release Procedure as per TS 38.463 section 8.2.7.
class e1_release_procedure
{
public:
  e1_release_procedure(const asn1::e1ap::e1_release_request_s& request_,
                       cu_up_index_t                           cu_up_index_,
                       e1ap_message_notifier&                  pdu_notifier_,
                       e1ap_cu_cp_notifier&                    cu_cp_notifier_,
                       e1ap_ue_context_list&                   ue_list_,
                       srslog::basic_logger&                   logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "E1 Release"; }
  void        send_e1_release_response();

  const asn1::e1ap::e1_release_request_s request;
  cu_up_index_t                          cu_up_index;
  e1ap_message_notifier&                 pdu_notifier;
  e1ap_cu_cp_notifier&                   cu_cp_notifier;
  e1ap_ue_context_list&                  ue_list;
  srslog::basic_logger&                  logger;

  // List of UEs to be released.
  std::vector<ue_index_t> ues_to_release;
};

} // namespace srs_cu_cp
} // namespace srsran
