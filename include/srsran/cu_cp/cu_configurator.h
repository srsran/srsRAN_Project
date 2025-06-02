/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/qos/qos_flow_id.h"
#include <map>

namespace srsran {

namespace srs_cu_cp {
class du_processor_repository;
class cu_cp_mobility_command_handler;
} // namespace srs_cu_cp

struct cu_handover_control_config {
  nr_cell_global_id_t                                    target_cell_id;
  std::map<pdu_session_id_t, std::vector<qos_flow_id_t>> pdu_sessions_for_ho;
  std::map<drb_id_t, std::vector<qos_flow_id_t>>         drbs_for_ho;
  std::vector<nr_cell_global_id_t>                       secondary_cells_to_be_added;
};

class cu_configurator
{
public:
  virtual ~cu_configurator() = default;

  /// Get UE index for a given GNB-CU-UE-F1AP-ID.
  virtual srs_cu_cp::ue_index_t get_ue_index(const srs_cu_cp::amf_ue_id_t& amf_ue_id,
                                             const srs_cu_cp::guami_t&     guami,
                                             const gnb_cu_ue_f1ap_id_t&    gnb_cu_ue_f1ap_id) const = 0;

  /// Get index of the DU that serves the given UE.
  virtual srs_cu_cp::du_index_t get_du_index(const srs_cu_cp::ue_index_t& ue_index) const = 0;

  /// Get DU index for a given NR Cell Global ID.
  virtual srs_cu_cp::du_index_t get_du_index(const nr_cell_global_id_t& nr_cgi) const = 0;

  /// Get PCI for a given NR Cell Global ID.
  virtual pci_t get_pci(const nr_cell_global_id_t& nr_cgi) const = 0;

  /// Trigger UE Handover.
  virtual async_task<srs_cu_cp::cu_cp_intra_cu_handover_response>
  trigger_handover(const srs_cu_cp::du_index_t&                      source_du_index,
                   const srs_cu_cp::cu_cp_intra_cu_handover_request& handover_req) = 0;
};

} // namespace srsran
