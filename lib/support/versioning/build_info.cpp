/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/versioning/build_info.h"
#include "hashes.h"

using namespace srsran;

const char* srsran::get_build_hash()
{
  return build_hash;
}

const char* srsran::get_build_info()
{
  return build_info;
}

const char* srsran::get_build_mode()
{
  return build_mode;
}
