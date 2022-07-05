/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/rlc/rlc_factory.h"
#include "rlc_tm_entity.h"
#include "rlc_um_entity.h"

using namespace srsgnb;

std::unique_ptr<rlc_entity> srsgnb::create_rlc_entity(const rlc_entity_creation_message& msg)
{
  switch (msg.config.mode) {
    case rlc_mode::tm:
      return std::make_unique<rlc_tm_entity>(msg.ue_index, msg.lcid, *msg.upper_dn, *msg.upper_cn);
    case rlc_mode::um_unidir_dl:
    case rlc_mode::um_unidir_ul:
    case rlc_mode::um_bidir:
      srsran_assert(msg.config.um != nullptr, "RLC UM selected, but msg.config.um is nullptr");
      return std::make_unique<rlc_um_entity>(
          msg.ue_index, msg.lcid, *msg.config.um, *msg.upper_dn, *msg.upper_cn, *msg.timers);
    case rlc_mode::am:
    default:
      srsran_terminate("RLC mode not supported");
  }
  return nullptr;
}
