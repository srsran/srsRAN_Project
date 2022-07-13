/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_PROCESSOR_H
#define SRSGNB_DU_PROCESSOR_H

#include "cu_cp_types.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
struct f1ap_initial_ul_rrc_msg;

struct f1ap_ul_rrc_msg;

struct f1_setup_request_message;

/// Interface used to handle cells
class du_processor_cell_handler
{
public:
  virtual ~du_processor_cell_handler() = default;

  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] packed_nr_cell_id The packed NR cell ID received over F1AP.
  virtual du_cell_index_t find_cell(uint64_t packed_nr_cell_id) = 0;
};

class du_processor_f1ap_setup_handler
{
public:
  virtual ~du_processor_f1ap_setup_handler()                                = default;
  virtual void handle_f1_setup_request(const f1_setup_request_message& msg) = 0;
};

/// Combined interface for all DU processor handlers
class du_processor_f1c_interface : public du_processor_f1ap_setup_handler, public du_processor_cell_handler
{
public:
  virtual ~du_processor_f1c_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_DU_PROCESSOR_H
