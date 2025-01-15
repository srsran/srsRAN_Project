/*
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/versioning/version.h"
#include "version_info.h"

using namespace srsran;

const char* srsran::get_version()
{
  return SRSRAN_VERSION_STRING;
}
