/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ofh/slot_symbol_point.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

TEST(slot_symbol_point_test, basic_construction_and_getters_should_pass)
{
  const unsigned    nof_symbols    = 14;
  const unsigned    initial_symbol = 7;
  slot_point        slot(1, 20, 4, 1);
  slot_symbol_point symbol_point(slot, initial_symbol, nof_symbols);

  ASSERT_EQ(symbol_point.get_nof_symbols(), nof_symbols);
  ASSERT_EQ(symbol_point.get_slot(), slot);
  ASSERT_EQ(symbol_point.get_symbol_index(), initial_symbol);
}

TEST(slot_symbol_point_test, symbol_add_and_subtract_should_pass)
{
  const unsigned    nof_symbols    = 14;
  const unsigned    initial_symbol = 7;
  slot_point        slot(1, 20, 0, 0);
  slot_symbol_point symbol_point(slot, initial_symbol, nof_symbols);

  symbol_point += 7;
  ASSERT_EQ(symbol_point.get_symbol_index(), 0);
  ASSERT_EQ(symbol_point.get_slot(), slot_point(1, 20, 0, 1));

  symbol_point += 30;
  ASSERT_EQ(symbol_point.get_symbol_index(), 2);
  ASSERT_EQ(symbol_point.get_slot(), slot_point(1, 20, 1, 1));

  symbol_point -= 30;
  ASSERT_EQ(symbol_point.get_symbol_index(), 0);
  ASSERT_EQ(symbol_point.get_slot(), slot_point(1, 20, 0, 1));

  symbol_point -= 7;
  ASSERT_EQ(symbol_point.get_symbol_index(), initial_symbol);
  ASSERT_EQ(symbol_point.get_slot(), slot);

  // Test wrap around.
  slot         = slot_point(1, 0);
  symbol_point = slot_symbol_point(slot, 0, nof_symbols);
  symbol_point -= 7;
  ASSERT_EQ(symbol_point.get_symbol_index(), 7);
  ASSERT_EQ(symbol_point.get_slot(), slot_point(1, 1023, 9, 1));
}

TEST(slot_symbol_point_test, equal_slot_symbol_points)
{
  const unsigned    nof_symbols   = 14;
  const unsigned    numerology    = 1;
  const unsigned    initial_value = 7;
  slot_symbol_point symbol_point(numerology, initial_value, nof_symbols);
  slot_symbol_point symbol_point2(numerology, initial_value, nof_symbols);

  ASSERT_EQ(symbol_point, symbol_point2);
}

TEST(slot_symbol_point_test, different_slot_symbol_points)
{
  const unsigned    nof_symbols   = 14;
  const unsigned    numerology    = 1;
  const unsigned    initial_value = 7;
  slot_symbol_point symbol_point(numerology, initial_value, nof_symbols);
  slot_symbol_point symbol_point2(numerology, initial_value + 1, nof_symbols);

  ASSERT_NE(symbol_point, symbol_point2);
}

TEST(slot_symbol_point_test, smaller_slot_symbol_point)
{
  const unsigned    nof_symbols   = 14;
  const unsigned    numerology    = 1;
  const unsigned    initial_value = 7;
  slot_symbol_point symbol_point(numerology, initial_value, nof_symbols);
  slot_symbol_point symbol_point2(numerology, initial_value + 1, nof_symbols);

  ASSERT_TRUE(symbol_point < symbol_point2);
}

TEST(slot_symbol_point_test, smaller_slot_symbol_point_with_slot_constructor)
{
  const unsigned    nof_symbols    = 14;
  const unsigned    initial_symbol = 7;
  slot_point        slot(1, 20, 0, 0);
  slot_point        slot2(1, 21, 0, 0);
  slot_symbol_point symbol_point(slot, initial_symbol, nof_symbols);
  slot_symbol_point symbol_point2(slot2, initial_symbol, nof_symbols);

  ASSERT_TRUE(symbol_point < symbol_point2);
}

TEST(slot_symbol_point_test, smaller_slot_symbol_point_same_slot_different_symbol)
{
  const unsigned    nof_symbols    = 14;
  const unsigned    initial_symbol = 7;
  slot_point        slot(1, 20, 0, 0);
  slot_symbol_point symbol_point(slot, initial_symbol, nof_symbols);
  slot_symbol_point symbol_point2(slot, initial_symbol + 1, nof_symbols);

  ASSERT_TRUE(symbol_point < symbol_point2);
}