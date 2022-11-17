/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/gtpu/gtpu_factory.h"
#include "gtpu_entity_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

std::unique_ptr<gtpu_entity> srsgnb::create_gtpu_entity(gtpu_entity_creation_message& msg)
{
  return std::make_unique<gtpu_entity_impl>(msg.ue_index, msg.cfg, *msg.dl_lower, *msg.ul_upper);
}
