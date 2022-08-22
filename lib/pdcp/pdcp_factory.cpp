/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/pdcp/pdcp_factory.h"
#include "pdcp_entity_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

std::unique_ptr<pdcp_entity> srsgnb::create_pdcp_entity(pdcp_entity_creation_message& msg)
{
  return std::make_unique<pdcp_entity_impl>(
      msg.ue_index, msg.lcid, msg.config, *msg.tx_lower, *msg.tx_upper_cn, *msg.rx_upper_dn, *msg.timers);
}
