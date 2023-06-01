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

#include "srsran/adt/filter_view.h"
#include "srsran/support/test_utils.h"

using namespace srsran;

void test_array_filter()
{
  std::array<bool, 10> a{};
  a[2] = true;

  int  count       = 0;
  auto filter_func = [](bool b) { return b; };
  for (bool v : views::filter(a, filter_func)) {
    TESTASSERT(v == true);
    count++;
  }
  TESTASSERT_EQ(1, count);
}

void test_rvalue_filter()
{
  int count = 0;
  for (int v : views::filter(std::vector<int>{1, 2, 3, 4, 5}, [](int i) { return i < 3; })) {
    TESTASSERT(v < 3);
    count++;
  }
  TESTASSERT_EQ(2, count);
}

void test_chain_filter()
{
  int count = 0;
  for (int v : views::filter(views::filter(std::vector<int>{1, 2, 3, 4, 5, 6, 7}, [](int i) { return i < 5; }),
                             [](int i) { return i % 2 == 0; })) {
    TESTASSERT(v < 5 and v % 2 == 0);
    count++;
  }
  TESTASSERT_EQ(2, count);
}

int main()
{
  test_array_filter();
  test_rvalue_filter();
  test_chain_filter();
}