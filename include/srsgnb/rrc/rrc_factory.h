/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RRC_FACTORY_H
#define SRSGNB_RRC_FACTORY_H

#include "rrc_config.h"
#include "rrc_ue_config.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/rrc/rrc.h"
#include "srsgnb/rrc/rrc_entity_interface.h"
#include "srsgnb/rrc/rrc_ue_entity_interface.h"
#include <map>
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

struct ue_context;

struct rrc_entity_creation_message {
  rrc_entity_creation_message(const rrc_cfg_t& cfg_) : cfg(cfg_) {}
  const rrc_cfg_t& cfg;
};

/// Create an instance of an RRC entity
std::unique_ptr<rrc_entity_du_interface> create_rrc_entity(const rrc_entity_creation_message& msg);

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_RRC_FACTORY_H
