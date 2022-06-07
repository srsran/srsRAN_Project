/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/downlink_processor_factory.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

namespace {

class dummy_downlink_processor : public downlink_processor
{
public:
  unsigned id;

  explicit dummy_downlink_processor(unsigned id) : id(id) {}

  void process_pdcch(pdcch_processor::pdu_t& pdu) override {}

  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
  }

  void process_ssb(const ssb_processor::pdu_t& pdu) override {}

  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override {}

  void send_resource_grid() override {}
};

} // namespace

static void test_dl_processor_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 1;
  downlink_processor_pool_config::info info{0, 1, {}};
  info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(0));
  dl_procs.dl_processors.push_back(std::move(info));

  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  auto& proc = dl_proc_pool->get_processor({1, 0, 0}, 0);

  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc).id, 0);
}

void test_pass_same_slot_gets_same_processor()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 1;
  downlink_processor_pool_config::info info{0, 1, {}};
  info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(0));
  dl_procs.dl_processors.push_back(std::move(info));

  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  auto& proc  = dl_proc_pool->get_processor({1, 0, 0}, 0);
  auto& proc1 = dl_proc_pool->get_processor({1, 0, 0}, 0);

  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc).id, 0);
  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc1).id, 0);
}

static void test_consecutive_dl_processor_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 1;
  downlink_processor_pool_config::info info{0, 1, {}};
  info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(0));
  info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(1));
  dl_procs.dl_processors.push_back(std::move(info));

  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  auto& proc  = dl_proc_pool->get_processor({1, 0, 0}, 0);
  auto& proc1 = dl_proc_pool->get_processor({1, 0, 1}, 0);

  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc).id, 0);
  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc1).id, 1);
}

static void test_2sectors_2dl_processor_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 2;
  for (unsigned i = 0, e = 2; i != e; ++i) {
    downlink_processor_pool_config::info info{i, 1, {}};
    info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(i * 10 + 0));
    info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(i * 10 + 1));
    dl_procs.dl_processors.push_back(std::move(info));
  }

  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  auto& proc00 = dl_proc_pool->get_processor({1, 0, 0}, 0);
  auto& proc01 = dl_proc_pool->get_processor({1, 0, 1}, 0);
  auto& proc10 = dl_proc_pool->get_processor({1, 0, 0}, 1);
  auto& proc11 = dl_proc_pool->get_processor({1, 0, 1}, 1);

  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc00).id, 0);
  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc01).id, 1);
  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc10).id, 10);
  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc11).id, 11);
}

static void test_circular_buffer_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 1;
  downlink_processor_pool_config::info info{0, 0, {}};
  info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(0));
  info.procs.emplace_back(std::make_unique<dummy_downlink_processor>(1));
  dl_procs.dl_processors.push_back(std::move(info));

  auto dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  auto& proc0 = dl_proc_pool->get_processor({0, 0, 0}, 0);
  auto& proc1 = dl_proc_pool->get_processor({0, 0, 1}, 0);
  auto& proc2 = dl_proc_pool->get_processor({0, 0, 2}, 0);

  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc0).id, 0);
  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc1).id, 1);
  TESTASSERT_EQ(static_cast<dummy_downlink_processor&>(proc2).id, 0);
}
int main()
{
  test_dl_processor_ok();
  test_pass_same_slot_gets_same_processor();
  test_consecutive_dl_processor_ok();
  test_2sectors_2dl_processor_ok();
  test_circular_buffer_ok();
  fmt::print("Downlink processor pool -> OK\n");
}
