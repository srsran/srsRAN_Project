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
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
class du_processor;

/// Interface to notify about DU connections to the CU-CP
class cu_cp_manager_du_connection_notifier
{
public:
  virtual ~cu_cp_manager_du_connection_notifier() = default;

  /// \brief Notifies the CU-CP manager about a new DU connection.
  virtual void on_new_connection() = 0;
};

/// Interface used to handle DU specific F1AP procedure outcomes (e.g. F1 removal request)
class cu_cp_manager_du_handler
{
public:
  virtual ~cu_cp_manager_du_handler() = default;

  /// \brief Handles a remove request. The corresponding DU processor will be removed.
  /// \param[in] du_index The index of the DU processor to delete.
  virtual void handle_du_remove_request(const du_index_t du_index) = 0;
};

class cu_cp_manager_du_interface
{
public:
  virtual ~cu_cp_manager_du_interface()                     = default;
  virtual void          add_du()                            = 0;
  virtual void          remove_du(du_index_t du_index)      = 0;
  virtual du_index_t    get_next_du_index()                 = 0;
  virtual du_processor* find_du(du_index_t du_index)        = 0;
  virtual du_processor* find_du(uint64_t packed_nr_cell_id) = 0;

  virtual size_t get_nof_dus() const = 0;
  virtual size_t get_nof_ues() const = 0;

  virtual f1c_message_handler* get_f1c_message_handler(const du_index_t du_index) = 0;
};

class cu_cp_manager_interface : public cu_cp_manager_du_connection_notifier,
                                public cu_cp_manager_du_handler,
                                public cu_cp_manager_du_interface

{
public:
  virtual ~cu_cp_manager_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_H
