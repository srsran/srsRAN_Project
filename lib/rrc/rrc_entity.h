/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/rrc/rrc_config.h"
#include "srsgnb/rrc/rrc_entity_interface.h"
#include "ue/rrc_ue_entity.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Main RRC representation with the DU
class rrc_entity : public rrc_entity_du_interface, public rrc_entity_ue_interface
{
public:
  rrc_entity(const rrc_cfg_t&              cfg_,
             rrc_ue_du_processor_notifier& rrc_ue_du_proc_notif_,
             rrc_ue_nas_notifier&          nas_notif_);
  ~rrc_entity() = default;

  // rrc_entity_du_interface
  rrc_ue_entity_interface* add_user(rrc_ue_creation_message msg) override;
  void                     remove_ue(ue_index_t ue_index) override;
  void                     release_ues() override;
  void                     handle_amf_connection() override;
  void                     handle_amf_connection_drop() override;

  // rrc_entity_ue_interface
  int  get_pucch_resources() override;
  bool is_rrc_connect_allowed() override;

private:
  // helpers
  bool             init_pucch();
  const rrc_cfg_t& cfg;

  bool reject_users = true; ///< Reject all connection attempts, i.e. when AMF is not connected.

  rrc_ue_du_processor_notifier& rrc_ue_du_proc_notifier; // notifier to the DU processor
  rrc_ue_nas_notifier&          nas_notifier;            // notifier to the NGAP

  // RRC-internal user database indexed by ue_index
  slot_array<std::unique_ptr<rrc_ue_entity>, MAX_NOF_UES> ue_db;
};

} // namespace srs_cu_cp

} // namespace srsgnb
