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

#include "e2_entity.h"
#include "e2_impl.h"
#include "e2_subscription_manager_impl.h"
#include "srsran/e2/e2.h"

using namespace srsran;
using namespace asn1::e2ap;

e2_entity::e2_entity(e2_agent_dependencies&& dependencies) :
  logger(*dependencies.logger), cfg(dependencies.cfg), task_exec(*dependencies.task_exec), main_ctrl_loop(128)
{
  e2sm_mngr         = std::make_unique<e2sm_manager>(logger);
  subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2sm_mngr);

  for (auto& e2sm_module : dependencies.e2sm_modules) {
    auto [ran_func_id, oid, packer, interface] = std::move(e2sm_module);
    e2sm_handlers.push_back(std::move(packer));
    e2sm_mngr->add_e2sm_service(oid, std::move(interface));
    subscription_mngr->add_ran_function_oid(ran_func_id, oid);
  }

  e2ap = std::make_unique<e2_impl>(logger,
                                   cfg,
                                   *this,
                                   *dependencies.timers,
                                   *dependencies.e2_client,
                                   *subscription_mngr,
                                   *e2sm_mngr,
                                   *dependencies.task_exec);
}

void e2_entity::start()
{
  // Create e2ap (sctp) connection to RIC
  e2ap->handle_e2_tnl_connection_request();

  if (not task_exec.execute([this]() {
        main_ctrl_loop.schedule([this](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Send E2AP Setup Request and await for E2AP setup response.
          CORO_AWAIT(e2ap->start_initial_e2_setup_routine());

          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to initiate E2AP setup procedure");
  }
}

void e2_entity::stop()
{
  if (not task_exec.execute([this]() {
        main_ctrl_loop.schedule([this](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Send E2AP Setup Request and await for E2AP setup response.
          CORO_AWAIT(e2ap->handle_e2_disconnection_request());

          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to initiate E2AP setup procedure");
  }
}

void e2_entity::on_e2_disconnection()
{
  logger.info("E2 connection was closed.");
  // TODO: notify all components about the E2 disconnection (e.g., stop running indication procedures).
  subscription_mngr->stop();
}
