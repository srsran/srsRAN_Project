/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/du_manager/converters/f1c_configuration_helpers.h"
#include "lib/f1c/common/f1ap_asn1_utils.h"
#include "unittests/f1c/common/test_helpers.h"
#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/f1c/common/f1c_common.h"
#include "srsgnb/f1c/du/f1ap_du.h"
#include "srsgnb/f1c/du/f1ap_du_factory.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_du {

class dummy_f1c_du_configurator : public f1c_du_configurator
{
public:
  struct dummy_ue_task_sched : public f1c_ue_task_scheduler {
    dummy_f1c_du_configurator* parent;

    explicit dummy_ue_task_sched(dummy_f1c_du_configurator* parent_) : parent(parent_) {}

    unique_timer create_timer() override { return parent->timers.create_unique_timer(); }

    /// \brief Schedule Async Task respective to a given UE.
    void schedule_async_task(async_task<void>&& task) override { parent->task_loop.schedule(std::move(task)); }
  };

  timer_manager&       timers;
  async_task_sequencer task_loop;
  dummy_ue_task_sched  ue_sched;

  optional<f1ap_ue_config_update_request> last_ue_config_update_req;
  f1ap_ue_config_update_response          next_ue_config_update_response;

  explicit dummy_f1c_du_configurator(timer_manager& timers_) : timers(timers_), task_loop(128), ue_sched(this) {}

  timer_manager& get_timer_manager() override { return timers; }

  void schedule_async_task(async_task<void>&& task) override { task_loop.schedule(std::move(task)); }

