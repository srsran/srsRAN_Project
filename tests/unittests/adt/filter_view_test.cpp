/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/filter_view.h"
#include "srsgnb/support/test_utils.h"

using namespace srsran;

void test_array_filter()
{
  std::array<bool, 10> a{};
  a[2] = true;

  int count = 0;
  for (bool v : views::filter(a, [](bool b) { return b; })) {
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