/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_sector_controller.h"

using namespace srsran;
using namespace ofh;

void sector_controller::start()
{
  ofh_rx.get_operation_controller().start();
}

void sector_controller::stop()
{
  ofh_rx.get_operation_controller().stop();
  slot_repo->clear();
}
