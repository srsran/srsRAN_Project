/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_test_environment.h"
#include "tests/unittests/cu_cp/test_doubles/mock_amf.h"
#include "tests/unittests/cu_cp/test_doubles/mock_cu_up.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_factory.h"
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
  cu_cp_configuration cfg = config_helpers::make_default_cu_cp_config();
  cfg.cu_cp_executor      = cu_cp_workers->exec.get();
  cfg.ngap_notifier       = &*amf_stub;
  cfg.timers              = &timers;
  cfg.ngap_config         = config_helpers::make_default_ngap_config();
  cfg.max_nof_dus         = params.max_nof_dus;
  cfg.max_nof_cu_ups      = params.max_nof_cu_ups;

  // create CU-CP instance.
  cu_cp = create_cu_cp(cfg);

  // Pass CU-CP PDU handler to AMF.
  amf_stub->attach_cu_cp_pdu_handler(cu_cp->get_cu_cp_ngap_connection_interface().get_ngap_message_handler());
}

cu_cp_test_environment::~cu_cp_test_environment()
{
  dus.clear();
  cu_ups.clear();
  cu_cp->stop();
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

optional<unsigned> cu_cp_test_environment::connect_new_du()
{
  auto du_stub = create_mock_du({get_cu_cp().get_connected_dus()});
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

optional<unsigned> cu_cp_test_environment::connect_new_cu_up()
{
  auto cu_up_obj = create_mock_cu_up(get_cu_cp().get_connected_cu_ups());
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