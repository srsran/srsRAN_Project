/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

struct nia_test_set {
  const char* name;
  uint32_t    count_i;
  uint8_t     bearer;
  uint8_t     direction;
  const char* ik_cstr;
  uint32_t    length;
  const char* message_cstr;
  const char* mact_cstr;
};
