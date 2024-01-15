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

#include "../../ue_manager_impl.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the handover of a UE between two different DUs.
/// TODO Add seqdiag
class handover_reconfiguration_routine
{
public:
  handover_reconfiguration_routine(const rrc_reconfiguration_procedure_request& request_,
                                   du_ue&                                       source_ue_,
                                   du_ue&                                       target_ue_,
                                   srslog::basic_logger&                        logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Handover reconfiguration routine"; }

private:
  /// \remark Send RRC Reconfiguration, see section 5.3.5 in TS 38.331
  /// \returns The transaction ID of the RRC Reconfiguration.
  uint8_t send_rrc_reconfiguration();

  // (sub-)routine requests
  const rrc_reconfiguration_procedure_request request;

  du_ue& source_ue; // UE in the source DU
  du_ue& target_ue; // UE in target DU

  srslog::basic_logger& logger;

  // (sub-)routine results
  uint8_t transaction_id;
  bool    procedure_result = false;
};

} // namespace srs_cu_cp
} // namespace srsran
