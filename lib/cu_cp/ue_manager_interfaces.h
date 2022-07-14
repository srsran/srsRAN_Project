/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_UE_MANAGER_INTERFACES_H
#define SRSGNB_CU_CP_UE_MANAGER_INTERFACES_H

#include "du_context.h"
#include "srsgnb/cu_cp/ue_context.h"

namespace srsgnb {

namespace srs_cu_cp {

struct ue_manager_initial_ul_rrc_message {
  du_cell_index_t                           pcell_index;
  asn1::unbounded_octstring<true>           rrc_container;
  asn1::unbounded_octstring<true>           du_to_cu_rrc_container;
  rnti_t                                    c_rnti;
  optional<asn1::unbounded_octstring<true>> rrc_container_rrc_setup_complete;
};

struct ue_manager_ul_rrc_message {
  ue_index_t                      ue_idx;
  asn1::unbounded_octstring<true> rrc_container;
};

// Forward declared class
class du_processor;

class ue_manager_ctrl_configurer
{
public:
  virtual ~ue_manager_ctrl_configurer() = default;

  virtual ue_context* add_ue(ue_context u)           = 0;
  virtual void        remove_ue(ue_index_t ue_index) = 0;
  virtual ue_context* find_ue(ue_index_t ue_index)   = 0;
  virtual ue_context* find_rnti(rnti_t rnti)         = 0;
  virtual size_t      get_nof_ues()                  = 0;
};

/// Interface to forward RRC messages at the UE manager
class ue_manager_rrc_message_handler
{
public:
  virtual ~ue_manager_rrc_message_handler() = default;

  virtual ue_index_t handle_initial_ul_rrc_message_transfer(const ue_manager_initial_ul_rrc_message& msg) = 0;
  virtual void       handle_ul_rrc_message_transfer(const ue_manager_ul_rrc_message& msg)                 = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_UE_MANAGER_INTERFACES_H
