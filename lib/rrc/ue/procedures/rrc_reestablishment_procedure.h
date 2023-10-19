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
  rrc_reestablishment_procedure(const asn1::rrc_nr::rrc_reest_request_s& request_,
                                rrc_ue_context_t&                        context_,
                                const byte_buffer&                       du_to_cu_container_,
                                up_resource_manager&                     up_resource_mng_,
                                rrc_ue_setup_proc_notifier&              rrc_setup_notifier_,
                                rrc_ue_reestablishment_proc_notifier&    rrc_ue_notifier_,
                                rrc_ue_srb_handler&                      srb_notifier_,
                                rrc_ue_du_processor_notifier&            du_processor_notifier_,
                                rrc_ue_reestablishment_notifier&         cu_cp_notifier_,
                                rrc_ue_control_notifier&                 ngap_ctrl_notifier_,
                                rrc_ue_nas_notifier&                     nas_notifier_,
                                rrc_ue_event_manager&                    event_mng_,
                                srslog::basic_logger&                    logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Reestablishment Procedure"; }

private:
  /// \brief Get and verify the reestablishment context of the reestablishing UE.
  bool get_and_verify_reestablishment_context();

  /// \brief Determined whether the Reestablishment Request is accepted or rejected.
  bool is_reestablishment_rejected();

  /// \brief Get and verify the ShortMAC-I and update the keys.
  bool verify_security_context();

  /// \brief Transfer the reestablishment context e.g. ue capabilities and update the security keys
  void transfer_reestablishment_context_and_update_keys();

  /// \brief Instruct DU processor to create SRB1 bearer.
  void create_srb1();

  /// \remark Send RRC Reestablishment, see section 5.3.7 in TS 36.331.
  void send_rrc_reestablishment();

  /// \brief Send UE Context Release Request.
  void send_ue_context_release_request(ue_index_t ue_index);

  async_task<void> handle_rrc_reestablishment_fallback();

  const asn1::rrc_nr::rrc_reest_request_s& reestablishment_request;
  rrc_ue_context_t&                        context;
  const byte_buffer&                       du_to_cu_container;
  up_resource_manager&                     up_resource_mng;
  rrc_ue_setup_proc_notifier&              rrc_ue_setup_notifier;
  rrc_ue_reestablishment_proc_notifier&    rrc_ue_reest_notifier; // handler to the parent RRC UE object
  rrc_ue_srb_handler&                      srb_notifier;          // for creating SRBs
  rrc_ue_du_processor_notifier&            du_processor_notifier; // notifier to the DU processor
  rrc_ue_reestablishment_notifier&         cu_cp_notifier;        // notifier to the CU-CP
  rrc_ue_control_notifier&                 ngap_ctrl_notifier;    // Control message notifier to the NGAP
  rrc_ue_nas_notifier&                     nas_notifier;          // notifier to the NGAP
  rrc_ue_event_manager&                    event_mng;             // event manager for the RRC UE entity
  srslog::basic_logger&                    logger;

  const asn1::rrc_nr::pdcp_cfg_s   srb1_pdcp_cfg;
  rrc_transaction                  transaction;
  eager_async_task<rrc_outcome>    task;
  rrc_reestablishment_ue_context_t reestablishment_context;
  bool                             context_transfer_success     = false;
  bool                             context_modification_success = false;
  cu_cp_ue_context_release_request ue_context_release_request;
};

} // namespace srs_cu_cp
} // namespace srsran
