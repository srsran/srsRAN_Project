/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/complex.h"

int main()
{
  srsran::cbf16_t a;

  a = srsran::to_cbf16({1.23, 4.56});
  abort();
}
