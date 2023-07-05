/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../rrc_ue_context.h"
#include "rrc_ue_event_manager.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

class rrc_reestablishment_procedure
{
public:
  rrc_reestablishment_procedure(rrc_ue_context_t&                     context_,
                                const ue_index_t                      old_ue_index_,
                                rrc_ue_reestablishment_proc_notifier& rrc_ue_notifier_,
                                rrc_ue_du_processor_notifier&         du_processor_notifier_,
                                rrc_ue_reestablishment_notifier&      cu_cp_notifier_,
                                rrc_ue_control_notifier&              ngap_ctrl_notifier_,
                                rrc_ue_event_manager&                 event_mng_,
                                srslog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Reestablishment Procedure"; }

private:
  /// Instruct DU processor to create SRB1 bearer.
  void create_srb1();

  /// \remark Send RRC Reestablishment, see section 5.3.7 in TS 36.331
  void send_rrc_reestablishment();

  rrc_ue_context_t&              context;
  const ue_index_t               old_ue_index = ue_index_t::invalid;
  const asn1::rrc_nr::pdcp_cfg_s srb1_pdcp_cfg;

  rrc_ue_reestablishment_proc_notifier& rrc_ue;                // handler to the parent RRC UE object
  rrc_ue_du_processor_notifier&         du_processor_notifier; // notifier to the DU processor
  rrc_ue_reestablishment_notifier&      cu_cp_notifier;        // notifier to the CU-CP
  rrc_ue_control_notifier&              ngap_ctrl_notifier;    // Control message notifier to the NGAP
  rrc_ue_event_manager&                 event_mng;             // event manager for the RRC UE entity
  srslog::basic_logger&                 logger;

  rrc_transaction                  transaction;
  eager_async_task<rrc_outcome>    task;
  bool                             context_modification_success = false;
  cu_cp_ue_context_release_request ue_context_release_request;

  const std::chrono::milliseconds rrc_reest_timeout_ms{
      1000}; // arbitrary timeout for RRC Reestablishment procedure, UE will be removed if timer fires
};

} // namespace srs_cu_cp
} // namespace srsran
