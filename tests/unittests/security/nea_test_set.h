/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

struct nea_test_set {
  const char* name;
  const char* key_cstr;
  uint32_t    count;
  uint8_t     bearer;
  uint8_t     direction;
  uint32_t    length;
  const char* plaintext_cstr;
  const char* ciphertext_cstr;
};