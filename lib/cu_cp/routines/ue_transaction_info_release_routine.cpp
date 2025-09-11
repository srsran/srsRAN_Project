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

#include "ue_transaction_info_release_routine.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_transaction_info_release_routine::ue_transaction_info_release_routine(
    const f1_ue_transaction_info_loss_event& loss_event_,
    ue_manager&                              ue_mng_,
    ngap_repository&                         ngap_db_,
    cu_cp_ue_removal_handler&                ue_rem_handler_,
    srslog::basic_logger&                    logger_) :
  loss_event(loss_event_), ue_mng(ue_mng_), ngap_db(ngap_db_), ue_rem_handler(ue_rem_handler_), logger(logger_)
{
}

void ue_transaction_info_release_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Prepare NG Reset messages for each PLMN.
  prepare_ng_reset_messages();

  // Run NG Reset to signal to the AMF the reset UEs.
  for (plmn_id_it = ng_reset_per_plmn.begin(); plmn_id_it != ng_reset_per_plmn.end(); ++plmn_id_it) {
    ngap = ngap_db.find_ngap(plmn_id_it->first);
    if (ngap == nullptr) {
      logger.debug("NGAP not found for PLMN={}. Skipping NGReset", plmn_id_it->first);
      continue;
    }

    CORO_AWAIT(ngap->handle_ng_reset_message(plmn_id_it->second));
  }

  // Launch removal procedure for the provided UEs.
  ues_remaining_count = loss_event.ues_lost.size();
  for (ue_index_t ue_idx : loss_event.ues_lost) {
    launch_ue_removal(ue_idx);
  }

  // Wait for all UEs to be removed.
  if (ues_remaining_count != 0) {
    CORO_AWAIT(all_ues_reset);
  }

  CORO_RETURN();
}

void ue_transaction_info_release_routine::prepare_ng_reset_messages()
{
  for (const auto& ue_idx : loss_event.ues_lost) {
    auto* ue = ue_mng.find_du_ue(ue_idx);
    if (ue == nullptr) {
      logger.debug("ue={}: UE not found in UE manager. Skipping NGReset", ue_idx);
      continue;
    }

    if (ng_reset_per_plmn.find(ue->get_ue_context().plmn) == ng_reset_per_plmn.end()) {
      // Create a new entry for the PLMN.
      ngap_cause_t reset_cause                     = ngap_cause_radio_network_t::radio_conn_with_ue_lost;
      ng_reset_per_plmn[ue->get_ue_context().plmn] = cu_cp_ng_reset{reset_cause, false, {}};
    }

    // Add reset message to the PLMN list.
    ng_reset_per_plmn[ue->get_ue_context().plmn].ues_to_reset.push_back(ue_idx);
  }
}

void ue_transaction_info_release_routine::launch_ue_removal(ue_index_t ue_idx)
{
  // Create task to be called after a UE removal.
  auto on_ue_removal = make_scope_exit([this]() {
    // If it is the last UE to be removed, notify the completion.
    if (--ues_remaining_count == 0) {
      all_ues_reset.set();
    }
  });

  auto* ue_it = ue_mng.find_ue(ue_idx);
  if (ue_it == nullptr) {
    // UE not found. The "on_ue_removal" will still decrement the counter.
    return;
  }

  // Cancel any pending transactions.
  ue_rem_handler.handle_pending_ue_task_cancellation(ue_idx);

  async_task<void> ue_rem_task =
      launch_async([this, ue_idx, on_ue_removal = std::move(on_ue_removal)](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);

        // Launch UE removal routine and wait for its completion.
        CORO_AWAIT(ue_rem_handler.handle_ue_removal_request(ue_idx));

        CORO_RETURN();
      });
  ue_mng.get_task_sched().handle_ue_async_task(ue_idx, std::move(ue_rem_task));
}
