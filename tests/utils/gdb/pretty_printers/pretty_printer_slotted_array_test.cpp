/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/slotted_array.h"

int main()
{
  srsran::slotted_array<int, 5> a;

  a.insert(1, 3);
  a.insert(3, -5);

  abort();
}
