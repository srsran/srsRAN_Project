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

#include "srsran/adt/unique_function.h"
#include "srsran/support/test_utils.h"

using namespace srsran;

struct C {
  std::unique_ptr<int> val{new int{5}};
};
struct D {
  std::array<int, 64> big_val;
  D() { big_val[0] = 6; }
};

void test_unique_function()
{
  int v = 0;

  auto l0 = [&v]() { v = 1; };

  unique_function<void()> t{l0};
  unique_function<void()> t2{[v]() mutable { v = 2; }};
  // sanity static checks
  static_assert(task_details::is_unique_function<std::decay_t<decltype(t)>>::value, "failed check\n");
  static_assert(std::is_base_of<std::false_type, task_details::is_unique_function<std::decay_t<decltype(l0)>>>::value,
                "failed check\n");

  t();
  t2();
  TESTASSERT_EQ(1, v);
  v              = 2;
  decltype(t) t3 = std::move(t);
  t3();
  TESTASSERT_EQ(1, v);

  C                       c;
  unique_function<void()> t4 = [&v, c = std::move(c)]() { v = *c.val; };
  {
    decltype(t4) t5;
    t5 = std::move(t4);
    t5();
    TESTASSERT(v == 5);
  }

  D                       d;
  unique_function<void()> t6 = [&v, d]() { v = d.big_val[0]; };
  {
    unique_function<void()> t7;
    t6();
    TESTASSERT_EQ(6, v);
    v  = 0;
    t7 = std::move(t6);
    t7();
    TESTASSERT_EQ(6, v);
  }

  auto l1 = [&v, c = C{}]() { v = *c.val; };
  auto l2 = [&v, d]() { v = d.big_val[0]; };
  t       = std::move(l1);
  t2      = l2;
  v       = 0;
  t();
  TESTASSERT_EQ(5, v);
  t2();
  TESTASSERT_EQ(6, v);
  TESTASSERT(t.is_in_small_buffer() and not t2.is_in_small_buffer());
  std::swap(t, t2);
  TESTASSERT(t2.is_in_small_buffer() and not t.is_in_small_buffer());
  v = 0;
  t();
  TESTASSERT_EQ(6, v);
  t2();
  TESTASSERT_EQ(5, v);

  // TEST: task works in const contexts
  t       = l2;
  auto l3 = [](const unique_function<void()>& task) { task(); };
  v       = 0;
  l3(t);
  TESTASSERT_EQ(6, v);
}

int main()
{
  test_unique_function();
}
