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

#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the creation of SRBs in the RRC UE.
///  \startuml
///    queue DU
///    participant F1AP
///    participant PDCP
///    participant RRC
///    participant "DU processor" as DUMNG
///    DU->F1AP: initial UL RRC message
///    activate F1AP
///    F1AP->DUMNG: ue_creation_request
///    activate DUMNG
///    Note over DUMNG: Allocate UE index
///    Note over DUMNG: Create UE object
///    Note over DUMNG: Create SRB0 notifiers
///    DUMNG->RRC: create_ue (with SRB0 handle)
///    DUMNG-->F1AP: return of "ue_creation_request" (with UE index and SRB0 handle)
///    deactivate DUMNG
///    Note over F1AP: Allocate F1AP-CU-UE-ID
///    Note over F1AP: Association of UE index\nwith F1AP-CU-UE-ID
///    F1AP->RRC: notify_init_ul_rrc_msg
///    deactivate F1AP
///    Note over RRC: Decide RRC Setup vs Reject
///    activate RRC
///    Note over RRC: Create SRB1 (SRB1)
///    Note over RRC: Initiate RRCSetup procedure
///    RRC->F1AP: send_rrc_setup
///    F1AP->DU: DL RRC Message Transfer
///    DU->F1AP: UL RRC Message Transfer
///    F1AP->RRC: rrc_setup_complete
///    deactivate RRC
///    Note over RRC: Decide next step
///  \enduml
class rrc_setup_procedure
{
public:
  rrc_setup_procedure(rrc_ue_context_t&                          context_,
                      const asn1::rrc_nr::establishment_cause_e& cause_,
                      const byte_buffer&                         du_to_cu_container_,
                      rrc_ue_setup_proc_notifier&                rrc_ue_notifier_,
                      rrc_ue_srb_handler&                        srb_notifier_,
                      rrc_ue_nas_notifier&                       nas_notifier_,
                      rrc_ue_event_manager&                      ev_mng_,
                      rrc_ue_logger&                             logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Setup Procedure"; }

private:
  /// Instruct DU processor to create SRB1 bearer.
  void create_srb1();

  /// \remark Send RRC Setup, see section 5.3.3 in TS 36.331
  void send_rrc_setup();

  /// \remark Forward the Initial UE Message to the NGAP
  void send_initial_ue_msg(const asn1::rrc_nr::rrc_setup_complete_s& rrc_setup_complete_msg);

  rrc_ue_context_t&                         context;
  const asn1::rrc_nr::establishment_cause_e cause;
  const byte_buffer&                        du_to_cu_container;

  rrc_ue_setup_proc_notifier& rrc_ue;       // handler to the parent RRC UE object
  rrc_ue_srb_handler&         srb_notifier; // for creation of SRBs
  rrc_ue_nas_notifier&        nas_notifier; // notifier to the NGAP
  rrc_ue_event_manager&       event_mng;    // event manager for the RRC UE entity
  rrc_ue_logger&              logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;
};

} // namespace srs_cu_cp
} // namespace srsran
