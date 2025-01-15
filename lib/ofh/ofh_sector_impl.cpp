/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_sector_impl.h"

using namespace srsran;
using namespace ofh;

receiver& sector_impl::get_receiver()
{
  return *ofh_receiver;
}

transmitter& sector_impl::get_transmitter()
{
  return *ofh_transmitter;
}

controller& sector_impl::get_controller()
{
  return ofh_sector_controller;
}

void sector_impl::set_error_notifier(error_notifier& notifier)
{
  ofh_transmitter->set_error_notifier(notifier);
}
