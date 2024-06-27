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

#include "../rrc_asn1_helpers.h"
#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of AS security keys in the RRC UE.
/// TODO Add seqdiag
class rrc_security_mode_command_procedure
{
public:
  rrc_security_mode_command_procedure(rrc_ue_context_t&                           context_,
                                      security::sec_selected_algos                security_algos_,
                                      rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
                                      rrc_ue_event_manager&                       ev_mng_,
                                      rrc_ue_logger&                              logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RRC Security Mode Command Procedure"; }

private:
  /// \remark Send RRC Security Mode Command, see section 5.3.3 in TS 38.331
  void send_rrc_security_mode_command();

  rrc_ue_context_t&                           context;
  security::sec_selected_algos                security_algos;
  rrc_ue_security_mode_command_proc_notifier& rrc_ue;    // handler to the parent RRC UE object
  rrc_ue_event_manager&                       event_mng; // event manager for the RRC UE entity
  rrc_ue_logger&                              logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  bool procedure_result = false;
};

} // namespace srs_cu_cp
} // namespace srsran
