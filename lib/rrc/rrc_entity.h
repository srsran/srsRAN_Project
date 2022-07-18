/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RRC_RRC_ENTITY_H
#define SRSGNB_LIB_RRC_RRC_ENTITY_H

#include "srsgnb/rrc/rrc_config.h"
#include "srsgnb/rrc/rrc_entity_interface.h"
#include "ue/rrc_ue_entity.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Main RRC representation with the DU
class rrc_entity : public rrc_entity_du_interface, public rrc_entity_ue_interface
{
public:
  rrc_entity(const rrc_cfg_t& cfg_);
  ~rrc_entity() = default;

  // rrc_entity_du_interface
  rrc_ue_entity_interface* add_user(ue_context& ctxt_) override;
  void                     release_ues() override;

  // rrc_entity_ue_interface
  int  get_pucch_resources() override;
  bool is_amf_connected() override;

private:
  // helpers
  bool             init_pucch();
  const rrc_cfg_t& cfg;

  // RRC-internal user database indexed by ue_index
  slot_array<std::unique_ptr<rrc_ue_entity>, MAX_NOF_UES> ue_db;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_LIB_RRC_RRC_ENTITY_H
