/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/ran/bcd_helpers.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief Fills ASN.1 F1SetupResponse struct.
/// \param[out] response The F1SetupResponse ASN.1 struct to fill.
/// \param[in] name The gNB CU name
/// \param[in] rrc_version The RRC version
/// \param[in] du_cell_db An array of DU cells to be activated
void fill_asn1_f1_setup_response(asn1::f1ap::f1_setup_resp_s&                               response,
                                 const std::string&                                         name,
                                 const uint8_t                                              rrc_version,
                                 const slotted_id_vector<du_cell_index_t, du_cell_context>& du_cell_db)
{
  // fill CU common info
  response->gnb_cu_name_present = true;
  response->gnb_cu_name->from_string(name);
  response->gnb_cu_rrc_version.value.latest_rrc_version.from_number(rrc_version);

  // activate all DU cells
  response->cells_to_be_activ_list_present = true;
  for (const auto& du_cell : du_cell_db) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> resp_cell;
    resp_cell->cells_to_be_activ_list_item().nr_pci_present = true;
    resp_cell->cells_to_be_activ_list_item().nr_pci         = du_cell.pci;
    // TODO: write converter to asn1::f1ap::nrcgi_s
    // resp_cell->cells_to_be_activ_list_item().nrcgi = to_nrcgi_s(du_cell.cgi);
    response->cells_to_be_activ_list.value.push_back(resp_cell);
  }
}

} // namespace srs_cu_cp

} // namespace srsran
