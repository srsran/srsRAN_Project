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

#include "cu_cp_ue_context.h"

namespace srsgnb {

class cu_cp_ue_manager_ctrl_configurer
{
public:
  virtual ~cu_cp_ue_manager_ctrl_configurer()                    = default;
  virtual cu_cp_ue_context* add_ue(cu_cp_ue_context u)           = 0;
  virtual void              remove_ue(cu_cp_ue_index_t ue_index) = 0;
  virtual cu_cp_ue_context* find_ue(cu_cp_ue_index_t ue_index)   = 0;
};

} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_INTERFACES_H
