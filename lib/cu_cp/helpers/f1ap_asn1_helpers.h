/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor_context.h"
#include "srsran/ran/bcd_helpers.h"
#include <map>

namespace srsran {

namespace srs_cu_cp {

/// \brief Fills ASN.1 F1SetupResponse struct.
/// \param[out] response The F1SetupResponse ASN.1 struct to fill.
/// \param[in] name The gNB CU name
/// \param[in] rrc_version The RRC version
/// \param[in] du_cell_db An array of DU cells to be activated
void fill_asn1_f1_setup_response(asn1::f1ap::f1_setup_resp_s&                      response,
                                 uint16_t                                          transaction_id,
                                 const std::string&                                name,
                                 const uint8_t                                     rrc_version,
                                 const std::map<du_cell_index_t, du_cell_context>& du_cell_db)
{
  // transaction id
  response->transaction_id.value = transaction_id;

  // fill CU common info
  response->gnb_cu_name_present = true;
  response->gnb_cu_name->from_string(name);
  response->gnb_cu_rrc_version.value.latest_rrc_version.from_number(rrc_version);

  // activate all DU cells
  response->cells_to_be_activ_list_present = true;
  for (const auto& du_cell_pair : du_cell_db) {
    const auto& du_cell = du_cell_pair.second;

    asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> resp_cell;
    resp_cell->cells_to_be_activ_list_item().nr_pci_present = true;
    resp_cell->cells_to_be_activ_list_item().nr_pci         = du_cell.pci;
    resp_cell->cells_to_be_activ_list_item().nr_cgi.plmn_id.from_number(plmn_string_to_bcd(du_cell.cgi.plmn));
    resp_cell->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.from_number(du_cell.cgi.nci);
    response->cells_to_be_activ_list.value.push_back(resp_cell);
  }
}

} // namespace srs_cu_cp

} // namespace srsran
