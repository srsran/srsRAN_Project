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

#include "adapters/f1ap_adapters.h"
#include "du_context.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/ran/nr_cgi.h"
#include "ue_manager.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

class du_processor : public du_processor_f1c_interface
{
public:
  du_processor(cu_cp_manager_config_t& cfg);
  ~du_processor() = default;

  // message handlers
  void handle_f1_setup_request(const f1_setup_request_message& msg) override;
  void handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg) override;

  // getter functions
  slot_array<du_cell_context, MAX_NOF_DU_CELLS>& get_cell_db() { return cell_db; };
  du_context&                                    get_context() { return context; };
  f1c_message_handler*                           get_f1c_message_handler() { return f1ap.get(); };
  du_cell_index_t                                get_next_du_cell_index();
  size_t                                         get_nof_ues() { return ue_mng.get_nof_ues(); };

private:
  // F1AP senders

  /// \brief Create and transmit the F1 Setup response message.
  /// \param[in] du_ctxt The context of the DU that should receive the message.
  void send_f1_setup_response(const du_context& du_ctxt);

  /// \brief Create and transmit the F1 Setup failure message.
  /// \param[in] cause The cause of the failure.
  void send_f1_setup_failure(asn1::f1ap::cause_c::types::options cause);

  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] packed_nr_cell_id The packed NR cell ID received over F1AP.
  du_cell_index_t find_cell(uint64_t packed_nr_cell_id);

  srslog::basic_logger&   logger;
  cu_cp_manager_config_t& cfg;

  du_context                                    context;
  slot_array<du_cell_context, MAX_NOF_DU_CELLS> cell_db; /// flattened version of served cells list provided by DU/F1AP
  std::atomic<uint16_t>                         next_du_cell_index{0};

  // Components
  std::unique_ptr<f1_interface> f1ap;

  ue_manager ue_mng;

  du_processor_f1ap_event_indicator f1ap_ev_notifier;
  ue_manager_f1ap_event_indicator   ue_manager_f1ap_ev_notifier;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_DU_PROCESSOR_H
