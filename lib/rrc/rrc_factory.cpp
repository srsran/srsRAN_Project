/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/rrc/rrc_factory.h"
#include "ue/rrc_ue_entity.h"

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<rrc_ue_entity_interface>
srsgnb::srs_cu_cp::create_rrc_ue_entity(const rrc_ue_entity_creation_message& msg)
{
  return std::make_unique<rrc_ue_entity>(msg.ue_ctxt, msg.cfg);
}