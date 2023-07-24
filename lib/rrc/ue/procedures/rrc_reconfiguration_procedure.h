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

#include "../rrc_ue_context.h"
#include "rrc_ue_event_manager.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/rrc/rrc_du.h"
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
                                rrc_ue_event_manager&                        ev_mng_,
                                rrc_ue_du_processor_notifier&                du_processor_,
                                srslog::basic_logger&                        logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RRC Reconfiguration Procedure"; }

private:
  /// \remark Send RRC Reconfiguration, see section 5.3.5 in TS 38.331
  void send_rrc_reconfiguration();

  rrc_ue_context_t&                           context;
  const rrc_reconfiguration_procedure_request args;

  rrc_ue_reconfiguration_proc_notifier& rrc_ue;                // handler to the parent RRC UE object
  rrc_ue_event_manager&                 event_mng;             // event manager for the RRC UE entity
  rrc_ue_du_processor_notifier&         du_processor_notifier; // For creating SRBs
  srslog::basic_logger&                 logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  const std::chrono::milliseconds timeout_ms{
      1000}; // arbitrary timeout for RRC Reconfig procedure, UE will be removed if timer fires
  bool procedure_result = false;
};

} // namespace srs_cu_cp
} // namespace srsran
