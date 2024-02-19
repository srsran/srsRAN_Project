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

#include "cu_cp_test_environment.h"
#include "tests/unittests/cu_cp/test_doubles/mock_cu_up.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg_ies.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/support/executors/task_worker.h"

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_test_environment::worker_manager
{
  const unsigned WORKER_QUEUE_SIZE = 1024;

public:
  void stop() { worker.stop(); }

  void wait_pending_tasks() { worker.wait_pending_tasks(); }

  task_worker                    worker{"cu_cp_worker", WORKER_QUEUE_SIZE};
  std::unique_ptr<task_executor> exec{std::make_unique<task_worker_executor>(worker)};
};

// ////

cu_cp_test_environment::cu_cp_test_environment(cu_cp_test_env_params params_) :
  params(std::move(params_)),
  cu_cp_workers(std::make_unique<worker_manager>()),
  timers(64),
  amf_stub(std::move(params.amf_stub))
{
  // Initialize logging
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("NGAP").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("RRC").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("SEC").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("PDCP").set_hex_dump_max_size(32);
  srslog::init();

  // create CU-CP config
  cu_cp_configuration cfg             = config_helpers::make_default_cu_cp_config();
  cfg.cu_cp_executor                  = cu_cp_workers->exec.get();
  cfg.ngap_notifier                   = &*amf_stub;
  cfg.timers                          = &timers;
  cfg.ngap_config                     = config_helpers::make_default_ngap_config();
  cfg.max_nof_dus                     = params.max_nof_dus;
  cfg.max_nof_cu_ups                  = params.max_nof_cu_ups;
  cfg.ue_config.max_nof_supported_ues = params.max_nof_dus * MAX_NOF_UES_PER_DU;

  // create CU-CP instance.
  cu_cp_inst = create_cu_cp(cfg);

  // Pass CU-CP PDU handler to AMF.
  amf_stub->attach_cu_cp_pdu_handler(cu_cp_inst->get_ng_handler().get_ngap_message_handler());
}

cu_cp_test_environment::~cu_cp_test_environment()
{
  dus.clear();
  cu_ups.clear();
  cu_cp_inst->stop();
  cu_cp_workers->stop();
}

void cu_cp_test_environment::tick()
{
  // Dispatch clock ticking to CU-CP worker
  cu_cp_workers->worker.push_task_blocking([this]() { timers.tick(); });
}

