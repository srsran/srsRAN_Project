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
  rrc_ue_event_manager&                 event_mng;             // event manager for the RRC UE entity
  srslog::basic_logger&                 logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  const std::chrono::milliseconds rrc_reest_timeout_ms{
      1000}; // arbitrary timeout for RRC Reestablishment procedure, UE will be removed if timer fires
};

} // namespace srs_cu_cp
} // namespace srsran
