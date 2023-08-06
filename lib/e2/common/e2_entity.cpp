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

#include "e2_entity.h"
#include "../e2sm/e2sm_kpm_asn1_packer.h"
#include "../e2sm/e2sm_kpm_impl.h"
#include "e2_impl.h"
#include "e2_subscription_manager_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include <memory>

using namespace srsran;
using namespace asn1::e2ap;

e2_entity::e2_entity(e2ap_configuration&           cfg_,
                     std::unique_ptr<e2_interface> decorated_e2_iface_,
                     task_executor&                task_exec_) :
  logger(srslog::fetch_basic_logger("E2")), cfg(cfg_), task_exec(task_exec_), main_ctrl_loop(128)
{
  decorated_e2_iface = std::move(decorated_e2_iface_);
}

e2_entity::e2_entity(e2ap_configuration&      cfg_,
                     e2_connection_client*    e2_client_,
                     e2_du_metrics_interface& e2_du_metrics_,
                     timer_factory            timers_,
                     task_executor&           task_exec_) :
  logger(srslog::fetch_basic_logger("E2AP")), cfg(cfg_), task_exec(task_exec_), main_ctrl_loop(128)
{
  e2_pdu_notifier   = e2_client_->handle_connection_request();
  subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2_pdu_notifier);

  if (cfg.e2sm_kpm_enabled) {
    auto e2sm_packer = std::make_unique<e2sm_kpm_asn1_packer>();
    auto e2sm_iface  = std::make_unique<e2sm_kpm_impl>(logger, *e2sm_packer, e2_du_metrics_);
    e2sm_handlers.push_back(std::move(e2sm_packer));
    subscription_mngr->add_e2sm_service(e2sm_kpm_asn1_packer::oid, std::move(e2sm_iface));
    subscription_mngr->add_ran_function_oid(1, e2sm_kpm_asn1_packer::oid);
  }

  decorated_e2_iface = std::make_unique<e2_impl>(cfg_, timers_, *e2_pdu_notifier, *subscription_mngr);
  e2_client_->connect_e2ap(e2_pdu_notifier.get(), this, this);
}

void e2_entity::start()
{
  if (not task_exec.execute([this]() {
        main_ctrl_loop.schedule([this](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Send E2AP Setup Request and await for E2AP setup response.
          CORO_AWAIT(decorated_e2_iface->start_initial_e2_setup_routine());

          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to initiate E2AP setup procedure");
  }
}

void e2_entity::stop()
{
  // placeholder_
}

async_task<e2_setup_response_message> e2_entity::handle_e2_setup_request(e2_setup_request_message& request)
{
  return decorated_e2_iface->handle_e2_setup_request(request);
}

async_task<e2_setup_response_message> e2_entity::start_initial_e2_setup_routine()
{
  return decorated_e2_iface->start_initial_e2_setup_routine();
}

void e2_entity::handle_connection_loss()
{
  task_exec.execute([this]() { decorated_e2_iface->handle_connection_loss(); });
}

void e2_entity::handle_message(const e2_message& msg)
{
  task_exec.execute([this, msg]() { decorated_e2_iface->handle_message(msg); });
}
