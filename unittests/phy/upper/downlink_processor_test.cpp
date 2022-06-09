/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/phy/upper/downlink_processor_single_executor_impl.h"
#include "../resource_grid_test_doubles.h"
#include "channel_processors/pdcch_processor_test_doubles.h"
#include "channel_processors/pdsch_processor_test_doubles.h"
#include "channel_processors/ssb_processor_test_doubles.h"
#include "downlink_processor_test_doubles.h"
#include "signal_processors/csi_rs_processor_test_doubles.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/srsgnb_test.h"
#include "upper_phy_rg_gateway_test_doubles.h"

using namespace srsgnb;

namespace {

/// \brief Task worker that implements the executor interface and requires manual calls to run pending deferred tasks.
/// Useful for unit testing. This implementation always queues the tasks, either by calling execute or defer.
class manual_task_worker_always_enqueue_tasks : public task_executor
{
public:
  manual_task_worker_always_enqueue_tasks(size_t q_size) : t_id(std::this_thread::get_id()), pending_tasks(q_size) {}

  std::thread::id get_thread_id() const { return t_id; }

  void execute(unique_task task) override { defer(std::move(task)); }

  void defer(unique_task task) override { pending_tasks.push_blocking(std::move(task)); }

  bool has_pending_tasks() const { return not pending_tasks.empty(); }

  bool is_stopped() const { return pending_tasks.is_stopped(); }

  void stop()
  {
    if (not is_stopped()) {
      pending_tasks.stop();
    }
  }

  void request_stop()
  {
    defer([this]() { stop(); });
  }

  /// Run all pending tasks until queue is emptied.
  bool run_pending_tasks()
  {
    assert_thread_id();
    bool ret = false, success = false;
    do {
      unique_task t;
      success = pending_tasks.try_pop(t);
      if (success) {
        t();
        ret = true;
      }
    } while (success);
    return ret;
  }

  /// Run next pending task if it is enqueued.
  bool try_run_next()
  {
    assert_thread_id();
    unique_task t;
    bool        success = pending_tasks.try_pop(t);
    if (not success) {
      return false;
    }
    t();
    return true;
  }

  /// Run next pending task once it is enqueued.
  bool run_next_blocking()
  {
    assert_thread_id();
    bool        success = false;
    unique_task t       = pending_tasks.pop_blocking(&success);
    if (not success) {
      return false;
    }
    t();
    return true;
  }

private:
  bool has_thread_id() const { return t_id != std::thread::id{}; }

  void assert_thread_id()
  {
    srsran_assert(t_id == std::this_thread::get_id(), "run() caller thread should not change.");
  }

  std::thread::id                 t_id;
  dyn_blocking_queue<unique_task> pending_tasks;
};

} // namespace

static void test_works_in_order()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_fto>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_fto>();
  auto ssb_processor    = std::make_unique<ssb_processor_fto>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_fto>();

  pdcch_processor_fto&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_fto&  pdsch_ref  = *pdsch_processor;
  ssb_processor_fto&    ssb_ref    = *ssb_processor;
  csi_rs_processor_fto& csi_rs_ref = *csi_rs_processor;

  auto       dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  TESTASSERT(!pdcch_ref.process_method_called);
  TESTASSERT(!pdsch_ref.process_method_called);
  TESTASSERT(!ssb_ref.process_method_called);
  TESTASSERT(!csi_rs_ref.process_method_called);
  TESTASSERT(!gw.sent);

  dl_processor->process_ssb({});
  TESTASSERT(ssb_ref.process_method_called);

  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  TESTASSERT(pdcch_ref.process_method_called);

  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  TESTASSERT(pdsch_ref.process_method_called);

  dl_processor->process_csi_rs({});
  TESTASSERT(csi_rs_ref.process_method_called);

  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(gw.sent);
}

static void test_finish_is_called_before_processing_pdus()
{
  upper_phy_rg_gateway_fto                gw;
  manual_task_worker_always_enqueue_tasks executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_fto>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_fto>();
  auto ssb_processor    = std::make_unique<ssb_processor_fto>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_fto>();

  pdcch_processor_fto&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_fto&  pdsch_ref  = *pdsch_processor;
  ssb_processor_fto&    ssb_ref    = *ssb_processor;
  csi_rs_processor_fto& csi_rs_ref = *csi_rs_processor;

  auto dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);

  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_csi_rs({});

  TESTASSERT(!pdcch_ref.process_method_called);
  TESTASSERT(!pdsch_ref.process_method_called);
  TESTASSERT(!ssb_ref.process_method_called);
  TESTASSERT(!csi_rs_ref.process_method_called);
  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();
  TESTASSERT(!gw.sent);

  // Run all the queued tasks.
  executor.run_pending_tasks();

  TESTASSERT(pdcch_ref.process_method_called);
  TESTASSERT(pdsch_ref.process_method_called);
  TESTASSERT(ssb_ref.process_method_called);
  TESTASSERT(csi_rs_ref.process_method_called);

  TESTASSERT(gw.sent);

  TESTASSERT(gw.sent);
}

static void test_process_pdu_after_finish_processing_pdus_does_nothing()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_fto>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_fto>();
  auto ssb_processor    = std::make_unique<ssb_processor_fto>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_fto>();

  pdcch_processor_fto&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_fto&  pdsch_ref  = *pdsch_processor;
  ssb_processor_fto&    ssb_ref    = *ssb_processor;
  csi_rs_processor_fto& csi_rs_ref = *csi_rs_processor;

  auto dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);

  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->finish_processing_pdus();

  TESTASSERT(pdcch_ref.process_method_called);
  TESTASSERT(pdsch_ref.process_method_called);
  TESTASSERT(ssb_ref.process_method_called);
  TESTASSERT(gw.sent);

  // Process a PDU after finish_processing_pdus() method has been called.
  dl_processor->process_csi_rs({});
  TESTASSERT(!csi_rs_ref.process_method_called);
}

static void test_process_pdu_before_configure_does_nothing()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_fto>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_fto>();
  auto ssb_processor    = std::make_unique<ssb_processor_fto>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_fto>();

  pdcch_processor_fto&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_fto&  pdsch_ref  = *pdsch_processor;
  ssb_processor_fto&    ssb_ref    = *ssb_processor;
  csi_rs_processor_fto& csi_rs_ref = *csi_rs_processor;

  auto dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_csi_rs({});

  TESTASSERT(!pdcch_ref.process_method_called);
  TESTASSERT(!pdsch_ref.process_method_called);
  TESTASSERT(!ssb_ref.process_method_called);
  TESTASSERT(!csi_rs_ref.process_method_called);
  TESTASSERT(!gw.sent);
}

static void test_finish_processing_before_configure_does_nothing()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto dl_processor =
      std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                std::make_unique<pdcch_processor_fto>(),
                                                                std::make_unique<pdsch_processor_fto>(),
                                                                std::make_unique<ssb_processor_fto>(),
                                                                std::make_unique<csi_rs_processor_fto>(),
                                                                executor);

  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(!gw.sent);
}

static void test_2consecutive_slots()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto dl_processor =
      std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                std::make_unique<pdcch_processor_fto>(),
                                                                std::make_unique<pdsch_processor_fto>(),
                                                                std::make_unique<ssb_processor_fto>(),
                                                                std::make_unique<csi_rs_processor_fto>(),
                                                                executor);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_csi_rs({});
  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(gw.sent);

  slot_point slot2(1, 2, 2);
  gw.clear_sent();
  resource_grid_dummy grid2;
  dl_processor->configure_resource_grid({slot2, sector}, grid2);

  dl_processor->process_ssb({});
  dl_processor->process_pdcch(pdu);
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_csi_rs({});
  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(gw.sent);
}

static void test_finish_without_processing_pdus_sends_the_grid()
{
  upper_phy_rg_gateway_fto                gw;
  manual_task_worker_always_enqueue_tasks executor(10);

  auto dl_processor =
      std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                std::make_unique<pdcch_processor_fto>(),
                                                                std::make_unique<pdsch_processor_fto>(),
                                                                std::make_unique<ssb_processor_fto>(),
                                                                std::make_unique<csi_rs_processor_fto>(),
                                                                executor);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(gw.sent);
}

int main()
{
  test_works_in_order();
  test_finish_is_called_before_processing_pdus();
  test_process_pdu_after_finish_processing_pdus_does_nothing();
  test_process_pdu_before_configure_does_nothing();
  test_finish_without_processing_pdus_sends_the_grid();
  test_finish_processing_before_configure_does_nothing();
  test_2consecutive_slots();
}