bool cu_cp_test_environment::tick_until(std::chrono::milliseconds timeout, const std::function<bool()>& stop_condition)
{
  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    done = false;

  // Tick up to "timeout" times, waiting for stop_condition() to return true.
  for (unsigned i = 0; i != timeout.count(); ++i) {
    if (stop_condition()) {
      return true;
    }

    // Push to CU-CP worker task taht checks the state of the condition.
    cu_cp_workers->worker.push_task_blocking([&]() {
      // Need to tick the clock.
      tick();

      std::lock_guard<std::mutex> lock(mutex);
      done = true;
      cvar.notify_one();
    });

    // Wait for tick to be processed.
    {
      std::unique_lock<std::mutex> lock(mutex);
      cvar.wait(lock, [&done]() { return done; });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return stop_condition();
}

bool cu_cp_test_environment::wait_for_ngap_tx_pdu(ngap_message& pdu, std::chrono::milliseconds timeout)
{
  return tick_until(timeout, [&]() { return amf_stub->try_pop_rx_pdu(pdu); });
}

bool cu_cp_test_environment::wait_for_e1ap_tx_pdu(unsigned                  cu_up_idx,
                                                  e1ap_message&             pdu,
                                                  std::chrono::milliseconds timeout)
{
  return tick_until(timeout, [&]() { return cu_ups.at(cu_up_idx)->try_pop_rx_pdu(pdu); });
}

bool cu_cp_test_environment::wait_for_f1ap_tx_pdu(unsigned du_idx, f1ap_message& pdu, std::chrono::milliseconds timeout)
{
  report_fatal_error_if_not(dus.size() >= du_idx and dus[du_idx] != nullptr, "DU index out of range");

  return tick_until(timeout, [&]() {
    if (du_idx >= dus.size() or dus[du_idx] == nullptr) {
      return false;
    }
    return dus[du_idx]->try_pop_rx_pdu(pdu);
  });
}

void cu_cp_test_environment::run_ng_setup()
{
  ngap_message ng_setup_resp = srs_cu_cp::generate_ng_setup_response();
  get_amf().enqueue_next_tx_pdu(ng_setup_resp);
  report_fatal_error_if_not(get_cu_cp().start(), "Failed to start CU-CP");

  ngap_message ngap_pdu;
  report_fatal_error_if_not(get_amf().try_pop_rx_pdu(ngap_pdu), "CU-CP did not send the NG Setup Request to the AMF");
  report_fatal_error_if_not(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request),
                            "CU-CP did not setup the AMF connection");
}

optional<unsigned> cu_cp_test_environment::connect_new_du()
{
  auto du_stub = create_mock_du({get_cu_cp().get_f1c_handler()});
  if (not du_stub) {
    return nullopt;
  }
  for (; dus.count(next_du_idx) != 0; ++next_du_idx) {
  }
  auto ret = dus.insert(std::make_pair(next_du_idx, std::move(du_stub)));
  report_fatal_error_if_not(ret.second, "Race condition detected");
  return next_du_idx;
}

bool cu_cp_test_environment::drop_du_connection(unsigned du_idx)
{
  auto it = dus.find(du_idx);
  if (it == dus.end()) {
    return false;
  }
  dus.erase(it);
  return true;
}

bool cu_cp_test_environment::run_f1_setup(unsigned du_idx)
{
  get_du(du_idx).push_tx_pdu(generate_f1_setup_request());
  f1ap_message f1ap_pdu;
  bool         result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000});
  return result;
}

optional<unsigned> cu_cp_test_environment::connect_new_cu_up()
{
  auto cu_up_obj = create_mock_cu_up(get_cu_cp().get_e1_handler());
  if (not cu_up_obj) {
    return nullopt;
  }
  for (; cu_ups.count(next_cu_up_idx) != 0; ++next_cu_up_idx) {
  }
  auto ret = cu_ups.insert(std::make_pair(next_cu_up_idx, std::move(cu_up_obj)));
  report_fatal_error_if_not(ret.second, "Race condition detected");
  return next_cu_up_idx;
}

bool cu_cp_test_environment::drop_cu_up_connection(unsigned cu_up_idx)
{
  auto it = cu_ups.find(cu_up_idx);
  if (it == cu_ups.end()) {
    return false;
  }
  cu_ups.erase(it);
  return true;
}

bool cu_cp_test_environment::run_e1_setup(unsigned cu_up_idx)
{
  get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_cu_up_e1_setup_request());
  e1ap_message e1ap_pdu;
  bool         result = this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000});
  return result;
}

bool cu_cp_test_environment::connect_new_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, rnti_t crnti)
{
  // Inject Initial UL RRC message
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
  test_logger.info("Injecting Initial UL RRC message");
  get_du(du_idx).push_tx_pdu(init_ul_rrc_msg);
  f1ap_message f1ap_pdu;
  bool         result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000});
  if (not result) {
    return false;
  }
  if (f1ap_pdu.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types_opts::init_msg or
      f1ap_pdu.pdu.init_msg().value.type().value !=
          asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer) {
    // CU-CP must send DL RRC message transfer.
    return false;
  }
  if (f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_du_ue_f1ap_id != gnb_du_ue_f1ap_id_to_uint(du_ue_id) or
      int_to_srb_id(f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id) != srb_id_t::srb0) {
    return false;
  }

  {
    asn1::cbit_ref              bref{f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->rrc_container};
    asn1::rrc_nr::dl_ccch_msg_s ccch;
    if (ccch.unpack(bref) != asn1::SRSASN_SUCCESS) {
      return false;
    }
    if (ccch.msg.c1().type().value != asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_setup) {
      return false;
    }
  }

  return true;
}
