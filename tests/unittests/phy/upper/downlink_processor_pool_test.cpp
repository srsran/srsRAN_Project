/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "downlink_processor_test_doubles.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(DownlingProcessorPool, ProcessorOk)
{
  downlink_processor_pool_config dl_procs;
  subcarrier_spacing             scs = subcarrier_spacing::kHz30;
  unsigned                       id  = 0;

  downlink_processor_pool_config::downlink_processor_set info{scs, {}};
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point                     slot(to_numerology_value(scs), 0, 0);
  downlink_processor_controller& dl_processor = dl_proc_pool->get_processor_controller(slot);

  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor).get_id(), id);
}

TEST(DownlingProcessorPool, SameSlotDifferentProcessor)
{
  downlink_processor_pool_config dl_procs;
  subcarrier_spacing             scs = subcarrier_spacing::kHz30;
  unsigned                       id  = 0;

  downlink_processor_pool_config::downlink_processor_set info{scs, {}};
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id));
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id + 1));
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id + 2));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point                     slot(to_numerology_value(scs), 0, 0);
  downlink_processor_controller& dl_processor_0 = dl_proc_pool->get_processor_controller(slot);
  downlink_processor_controller& dl_processor_1 = dl_proc_pool->get_processor_controller(slot);

  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_0).get_id(), id);
  ASSERT_NE(static_cast<downlink_processor_spy&>(dl_processor_1).get_id(), id);
  ASSERT_TRUE(&dl_processor_0 != &dl_processor_1);
}

TEST(DownlingProcessorPool, ConsecutiveProcessorOk)
{
  downlink_processor_pool_config                         dl_procs;
  subcarrier_spacing                                     scs = subcarrier_spacing::kHz30;
  unsigned                                               id0 = 0;
  unsigned                                               id1 = 1;
  downlink_processor_pool_config::downlink_processor_set info{scs, {}};

  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id0));
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(id1));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point                     first(to_numerology_value(scs), 0, 0);
  downlink_processor_controller& dl_processor_0 = dl_proc_pool->get_processor_controller(first);
  slot_point                     second(to_numerology_value(scs), 0, 1);
  downlink_processor_controller& dl_processor_1 = dl_proc_pool->get_processor_controller(second);

  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_0).get_id(), id0);
  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_1).get_id(), id1);
}

TEST(DownlingProcessorPool, CircularBufferOk)
{
  downlink_processor_pool_config                         dl_procs;
  subcarrier_spacing                                     scs = subcarrier_spacing::kHz30;
  downlink_processor_pool_config::downlink_processor_set info{scs, {}};
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(0));
  info.procs.emplace_back(std::make_unique<downlink_processor_spy>(1));
  dl_procs.dl_processors.push_back(std::move(info));

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point                     first(to_numerology_value(scs), 0, 0);
  downlink_processor_controller& dl_processor_0 = dl_proc_pool->get_processor_controller(first);
  slot_point                     second(to_numerology_value(scs), 0, 1);
  downlink_processor_controller& dl_processor_1 = dl_proc_pool->get_processor_controller(second);
  slot_point                     third(to_numerology_value(scs), 0, 2);
  downlink_processor_controller& dl_processor_2 = dl_proc_pool->get_processor_controller(third);

  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_0).get_id(), 0);
  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_1).get_id(), 1);
  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_2).get_id(), 0);
}

TEST(DownlingProcessorPool, TwoNumerologiesTwoProcessors)
{
  downlink_processor_pool_config dl_procs;
  for (auto scs : {subcarrier_spacing::kHz30, subcarrier_spacing::kHz120}) {
    downlink_processor_pool_config::downlink_processor_set info{scs, {}};
    info.procs.emplace_back(std::make_unique<downlink_processor_spy>(to_numerology_value(scs) * 10 + 0));
    info.procs.emplace_back(std::make_unique<downlink_processor_spy>(to_numerology_value(scs) * 10 + 1));
    dl_procs.dl_processors.push_back(std::move(info));
  }

  std::unique_ptr<downlink_processor_pool> dl_proc_pool = create_dl_processor_pool({std::move(dl_procs)});

  slot_point                     first(1, 0, 0);
  downlink_processor_controller& dl_processor_010 = dl_proc_pool->get_processor_controller(first);
  slot_point                     second(1, 0, 1);
  downlink_processor_controller& dl_processor_011 = dl_proc_pool->get_processor_controller(second);
  slot_point                     third(3, 0, 0);
  downlink_processor_controller& dl_processor_030 = dl_proc_pool->get_processor_controller(third);
  slot_point                     fourth(3, 0, 1);
  downlink_processor_controller& dl_processor_031 = dl_proc_pool->get_processor_controller(fourth);

  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_010).get_id(), 10);
  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_011).get_id(), 11);
  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_030).get_id(), 30);
  ASSERT_EQ(static_cast<downlink_processor_spy&>(dl_processor_031).get_id(), 31);
}
