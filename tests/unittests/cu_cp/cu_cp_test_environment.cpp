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
#include "tests/unittests/cu_cp/test_doubles/dummy_amf.h"
#include "tests/unittests/cu_cp/test_doubles/dummy_cu_up.h"
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

class cu_cp_test_environment::gateway_manager
{
public:
  std::unique_ptr<synchronized_dummy_cu_up> cu_up;
};

// ////

cu_cp_test_environment::cu_cp_test_environment(cu_cp_test_env_params params) :
  cu_cp_workers(std::make_unique<worker_manager>()),
  timers(64),
  amf_stub(std::move(params.amf_stub)),
  gw(std::make_unique<gateway_manager>())
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

  // create CU-CP instance.
  cu_cp = create_cu_cp(cfg);

  // Pass CU-CP PDU handler to AMF.
  amf_stub->attach_cu_cp_pdu_handler(cu_cp->get_cu_cp_ngap_connection_interface().get_ngap_message_handler());

  // Instantiate CU-UP.
  gw->cu_up = std::make_unique<synchronized_dummy_cu_up>(cu_cp->get_connected_cu_ups());
}

cu_cp_test_environment::~cu_cp_test_environment()
{
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
  enum class task_done { running, condition_true, condition_false };
  std::mutex              mutex;
  std::condition_variable cvar;
  task_done               done = task_done::running;

  // Tick up to "timeout" times, waiting for stop_condition() to return true.
  for (unsigned i = 0; i != timeout.count(); ++i) {
    cu_cp_workers->worker.push_task_blocking([&]() {
      bool success = stop_condition();
      if (not success) {
        // Need to tick the clock.
        tick();
        success = stop_condition();
      }

      std::lock_guard<std::mutex> lock(mutex);
      done = success ? task_done::condition_true : task_done::condition_false;
      cvar.notify_one();
    });

    // Wait for tick to be processed.
    {
      std::unique_lock<std::mutex> lock(mutex);
      cvar.wait(lock, [&done]() { return done == task_done::running; });
    }

    if (done == task_done::condition_true) {
      return true;
    }
  }
  return false;
}

bool cu_cp_test_environment::wait_for_ngap_tx_pdu(ngap_message& pdu, std::chrono::milliseconds timeout)
{
  return tick_until(timeout, [&]() { return amf_stub->try_pop_rx_pdu(pdu); });
}

bool cu_cp_test_environment::try_pop_e1ap_tx_pdu(e1ap_message& pdu)
{
  return gw->cu_up->try_pop_rx_pdu(pdu);
}

bool cu_cp_test_environment::wait_for_e1ap_tx_pdu(e1ap_message& pdu, std::chrono::milliseconds timeout)
{
  return tick_until(timeout, [&]() { return gw->cu_up->try_pop_rx_pdu(pdu); });
}
