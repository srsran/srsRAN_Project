/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_receiver_controller.h"

using namespace srsran;
using namespace ofh;

void receiver_controller::start()
{
  msg_receiver.get_ethernet_receiver().start(msg_receiver);
}

void receiver_controller::stop()
{
  msg_receiver.get_ethernet_receiver().stop();
}
