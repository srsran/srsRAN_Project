/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_DU_PROCESSOR_H
#define SRSGNB_CU_CP_DU_PROCESSOR_H

//#include "du_context.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/ran/nr_cgi.h"
#include "ue_manager.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

class du_processor
{
public:
  du_processor(cu_cp_manager_config_t& cfg);
  ~du_processor() = default;

  // message handlers
  void handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg);

  // getter functions
  slot_array<du_cell_context, MAX_NOF_DU_CELLS>& get_cell_db() { return cell_db; };
  du_context&                                    get_context() { return context; }

private:
  /// \brief Send RRCSetup message via DL RRC message transfer (TS 38.473 section 8.4.2), as response to an initial UL
  /// RRC message transfer.
  /// \param[in] msg The received initial UL RRC message transfer.
  void send_rrc_setup(const f1ap_initial_ul_rrc_msg& msg);

  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] The packed NR cell ID received over F1AP.
  du_cell_index_t find_cell(uint64_t packed_nr_cell_id);

  srslog::basic_logger&   logger;
  cu_cp_manager_config_t& cfg;

  du_context                                    context;
  slot_array<du_cell_context, MAX_NOF_DU_CELLS> cell_db; /// flattened version of served cells list provided by DU/F1AP

  ue_manager ue_mng;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_DU_PROCESSOR_H
