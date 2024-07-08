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

#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Modifies the RRC connection of a UE in RRC connected mode. It's used to establishe bearers, etc.
/// TODO Add seqdiag
class rrc_reconfiguration_procedure
{
public:
  rrc_reconfiguration_procedure(rrc_ue_context_t&                            context_,
                                const rrc_reconfiguration_procedure_request& args_,
                                rrc_ue_reconfiguration_proc_notifier&        rrc_ue_notifier_,
                                rrc_ue_context_update_notifier&              cu_cp_notifier_,
                                rrc_ue_cu_cp_ue_notifier&                    cu_cp_ue_notifier_,
                                rrc_ue_event_manager&                        event_mng_,
                                rrc_ue_srb_handler&                          srb_notifier_,
                                rrc_ue_logger&                               logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RRC Reconfiguration Procedure"; }

private:
  /// \remark Send RRC Reconfiguration, see section 5.3.5 in TS 38.331
  void send_rrc_reconfiguration();

  rrc_ue_context_t&                           context;
  const rrc_reconfiguration_procedure_request args;

  rrc_ue_reconfiguration_proc_notifier& rrc_ue;            // handler to the parent RRC UE object
  rrc_ue_context_update_notifier&       cu_cp_notifier;    // to release the UE if the reconfiguration fails
  rrc_ue_cu_cp_ue_notifier&             cu_cp_ue_notifier; // to schedule the UE release
  rrc_ue_event_manager&                 event_mng;         // event manager for the RRC UE entity
  rrc_ue_srb_handler&                   srb_notifier;      // For creating SRBs
  rrc_ue_logger&                        logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  bool procedure_result = false;
};

} // namespace srs_cu_cp
} // namespace srsran
