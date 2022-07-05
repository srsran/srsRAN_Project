/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_MANAGER_INTERFACES_H
#define SRSGNB_CU_CP_MANAGER_INTERFACES_H

#include "du_context.h"
#include "ue_context.h"

namespace srsgnb {

namespace srs_cu_cp {

class du_processor;

class ue_manager_ctrl_configurer
{
public:
  virtual ~ue_manager_ctrl_configurer() = default;

  virtual ue_context* add_ue(ue_context u)           = 0;
  virtual void        remove_ue(ue_index_t ue_index) = 0;
  virtual ue_index_t  get_next_ue_index()            = 0;
  virtual ue_context* find_ue(ue_index_t ue_index)   = 0;
};

class du_manager_ctrl_configurer
{
public:
  virtual ~du_manager_ctrl_configurer() = default;

  virtual du_processor*    add_du(std::unique_ptr<du_processor> du) = 0;
  virtual void             remove_du(du_index_t du_index)        = 0;
  virtual du_index_t       get_next_du_index()                   = 0;
  virtual du_processor*    find_du(du_index_t du_index)             = 0;
  virtual du_processor*    find_du(uint64_t packed_nr_cell_id)      = 0;
  virtual size_t           get_nof_dus() const                   = 0;

  virtual void handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg) = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_INTERFACES_H