  async_task<f1ap_ue_config_update_response>
  request_ue_config_update(const f1ap_ue_config_update_request& request) override
  {
    last_ue_config_update_req = request;
    return launch_async([this](coro_context<async_task<f1ap_ue_config_update_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(next_ue_config_update_response);
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
  f1c_ue_task_scheduler& get_ue_handler(du_ue_index_t ue_index) override { return ue_sched; }
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

f1_setup_request_message generate_f1_setup_request_message()
{
  f1_setup_request_message           request_msg  = {};
  du_setup_params                    setup_params = {};
  du_cell_config                     cell         = config_helpers::make_default_du_cell_config();
  std::vector<const du_cell_config*> cells        = {&cell};
  fill_asn1_f1_setup_request(request_msg.msg, setup_params, cells);

  return request_msg;
}

f1c_message generate_f1_setup_response_message(unsigned transaction_id)
{
  f1c_message f1_setup_response = {};

  f1_setup_response.pdu.set_successful_outcome();
  f1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_res                 = f1_setup_response.pdu.successful_outcome().value.f1_setup_resp();
  setup_res->transaction_id.value = transaction_id;
  setup_res->gnb_cu_name_present  = true;
  setup_res->gnb_cu_name.value.from_string("srsCU");
  setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);

  return f1_setup_response;
}

f1c_message generate_f1_setup_failure_message(unsigned transaction_id)
{
  f1c_message f1_setup_failure = {};

  f1_setup_failure.pdu.set_unsuccessful_outcome();
  f1_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_fail                 = f1_setup_failure.pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->transaction_id.value = transaction_id;
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() =
      asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  setup_fail->time_to_wait_present     = false;
  setup_fail->crit_diagnostics_present = false;
  // add critical diagnostics

  return f1_setup_failure;
}

f1c_message generate_f1_setup_failure_message_with_time_to_wait(unsigned                   transaction_id,
                                                                asn1::f1ap::time_to_wait_e time_to_wait)
{
  f1c_message f1_setup_failure = generate_f1_setup_failure_message(transaction_id);

  auto& setup_fail                 = f1_setup_failure.pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = time_to_wait;

  return f1_setup_failure;
}

f1c_message generate_f1_dl_rrc_message_transfer(const byte_buffer& rrc_container)
{
  f1c_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DLRRC_MSG_TRANSFER);
  auto& dl_msg                      = msg.pdu.init_msg().value.dlrrc_msg_transfer();
  dl_msg->gnb_cu_ue_f1_ap_id->value = 0;
  dl_msg->gnb_du_ue_f1_ap_id->value = 0;
  dl_msg->srbid->value              = 1;
  dl_msg->rrc_container->resize(rrc_container.length());
  std::copy(rrc_container.begin(), rrc_container.end(), dl_msg->rrc_container->begin());

  return msg;
}

asn1::f1ap::drbs_to_be_setup_item_s generate_drb_am_config(drb_id_t drbid)
{
  using namespace asn1::f1ap;

  drbs_to_be_setup_item_s drb;
  drb.drbid = drb_id_to_uint(drbid);
  drb.qo_sinfo.set_choice_ext().load_info_obj(ASN1_F1AP_ID_DRB_INFO);
  auto& drb_info                                                            = drb.qo_sinfo.choice_ext()->drb_info();
  drb_info.drb_qos.qo_s_characteristics.set_non_dynamic_minus5_qi().five_qi = 8;
  drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level                    = 1;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap.value =
      pre_emption_cap_opts::shall_not_trigger_pre_emption;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      pre_emption_vulnerability_opts::not_pre_emptable;
  drb_info.drb_qos.reflective_qos_attribute_present = true;
  drb_info.drb_qos.reflective_qos_attribute.value =
      qo_sflow_level_qos_params_s::reflective_qos_attribute_opts::subject_to;
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value = rlc_mode_opts::rlc_am;

  return drb;
}

f1c_message generate_f1_ue_context_setup_request(const std::initializer_list<drb_id_t> drbs_to_add)
{
  using namespace asn1::f1ap;
  f1c_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& dl_msg    = msg.pdu.init_msg().value.ue_context_setup_request();
  dl_msg->gnb_cu_ue_f1_ap_id->value     = 0;
  dl_msg->gnb_du_ue_f1_ap_id->value     = 0;
  dl_msg->srbs_to_be_setup_list_present = true;
  dl_msg->srbs_to_be_setup_list.value.resize(1);
  dl_msg->srbs_to_be_setup_list.value[0].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
  srbs_to_be_setup_item_s& srb2 = dl_msg->srbs_to_be_setup_list.value[0]->srbs_to_be_setup_item();
  srb2.srbid                    = 2;

  dl_msg->drbs_to_be_setup_list_present = drbs_to_add.size() > 0;
  dl_msg->drbs_to_be_setup_list.value.resize(drbs_to_add.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_add) {
    dl_msg->drbs_to_be_setup_list.value[count].load_info_obj(ASN1_F1AP_ID_DRB_INFO);
    dl_msg->drbs_to_be_setup_list.value[count]->drbs_to_be_setup_item() = generate_drb_am_config(drbid);
    ++count;
  }

  return msg;
}

class dummy_f1_tx_pdu_notifier : public f1_tx_pdu_notifier
{
public:
  byte_buffer last_pdu;

  void on_tx_pdu(byte_buffer pdu) override { last_pdu = std::move(pdu); }
};

/// Fixture class for F1AP
class f1ap_du_test : public ::testing::Test
{
protected:
  struct ue_test_context {
    std::unique_ptr<dummy_f1_tx_pdu_notifier> tx_pdu_notif;
    std::vector<f1_bearer*>                   bearers;
  };

  f1ap_du_test()
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    f1ap = create_f1ap(msg_notifier, f1c_du_cfg_handler, ctrl_worker, ue_exec_mapper);
  }

  ~f1ap_du_test()
  {
    // flush logger after each test
    srslog::flush();
  }

  void run_f1_setup_procedure()
  {
    // Action 1: Launch F1 setup procedure
    f1_setup_request_message request_msg = generate_f1_setup_request_message();
    test_logger.info("Launch f1 setup request procedure...");
    async_task<f1_setup_response_message>         t = f1ap->handle_f1ap_setup_request(request_msg);
    lazy_task_launcher<f1_setup_response_message> t_launcher(t);

    // Action 2: F1 setup response received.
    unsigned    transaction_id    = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
    f1c_message f1_setup_response = generate_f1_setup_response_message(transaction_id);
    test_logger.info("Injecting F1SetupResponse");
    f1ap->handle_message(f1_setup_response);
  }

  ue_test_context* run_f1_ue_create(du_ue_index_t ue_index)
  {
    auto                                   notif = std::make_unique<dummy_f1_tx_pdu_notifier>();
    srsgnb::srs_du::f1ap_ue_create_request msg;
    msg.ue_index   = ue_index;
    msg.cell_index = to_du_cell_index(0);
    msg.c_rnti     = to_rnti(0x4601 + ue_index);
    msg.du_cu_rrc_container.append({0x1, 0x2, 0x3});
    msg.srbs_to_add.resize(1);
    msg.srbs_to_add[0].srb_id          = srb_id_t::srb1;
    msg.srbs_to_add[0].f1_tx_pdu_notif = notif.get();
    test_logger.info("Creating ueId={}", msg.ue_index);
    f1ap_ue_create_response resp = f1ap->handle_ue_creation_request(msg);
    if (resp.result) {
      test_ues.emplace(ue_index);
      test_ues[ue_index].bearers      = resp.bearers_added;
      test_ues[ue_index].tx_pdu_notif = std::move(notif);
      return &test_ues[ue_index];
    }
    return nullptr;
  }

  /// Notifier for messages coming out from F1c to Gateway.
  f1c_null_notifier msg_notifier = {};

  timer_manager                 timers;
  dummy_f1c_du_configurator     f1c_du_cfg_handler{timers};
  manual_task_worker            ctrl_worker{128};
  dummy_ue_executor_mapper      ue_exec_mapper{ctrl_worker};
  std::unique_ptr<f1_interface> f1ap;

  /// Storage of UE context related to the current unit test.
  slotted_array<ue_test_context, MAX_NOF_DU_UES> test_ues;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

} // namespace srs_du
} // namespace srsgnb
