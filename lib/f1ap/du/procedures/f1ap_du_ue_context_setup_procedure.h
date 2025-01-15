/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../f1ap_du_context.h"
#include "../ue_context/f1ap_du_ue.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"

namespace srsran {
namespace srs_du {

class f1ap_du_ue_manager;

/// This procedure handles UE CONTEXT SETUP REQUEST as per TS38.473, Section 8.3.1.
class f1ap_du_ue_context_setup_procedure
{
public:
  f1ap_du_ue_context_setup_procedure(const asn1::f1ap::ue_context_setup_request_s& msg,
                                     f1ap_du_ue_manager&                           ue_mng_,
                                     f1ap_du_configurator&                         du_mng_,
                                     du_ue_index_t                                 ue_index_,
                                     const f1ap_du_context&                        ctxt_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  /// Initiates UE Configuration in the DU.
  async_task<f1ap_ue_context_update_response> request_du_ue_config();

  /// Sends UE Context Setup Response to CU.
  void send_ue_context_setup_response();

  /// Sends UE Context Setup Failure to CU.
  void send_ue_context_setup_failure();

  /// Handles the RRC container.
  async_task<bool> handle_rrc_container();

  /// Gets the cell index that matches the given NR-CGI from the F1AP DU context.
  expected<unsigned> get_cell_index_from_nr_cgi(nr_cell_global_id_t nr_cgi) const;

  /// Returns the name of this procedure.
  const char* name() const { return "UE Context Setup"; }

  const asn1::f1ap::ue_context_setup_request_s msg;
  f1ap_du_ue_manager&                          ue_mng;
  f1ap_du_configurator&                        du_mng;
  const du_ue_index_t                          ue_index;
  srslog::basic_logger&                        logger;

  f1ap_du_ue* ue = nullptr;

  const f1ap_du_context& du_ctxt;
  expected<unsigned>     sp_cell_index = 0;

  std::optional<f1ap_ue_context_creation_response> du_ue_create_response;
  f1ap_ue_context_update_response                  du_ue_cfg_response;
};
} // namespace srs_du
} // namespace srsran
