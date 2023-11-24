/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_downlink_manager_impl.h"

using namespace srsran;
using namespace ofh;

downlink_handler& downlink_manager_impl::get_downlink_handler()
{
  return handler;
}

ota_symbol_boundary_notifier& downlink_manager_impl::get_ota_symbol_boundary_notifier()
{
  return handler.get_ota_symbol_boundary_notifier();
}