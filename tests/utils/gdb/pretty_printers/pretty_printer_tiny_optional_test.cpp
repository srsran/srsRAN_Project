/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/tiny_optional.h"

int main()
{
  srsran::tiny_optional<std::unique_ptr<int>> a;
  srsran::tiny_optional<float>                b;
  srsran::tiny_optional<std::unique_ptr<int>> c = {};
  srsran::tiny_optional<float>                d;

  int a_val = 8;
  a         = &a_val;
  b         = 3.4;
  d         = {};
  abort();
}
