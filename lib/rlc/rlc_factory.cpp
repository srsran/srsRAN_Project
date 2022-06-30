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

std::unique_ptr<rlc_entity> srsgnb::create_rlc_entity(const rlc_entity_config& config)
{
  switch (config.mode) {
    case rlc_mode::TM:
      return std::make_unique<rlc_tm_entity>(config.ue_index, config.lcid, *config.upper_dn, *config.upper_cn);
    case rlc_mode::UM:
      return std::make_unique<rlc_um_entity>(config.ue_index, config.lcid, *config.upper_dn, *config.upper_cn);
    case rlc_mode::AM:
    default:
      srsran_terminate("RLC mode not supported");
  }
  return nullptr;
}
