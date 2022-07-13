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
#include "srsgnb/f1_interface/cu/f1ap_cu.h"

namespace srsgnb {

namespace srs_cu_cp {

struct ue_manager_initial_ul_rrc_message {
  ue_index_t              ue_index;
  du_cell_index_t         pcell_index;
  f1ap_initial_ul_rrc_msg msg;
};

struct ue_manager_ul_rrc_message {
  ue_index_t      ue_idx;
  f1ap_ul_rrc_msg msg;
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

/// Interface to support UE creation
class ue_manager_ue_index_handler
{
public:
  virtual ~ue_manager_ue_index_handler() = default;

  /// \brief Get the next available UE index.
  /// \return The UE index.
  virtual ue_index_t get_next_ue_index() = 0;
};

/// Interface to forward RRC messages at the UE manager
class ue_manager_rrc_message_handler
{
public:
  virtual ~ue_manager_rrc_message_handler() = default;

  virtual void handle_initial_ul_rrc_message_transfer(const ue_manager_initial_ul_rrc_message& msg) = 0;
  virtual void handle_ul_rrc_message_transfer(const ue_manager_ul_rrc_message& msg)                 = 0;
};

/// Combined interface for all UE manager f1c handlers
class ue_manager_f1c_handler : public ue_manager_ue_index_handler, public ue_manager_rrc_message_handler
{
public:
  virtual ~ue_manager_f1c_handler() = default;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_UE_MANAGER_INTERFACES_H
