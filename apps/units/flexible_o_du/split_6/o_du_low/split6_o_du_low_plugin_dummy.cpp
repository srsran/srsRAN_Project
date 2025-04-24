/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_o_du_low_plugin_dummy.h"

using namespace srsran;

#ifndef SRSRAN_HAS_ENTERPRISE
std::unique_ptr<split6_o_du_low_plugin> srsran::create_split6_o_du_low_plugin(std::string_view app_name)
{
  return std::make_unique<split6_o_du_low_plugin_dummy>();
}
#endif
