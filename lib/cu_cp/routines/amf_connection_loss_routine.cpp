/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "amf_connection_loss_routine.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_loss_routine::amf_connection_loss_routine(const amf_index_t                 amf_index_,
                                                         const cu_cp_configuration&        cu_cp_cfg_,
                                                         std::vector<plmn_identity>&       plmns_,
                                                         cu_cp_ue_context_release_handler& ue_release_handler_,
                                                         ue_manager&                       ue_mng_,
                                                         cu_cp_controller&                 controller_,
                                                         srslog::basic_logger&             logger_) :
  amf_index(amf_index_),
  cu_cp_cfg(cu_cp_cfg_),
  plmns(plmns_),
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

  release_ues();

  // Try to reconnect to AMF.
  controller.amf_connection_handler().reconnect_to_amf(amf_index, &ue_mng, cu_cp_cfg.ngap.amf_reconnection_retry_time);

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}

void amf_connection_loss_routine::release_ues()
{
  // Release all UEs with the PLMNs served by the disconnected AMF.
  for (const auto& plmn : plmns) {
    std::vector<cu_cp_ue*> ues = ue_mng.find_ues(plmn);

    for (const auto& ue : ues) {
      if (ue != nullptr) {
        logger.info("ue={}: Releasing UE (PLMN {}) due to N2 disconnection", ue->get_ue_index(), plmn);
        ue->get_task_sched().schedule_async_task(
            launch_async([this,
                          command = cu_cp_ue_context_release_command{ue->get_ue_index(),
                                                                     ngap_cause_transport_t::transport_res_unavailable,
                                                                     true}](coro_context<async_task<void>>& ctx) {
              CORO_BEGIN(ctx);
              // The outcome of the procedure is ignored, as we don't send anything to the (lost) AMF.
              CORO_AWAIT(ue_release_handler.handle_ue_context_release_command(command));
              CORO_RETURN();
            }));
      }
    }
  }
}
