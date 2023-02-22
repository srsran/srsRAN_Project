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

#include "../common/f1ap_du_test_messages.h"
#include "../common/test_helpers.h"
#include "lib/du_manager/converters/f1ap_configuration_helpers.h"
#include "lib/f1ap/common/f1ap_asn1_utils.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "srsran/support/async/async_task_loop.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_du {

class dummy_f1ap_du_configurator : public f1ap_du_configurator
{
public:
  struct dummy_ue_task_sched : public f1ap_ue_task_scheduler {
    dummy_f1ap_du_configurator* parent;

    explicit dummy_ue_task_sched(dummy_f1ap_du_configurator* parent_) : parent(parent_) {}

    unique_timer create_timer() override { return parent->timers.create_unique_timer(); }

    /// \brief Schedule Async Task respective to a given UE.
    void schedule_async_task(async_task<void>&& task) override { parent->task_loop.schedule(std::move(task)); }
  };

  timer_manager&       timers;
  async_task_sequencer task_loop;
  dummy_ue_task_sched  ue_sched;
  f1ap_interface*      f1ap;

  // DU manager -> F1AP.
  f1ap_ue_configuration_request            next_ue_cfg_req;
  optional<f1ap_ue_configuration_response> last_ue_cfg_response;

  // F1-C procedures.
  optional<f1ap_ue_context_update_request> last_ue_context_update_req;
  f1ap_ue_context_update_response          next_ue_context_update_response;

  explicit dummy_f1ap_du_configurator(timer_manager& timers_) : timers(timers_), task_loop(128), ue_sched(this) {}

  void connect(f1ap_interface& f1ap_) { f1ap = &f1ap_; }

  timer_manager& get_timer_manager() override { return timers; }

  void schedule_async_task(async_task<void>&& task) override { task_loop.schedule(std::move(task)); }

  async_task<f1ap_ue_context_update_response>
  request_ue_context_update(const f1ap_ue_context_update_request& request) override
  {
    last_ue_context_update_req = request;
    return launch_async([this](coro_context<async_task<f1ap_ue_context_update_response>>& ctx) {
      CORO_BEGIN(ctx);
      last_ue_cfg_response = f1ap->handle_ue_configuration_request(next_ue_cfg_req);
      CORO_RETURN(next_ue_context_update_response);
    });
  }

  async_task<void> request_ue_removal(const f1ap_ue_delete_request& request) override
  {
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  /// \brief Retrieve task scheduler specific to a given UE.
  f1ap_ue_task_scheduler& get_ue_handler(du_ue_index_t ue_index) override { return ue_sched; }
};

class dummy_ue_executor_mapper : public du_high_ue_executor_mapper
{
public:
  dummy_ue_executor_mapper(task_executor& exec_) : exec(exec_) {}

  task_executor& rebind_executor(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    return executor(ue_index);
  }
  task_executor& executor(du_ue_index_t ue_index) override { return exec; }

  task_executor& exec;
};

f1_setup_request_message generate_f1_setup_request_message();

/// \brief Generate F1AP ASN.1 DRB AM Setup configuration.
asn1::f1ap::drbs_to_be_setup_item_s generate_drb_am_setup_item(drb_id_t drbid);

/// \brief Generate an F1AP UE Context Setup Request message with specified list of DRBs.
f1ap_message generate_ue_context_setup_request(const std::initializer_list<drb_id_t>& drbs_to_add);

/// \brief Generate F1AP ASN.1 DRB AM Setup configuration.
asn1::f1ap::drbs_to_be_setup_mod_item_s generate_drb_am_mod_item(drb_id_t drbid);

/// \brief Generate an F1AP UE Context Modification Request message with specified list of DRBs.
f1ap_message generate_ue_context_modification_request(const std::initializer_list<drb_id_t>& drbs_to_add);

class dummy_f1c_rx_sdu_notifier : public f1c_rx_sdu_notifier
{
public:
  byte_buffer last_pdu;

  void on_new_sdu(byte_buffer pdu) override { last_pdu = std::move(pdu); }
};

class dummy_f1u_rx_sdu_notifier : public f1u_rx_sdu_notifier
{
public:
  byte_buffer        last_pdu;
  optional<uint32_t> last_pdu_sn;
  optional<uint32_t> last_discard_sn;

  void on_new_sdu(pdcp_tx_pdu sdu) override
  {
    last_pdu    = std::move(sdu.buf);
    last_pdu_sn = sdu.pdcp_sn;
  }

  void on_discard_sdu(uint32_t pdcp_sn) override { last_discard_sn = pdcp_sn; }
};

/// Fixture class for F1AP
class f1ap_du_test : public ::testing::Test
{
protected:
  struct f1c_test_bearer {
    srb_id_t                  srb_id = srb_id_t::nulltype;
    dummy_f1c_rx_sdu_notifier rx_sdu_notifier;
    f1c_bearer*               bearer = nullptr;
  };
  struct f1u_test_bearer {
    drb_id_t                  drb_id = drb_id_t::invalid;
    dummy_f1u_rx_sdu_notifier rx_sdu_notifier;
    f1u_bearer*               bearer = nullptr;
  };
  struct ue_test_context {
    du_ue_index_t                                ue_index;
    rnti_t                                       crnti;
    slotted_array<f1c_test_bearer, MAX_NOF_SRBS> f1c_bearers;
    slotted_array<f1u_test_bearer, MAX_NOF_DRBS> f1u_bearers;
  };

  f1ap_du_test();
  ~f1ap_du_test();

  /// \brief Run F1 Setup Procedure to completion.
  void run_f1_setup_procedure();

  /// \brief Create new UE in F1AP.
  ue_test_context* run_f1ap_ue_create(du_ue_index_t ue_index);

  void run_ue_context_setup_procedure(du_ue_index_t ue_index, const f1ap_message& msg);

  /// \brief Update UE config in F1AP.
  f1ap_ue_configuration_response update_f1ap_ue_config(du_ue_index_t                   ue_index,
                                                       std::initializer_list<srb_id_t> srbs,
                                                       std::initializer_list<drb_id_t> drbs);

  /// Notifier for messages coming out from F1AP to Gateway.
  f1ap_null_notifier msg_notifier = {};

  timer_manager                   timers;
  dummy_f1ap_du_configurator      f1ap_du_cfg_handler{timers};
  manual_task_worker              ctrl_worker{128};
  dummy_ue_executor_mapper        ue_exec_mapper{ctrl_worker};
  std::unique_ptr<f1ap_interface> f1ap;

  /// Storage of UE context related to the current unit test.
  slotted_array<ue_test_context, MAX_NOF_DU_UES> test_ues;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

} // namespace srs_du
} // namespace srsran
