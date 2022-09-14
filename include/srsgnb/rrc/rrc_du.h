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

struct rrc_ue_creation_message {
  ue_index_t                      ue_index;
  rnti_t                          c_rnti;
  nr_cell_global_identity         cgi;
  srb_notifiers                   srbs;
  asn1::unbounded_octstring<true> du_to_cu_container;
  rrc_ue_task_scheduler*          ue_task_sched;
};

/// \brief Interface class for the main RRC DU object used by the RRC UE objects.
/// This interface provides PUCCH resources and the RRC connection permission.
class rrc_du_ue_manager
{
public:
  rrc_du_ue_manager()          = default;
  virtual ~rrc_du_ue_manager() = default;

  /// Request the allocation of PUCCH resources for the UE.
  virtual int get_pucch_resources() = 0;

  /// Check if the parent allows RRC connections.
  virtual bool is_rrc_connect_allowed() = 0;
};

/// \brief Interface class to the main RRC DU object to manage RRC UEs.
/// This interface provides functions to add, remove and release UEs.
class rrc_du_ue_repository : public rrc_amf_connection_handler
{
public:
  rrc_du_ue_repository()          = default;
  virtual ~rrc_du_ue_repository() = default;

  /// Creates a new RRC UE object and returns a handle to it.
  virtual rrc_ue_interface* add_ue(rrc_ue_creation_message msg) = 0;

  /// Remove a RRC UE object.
  /// \param[in] ue_index The index of the UE object to remove.
  virtual void remove_ue(ue_index_t ue_index) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void release_ues() = 0;
};

/// Combined entry point for the RRC DU handling.
class rrc_du_interface : public rrc_du_ue_manager, public rrc_du_ue_repository
{
public:
  virtual ~rrc_du_interface() = default;
};

} // namespace srs_cu_cp

} // namespace srsgnb
