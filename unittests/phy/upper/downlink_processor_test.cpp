/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/phy/upper/downlink_processor_impl.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/downlink_processor_factory.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

namespace {

class dummy_resource_grid : public resource_grid
{
public:
  void set_all_zero() override {}
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override {}
  span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override
  {
    return {};
  }

  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override {}
  void get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const override {}

  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override
  {
    return {};
  }

  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override {}
};

class dummy_upper_rg_gateway : public upper_phy_rg_gateway
{
public:
  bool sent = false;
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    sent = true;
    fmt::print("[Upper RG Gateway] - Sending resource grid\n");
  }
};

class dummy_pdsch_proc : public pdsch_processor
{
public:
  void process(resource_grid_writer&                                        grid,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override
  {
    fmt::print("[PDSCH Processor] - Processing PDU\n");
  }
};

class dummy_pdcch_proc : public pdcch_processor
{
public:
  void process(resource_grid_writer& grid, pdu_t& pdu) override { fmt::print("[PDCCH Processor] - Processing PDU\n"); }
};

class dummy_ssb_proc : public ssb_processor
{
public:
  void process(const pdu_t& pdu, resource_grid_writer& grid) override
  {
    fmt::print("[SSB Processor] - Processing PDU\n");
  }
};

class dummy_pdsch_proc_slow : public pdsch_processor
{
public:
  void process(resource_grid_writer&                                        grid,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    fmt::print("[PDSCH Processor] - Processing PDU\n");
  }
};

class dummy_pdcch_proc_slow : public pdcch_processor
{
public:
  void process(resource_grid_writer& grid, pdu_t& pdu) override
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    fmt::print("[PDCCH Processor] - Processing PDU\n");
  }
};

class dummy_ssb_proc_slow : public ssb_processor
{
public:
  void process(const pdu_t& pdu, resource_grid_writer& grid) override
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    fmt::print("[SSB Processor] - Processing PDU\n");
  }
};

class dummy_dl_processor_factory : public downlink_processor_factory
{
  dummy_upper_rg_gateway& gw;
  task_executor&          executor;
  bool                    slow;

public:
  dummy_dl_processor_factory(dummy_upper_rg_gateway& gw, task_executor& executor, bool slow) :
    gw(gw), executor(executor), slow(slow)
  {
  }

  std::unique_ptr<downlink_processor> create(const downlink_processor_config& config) override
  {
    if (!slow)
      return std::make_unique<downlink_processor_impl>(gw,
                                                       std::make_unique<dummy_pdcch_proc>(),
                                                       std::make_unique<dummy_pdsch_proc>(),
                                                       std::make_unique<dummy_ssb_proc>(),
                                                       executor);

    return std::make_unique<downlink_processor_impl>(gw,
                                                     std::make_unique<dummy_pdcch_proc_slow>(),
                                                     std::make_unique<dummy_pdsch_proc_slow>(),
                                                     std::make_unique<dummy_ssb_proc_slow>(),
                                                     executor);
  }
};

} // namespace

static downlink_processor_pool_config
create_dl_processors(dummy_upper_rg_gateway& gw, task_executor& executor, bool slow = false)
{
  dummy_dl_processor_factory     factory(gw, executor, slow);
  downlink_processor_pool_config procs;
  procs.num_sectors = 3;
  // Create 3 cells.
  for (unsigned i = 0, e = 3; i != e; ++i) {
    // Create for every numerology.
    for (unsigned y = 0, z = 5; y != z; ++y) {
      downlink_processor_pool_config::info info;
      info.sector     = i;
      info.numerology = y;
      // Create 10 processors/cell.
      for (unsigned j = 0, k = (y + 1) * 10; j != k; ++j) {
        info.procs.emplace_back(factory.create({i * 10000 + y * 1000 + j}));
      }
      procs.dl_processors.push_back(std::move(info));
    }
  }

  return procs;
}

static void test_works_in_random_order()
{
  fmt::print("Testing downlink processor works in random order\n");
  dummy_upper_rg_gateway gw;
  task_worker            worker("", 10);
  task_worker_executor   executor(worker);

  auto dl_procs = create_dl_processors(gw, executor);

  // Create downlink processor pool.
  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point slot(1, 2, 1);
  unsigned   sector  = 0;
  auto&      dl_proc = dl_proc_pool->get_processor(slot, sector);

  dummy_resource_grid grid;
  dl_proc.configure_resource_grid({slot, sector}, grid);

  dl_proc.process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_proc.process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_proc.process_pdsch({data}, {});
  dl_proc.send_resource_grid();

  while (worker.nof_pending_tasks()) {
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }
  worker.stop();

  TESTASSERT(gw.sent);
}

static void test_works_send_after_finish()
{
  fmt::print("Testing downlink processor works if send_resource_grid() is called after finishing processing\n");
  dummy_upper_rg_gateway gw;
  task_worker            worker("", 10);
  task_worker_executor   executor(worker);

  auto dl_procs = create_dl_processors(gw, executor);

  // Create downlink processor pool.
  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point slot(1, 2, 1);
  unsigned   sector  = 0;
  auto&      dl_proc = dl_proc_pool->get_processor(slot, sector);

  dummy_resource_grid grid;
  dl_proc.configure_resource_grid({slot, sector}, grid);

  dl_proc.process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_proc.process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_proc.process_pdsch({data}, {});
  TESTASSERT(!gw.sent);
  while (worker.nof_pending_tasks()) {
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }

  worker.stop();

  dl_proc.send_resource_grid();

  TESTASSERT(gw.sent);
}

static void test_send_is_called_before_processing_pdus()
{
  fmt::print("Testing downlink processor works if send_resource_grid() is called before finishing processing\n");
  dummy_upper_rg_gateway gw;
  task_worker            worker("", 10);
  task_worker_executor   executor(worker);

  auto dl_procs = create_dl_processors(gw, executor, true);

  // Create downlink processor pool.
  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point slot(1, 2, 1);
  unsigned   sector  = 0;
  auto&      dl_proc = dl_proc_pool->get_processor(slot, sector);

  dummy_resource_grid grid;
  dl_proc.configure_resource_grid({slot, sector}, grid);

  dl_proc.process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_proc.process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_proc.process_pdsch({data}, {});
  TESTASSERT(!gw.sent);
  dl_proc.send_resource_grid();
  TESTASSERT(!gw.sent);

  while (worker.nof_pending_tasks()) {
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }

  worker.stop();

  TESTASSERT(gw.sent);
}

int main()
{
  test_works_in_random_order();
  test_works_send_after_finish();
  test_send_is_called_before_processing_pdus();
  fmt::print("Downlink processor test -> OK\n");
}
