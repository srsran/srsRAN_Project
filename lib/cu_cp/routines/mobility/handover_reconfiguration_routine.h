/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
