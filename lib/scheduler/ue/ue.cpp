/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue.h"

using namespace srsgnb;

void ue::handle_sr_indication(const sr_indication_message& msg)
{
  last_sr = msg;
}
