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

#include "srsgnb/cu_cp/ue_context.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Interface class for the main RRC object of the DU used by the RRC UE objects.
class rrc_entity_ue_interface
{
public:
  rrc_entity_ue_interface()          = default;
  virtual ~rrc_entity_ue_interface() = default;

  /// Request the allocation of PUCCH resources for the UE.
  virtual int get_pucch_resources() = 0;
  /// Check if the parent allows RRC connections.
  virtual bool is_rrc_connect_allowed() = 0;
};

/// Interface class to the main RRC object of the DU used by the DU itself.
class rrc_entity_du_interface : public rrc_amf_connection_handler
{
public:
  rrc_entity_du_interface()          = default;
  virtual ~rrc_entity_du_interface() = default;

  /// Creates a new RRC UE object and returns a handle to it.
  virtual rrc_ue_entity_interface* add_user(ue_context& ctxt_) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void release_ues() = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb
