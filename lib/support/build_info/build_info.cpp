/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/build_info/build_info.h"
#include "hashes.h"

namespace srsran {

const char* get_build_hash()
{
  return build_hash;
}

const char* get_build_info()
{
  return build_info;
}

const char* get_build_mode()
{
  return build_mode;
}

} // namespace srsran