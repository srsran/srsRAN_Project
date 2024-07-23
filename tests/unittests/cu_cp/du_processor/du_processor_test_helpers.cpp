/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "du_processor_test_helpers.h"
#include "../du_processor_test_messages.h"
#include "lib/cu_cp/cu_cp_controller/common_task_scheduler.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

namespace {

class dummy_task_sched final : public common_task_scheduler
{
public:
  bool schedule_async_task(async_task<void> task) override { return task_sched.schedule(std::move(task)); }

private:
  fifo_async_task_scheduler task_sched{32};
};

struct dummy_cu_cp_ue_admission_controller : public cu_cp_ue_admission_controller {
  bool request_ue_setup() const override { return true; }
};

struct dummy_cu_cp_measurement_handler : public cu_cp_measurement_handler {
  std::optional<rrc_meas_cfg>
  handle_measurement_config_request(ue_index_t                  ue_index,
                                    nr_cell_identity            nci,
                                    std::optional<rrc_meas_cfg> current_meas_config = {}) override
  {
    return std::nullopt;
  };
  void handle_measurement_report(const ue_index_t ue_index, const rrc_meas_results& meas_results) override {}
};

struct dummy_cu_cp_ue_removal_handler : public cu_cp_ue_removal_handler {
  async_task<void> handle_ue_removal_request(ue_index_t ue_index) override { return launch_no_op_task(); }
  void             handle_pending_ue_task_cancellation(ue_index_t ue_index) override {}
};

struct dummy_cu_cp_rrc_ue_interface : public cu_cp_rrc_ue_interface {
  rrc_ue_reestablishment_context_response
  handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) override
  {
    return {};
  }
  async_task<bool> handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index) override
  {
    return launch_no_op_task(true);
  }
  void             handle_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) override {}
  void             handle_rrc_reestablishment_complete(ue_index_t old_ue_index) override {}
  async_task<bool> handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index) override
  {
    return launch_no_op_task(true);
  }
  async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) override
  {
    return launch_no_op_task();
  }
};

struct dummy_cu_cp_du_event_handler : public cu_cp_du_event_handler {
public:
  dummy_cu_cp_du_event_handler(ue_manager& ue_mng_) : ue_mng(ue_mng_) {}

  void handle_du_processor_creation(du_index_t                       du_index,
                                    f1ap_ue_context_removal_handler& f1ap_handler,
                                    f1ap_statistics_handler&         f1ap_statistic_handler,
                                    rrc_ue_handler&                  rrc_handler,
                                    rrc_du_statistics_handler&       rrc_statistic_handler) override
  {
  }
  void handle_du_processor_removal(du_index_t du_index) override {}
  void handle_rrc_ue_creation(ue_index_t ue_index, rrc_ue_interface& rrc_ue) override
  {
    ue_mng.get_rrc_ue_cu_cp_adapter(ue_index).connect_cu_cp(rrc_ue_handler,
                                                            ue_rem_handler,
                                                            ue_admission_handler,
                                                            ue_mng.find_ue(ue_index)->get_up_resource_manager(),
                                                            meas_handler);
  }
  byte_buffer handle_target_cell_sib1_required(du_index_t du_index, nr_cell_global_id_t cgi) override { return {}; }
  async_task<void> handle_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev) override
  {
    return launch_no_op_task();
  }

private:
  ue_manager&                         ue_mng;
  dummy_cu_cp_ue_admission_controller ue_admission_handler;
  dummy_cu_cp_measurement_handler     meas_handler;
  dummy_cu_cp_ue_removal_handler      ue_rem_handler;
  dummy_cu_cp_rrc_ue_interface        rrc_ue_handler;
};

} // namespace

du_processor_test::du_processor_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    return cucfg;
  }()),
  common_task_sched(std::make_unique<dummy_task_sched>()),
  du_cfg_mgr{cu_cp_cfg.node.gnb_id, cu_cp_cfg.node.plmn}
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create and start DU processor
  du_processor_config_t du_cfg = {uint_to_du_index(0),
                                  cu_cp_cfg,
                                  srslog::fetch_basic_logger("CU-CP"),
                                  &du_conn_notifier,
                                  du_cfg_mgr.create_du_handler()};
  du_processor_obj             = create_du_processor(std::move(du_cfg),
                                         cu_cp_notifier,
                                         f1ap_pdu_notifier,
                                         rrc_ue_ngap_notifier,
                                         rrc_ue_ngap_notifier,
                                         rrc_du_cu_cp_notifier,
                                         *common_task_sched,
                                         ue_mng);

  cu_cp_event_handler = std::make_unique<dummy_cu_cp_du_event_handler>(ue_mng);
  cu_cp_notifier.attach_handler(&*cu_cp_event_handler, nullptr);
}

du_processor_test::~du_processor_test()
{
  // flush logger after each test
  srslog::flush();
}
