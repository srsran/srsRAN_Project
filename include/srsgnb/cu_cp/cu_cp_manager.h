/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_MANAGER_H
#define SRSGNB_CU_CP_MANAGER_H

#include "srsgnb/ran/cu_cp_types.h"
#include <string>

namespace srsgnb {

struct ul_rrc_message_transfer_message;

struct cu_cp_ue_delete_message {
  cu_cp_ue_index_t ue_index;
};

/// Interface used to handle external events (e.g. UL RRC Message Transfer).
class cu_cp_manager_ccch_handler
{
public:
  virtual ~cu_cp_manager_ccch_handler()                                                   = default;
  virtual void handle_ul_rrc_message_transfer(const ul_rrc_message_transfer_message& msg) = 0;
};

class cu_cp_manager_interface_query
{
public:
  virtual ~cu_cp_manager_interface_query() = default;
  virtual size_t nof_ues()                 = 0;
};

class cu_cp_manager_interface : public cu_cp_manager_interface_query, public cu_cp_manager_ccch_handler
{
public:
  virtual ~cu_cp_manager_interface() = default;
};

} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_H
