/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_timing_manager_impl.h"

using namespace srsran;
using namespace ofh;

ota_symbol_boundary_notifier_manager& timing_manager_impl::get_ota_symbol_boundary_notifier_manager()
{
  return worker;
}

operation_controller& timing_manager_impl::get_controller()
{
  return worker;
}
