/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_test_doubles.h"
#include "srsgnb/phy/upper/downlink_processor_factory.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

static void test_dl_processor_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 1;
  unsigned sector      = 0;
  unsigned numerology  = 1;
  unsigned id          = 0;

  downlink_processor_pool_config::info info{sector, numerology, {}};
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point          slot(numerology, 0, 0);
  downlink_processor& dl_processor = dl_proc_pool->get_processor(slot, id);

  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor).get_id(), id);
}

void test_pass_same_slot_gets_same_processor()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 1;
  unsigned sector      = 0;
  unsigned numerology  = 1;
  unsigned id          = 0;

  downlink_processor_pool_config::info info{sector, numerology, {}};
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point          slot(numerology, 0, 0);
  downlink_processor& dl_processor_0 = dl_proc_pool->get_processor(slot, sector);
  downlink_processor& dl_processor_1 = dl_proc_pool->get_processor(slot, sector);

  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_0).get_id(), id);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_1).get_id(), id);
  TESTASSERT(&dl_processor_0 == &dl_processor_1);
}

static void test_consecutive_dl_processor_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors                            = 1;
  unsigned                             numerology = 1;
  unsigned                             sector     = 0;
  downlink_processor_pool_config::info info{sector, numerology, {}};

  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(0));
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(1));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point          first(numerology, 0, 0);
  downlink_processor& dl_processor_0 = dl_proc_pool->get_processor(first, sector);
  slot_point          second(numerology, 0, 1);
  downlink_processor& dl_processor_1 = dl_proc_pool->get_processor(second, sector);

  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_0).get_id(), 0);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_1).get_id(), 1);
}

static void test_2sectors_2dl_processor_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 2;
  unsigned numerology  = 1;
  for (unsigned i = 0, e = dl_procs.num_sectors; i != e; ++i) {
    downlink_processor_pool_config::info info{i, numerology, {}};
    info.procs.emplace_back(std::make_unique<downlink_processor_spy>(i * 10 + 0));
    info.procs.emplace_back(std::make_unique<downlink_processor_spy>(i * 10 + 1));
    dl_procs.dl_processors.push_back(std::move(info));
  }

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point          first(numerology, 0, 0);
  downlink_processor& dl_processor_00 = dl_proc_pool->get_processor(first, 0);
  slot_point          second(numerology, 0, 1);
  downlink_processor& dl_processor_01 = dl_proc_pool->get_processor(second, 0);
  slot_point          third(numerology, 0, 0);
  downlink_processor& dl_processor_10 = dl_proc_pool->get_processor(third, 1);
  slot_point          fourth(numerology, 0, 1);
  downlink_processor& dl_processor_11 = dl_proc_pool->get_processor(fourth, 1);

  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_00).get_id(), 0);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_01).get_id(), 1);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_10).get_id(), 10);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_11).get_id(), 11);
}

static void test_circular_buffer_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors                            = 1;
  unsigned                             numerology = 0;
  unsigned                             sector     = 0;
  downlink_processor_pool_config::info info{sector, numerology, {}};
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(0));
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(1));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point          first(numerology, 0, 0);
  downlink_processor& dl_processor_0 = dl_proc_pool->get_processor(first, sector);
  slot_point          second(numerology, 0, 1);
  downlink_processor& dl_processor_1 = dl_proc_pool->get_processor(second, sector);
  slot_point          third(numerology, 0, 2);
  downlink_processor& dl_processor_2 = dl_proc_pool->get_processor(third, sector);

  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_0).get_id(), 0);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_1).get_id(), 1);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_2).get_id(), 0);
}

static void test_2sectors_2numerologies_2dl_processor_ok()
{
  downlink_processor_pool_config dl_procs;
  dl_procs.num_sectors = 2;
  for (unsigned i = 0, e = dl_procs.num_sectors; i != e; ++i) {
    for (auto numerology : {1U, 3U}) {
      downlink_processor_pool_config::info info{i, numerology, {}};
      info.procs.emplace_back(std::make_unique<downlink_processor_spy>(i * 100 + numerology * 10 + 0));
      info.procs.emplace_back(std::make_unique<downlink_processor_spy>(i * 100 + numerology * 10 + 1));
      dl_procs.dl_processors.push_back(std::move(info));
    }
  }

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point          first(1, 0, 0);
  downlink_processor& dl_processor_010 = dl_proc_pool->get_processor(first, 0);
  slot_point          second(1, 0, 1);
  downlink_processor& dl_processor_011 = dl_proc_pool->get_processor(second, 0);
  slot_point          third(3, 0, 0);
  downlink_processor& dl_processor_030 = dl_proc_pool->get_processor(third, 0);
  slot_point          fourth(3, 0, 1);
  downlink_processor& dl_processor_031 = dl_proc_pool->get_processor(fourth, 0);
  downlink_processor& dl_processor_110 = dl_proc_pool->get_processor(first, 1);
  downlink_processor& dl_processor_111 = dl_proc_pool->get_processor(second, 1);
  downlink_processor& dl_processor_130 = dl_proc_pool->get_processor(third, 1);
  downlink_processor& dl_processor_131 = dl_proc_pool->get_processor(fourth, 1);

  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_010).get_id(), 10);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_011).get_id(), 11);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_030).get_id(), 30);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_031).get_id(), 31);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_110).get_id(), 110);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_111).get_id(), 111);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_130).get_id(), 130);
  TESTASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_131).get_id(), 131);
}

int main()
{
  test_dl_processor_ok();
  test_pass_same_slot_gets_same_processor();
  test_consecutive_dl_processor_ok();
  test_2sectors_2dl_processor_ok();
  test_2sectors_2numerologies_2dl_processor_ok();
  test_circular_buffer_ok();
}
