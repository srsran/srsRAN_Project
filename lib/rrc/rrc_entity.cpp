/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_entity.h"
#include "../ran/gnb_format.h"

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_entity::rrc_entity(const rrc_cfg_t& cfg_) : cfg(cfg_) {}

rrc_ue_entity_interface* rrc_entity::add_user(ue_context& ctxt_)
{
  if (ue_db.contains(ctxt_.ue_index)) {
    // UE already exists with same ue_index
    return nullptr;
  }

  ue_index_t ue_index = ctxt_.ue_index;
  ue_db.emplace(ue_index, std::make_unique<rrc_ue_entity>(ctxt_, cfg.ue_default_cfg));
  auto& u = ue_db[ue_index];
  return u.get();
}

void rrc_entity::release_ues()
{
  // release all UEs connected to this RRC entity
}

int rrc_entity::get_pucch_resources()
{
  return 0;
}

bool rrc_entity::is_amf_connected()
{
  return false;
}