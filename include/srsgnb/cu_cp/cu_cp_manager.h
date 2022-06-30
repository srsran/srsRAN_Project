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

#include "cu_cp_types.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
struct f1ap_initial_ul_rrc_msg;

struct f1ap_ul_rrc_msg;

struct f1_setup_request_message;

struct ue_delete_message {
  ue_index_t ue_index;
};

/// Interface used to handle external events (e.g. UL RRC Message Transfer).
class cu_cp_manager_ccch_handler
{
public:
  virtual ~cu_cp_manager_ccch_handler()                                                   = default;
  virtual void handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg) = 0;
  virtual void handle_ul_rrc_message_transfer(const f1ap_ul_rrc_msg& msg)                 = 0;
};

class cu_cp_manager_f1ap_setup_handler
{
public:
  virtual ~cu_cp_manager_f1ap_setup_handler()                               = default;
  virtual void handle_f1_setup_request(const f1_setup_request_message& msg) = 0;
};

class cu_cp_manager_interface_query
{
public:
  virtual ~cu_cp_manager_interface_query() = default;
  virtual size_t get_nof_dus() const       = 0;
  virtual size_t get_nof_ues() const       = 0;
};

/// Combined interface for all F1C handlers
class cu_cp_manager_f1c_interface : public cu_cp_manager_f1ap_setup_handler, public cu_cp_manager_ccch_handler
{
public:
  virtual ~cu_cp_manager_f1c_interface() = default;
};

class cu_cp_manager_interface : public cu_cp_manager_interface_query, public cu_cp_manager_f1c_interface
{
public:
  virtual ~cu_cp_manager_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_H
