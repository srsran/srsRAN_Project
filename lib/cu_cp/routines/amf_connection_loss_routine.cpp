/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amf_connection_loss_routine.h"
#include "cell_deactivation_routine.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_loss_routine::amf_connection_loss_routine(const amf_index_t                 amf_index_,
                                                         const cu_cp_configuration&        cu_cp_cfg_,
                                                         std::vector<plmn_identity>&       plmns_,
                                                         du_processor_repository&          du_db_,
                                                         cu_cp_ue_context_release_handler& ue_release_handler_,
                                                         ue_manager&                       ue_mng_,
                                                         cu_cp_controller&                 controller_,
                                                         srslog::basic_logger&             logger_) :
  amf_index(amf_index_),
  cu_cp_cfg(cu_cp_cfg_),
  plmns(plmns_),
  du_db(du_db_),
  ue_release_handler(ue_release_handler_),
  ue_mng(ue_mng_),
  controller(controller_),
  logger(logger_)
{
}

void amf_connection_loss_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  // Notify AMF connection manager about the disconnection.
  controller.amf_connection_handler().handle_amf_connection_loss(amf_index);

  // Stop accepting new UEs for the given PLMNs.
  ue_mng.add_blocked_plmns(plmns);

  // Deactivate the cells served by the disconnected AMF.
  CORO_AWAIT(launch_async<cell_deactivation_routine>(cu_cp_cfg, plmns, du_db, ue_release_handler, ue_mng, logger));

  // Try to reconnect to AMF.
  controller.amf_connection_handler().reconnect_to_amf(amf_index, &ue_mng, cu_cp_cfg.ngap.amf_reconnection_retry_time);

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}
