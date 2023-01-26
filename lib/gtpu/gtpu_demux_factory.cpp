/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/gtpu/gtpu_demux_factory.h"
#include "gtpu_demux_impl.h"

using namespace srsgnb;

std::unique_ptr<gtpu_demux> srsgnb::create_gtpu_demux(gtpu_demux_creation_message_msg& msg)
{
  return std::make_unique<gtpu_demux_impl>(msg.cu_up_exec);
}
