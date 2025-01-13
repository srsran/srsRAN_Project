/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../asn1_helpers.h"
#include "f1ap_asn1_converters.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "srsran/ran/bcd_helper.h"
#include "srsran/ran/rb_id.h"
#include "srsran/ran/rnti.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Convert the \c cu_cp_paging_message type to ASN.1.
/// \param[out] asn1_paging The ASN.1 type struct to store the result.
/// \param[in] paging The common type Paging message.
inline void fill_asn1_paging_message(asn1::f1ap::paging_s& asn1_paging, const cu_cp_paging_message& paging)
{
  // Add ue id idx value
  uint64_t five_g_s_tmsi = paging.ue_paging_id.to_number();

  // UE Identity Index value is defined as: UE_ID 5G-S-TMSI mod 1024  (see TS 38.304 section 7.1)
  asn1_paging->ue_id_idx_value.set_idx_len10().from_number(five_g_s_tmsi % 1024);

  // Add paging id
  asn1_paging->paging_id.set_cn_ue_paging_id().set_five_g_s_tmsi().from_number(five_g_s_tmsi);

  // Add paging drx
  if (paging.paging_drx.has_value()) {
    asn1_paging->paging_drx_present = true;
    asn1::number_to_enum(asn1_paging->paging_drx, paging.paging_drx.value());
  }

  // Add paging prio
  if (paging.paging_prio.has_value()) {
    asn1_paging->paging_prio_present = true;
    asn1::number_to_enum(asn1_paging->paging_prio, paging.paging_prio.value());
  }

  // Add paging cell list
  for (const auto& cell_item : paging.assist_data_for_paging.value()
                                   .assist_data_for_recommended_cells.value()
                                   .recommended_cells_for_paging.recommended_cell_list) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o> asn1_paging_cell_item_container;
    auto& asn1_paging_cell_item = asn1_paging_cell_item_container->paging_cell_item();

    asn1_paging_cell_item.nr_cgi.nr_cell_id.from_number(cell_item.ngran_cgi.nci.value());
    asn1_paging_cell_item.nr_cgi.plmn_id = cell_item.ngran_cgi.plmn_id.to_bytes();

    asn1_paging->paging_cell_list.push_back(asn1_paging_cell_item_container);
  }

  // Add paging origin
  if (paging.paging_origin.has_value()) {
    asn1_paging->paging_origin_present = true;
    asn1::bool_to_enum(asn1_paging->paging_origin, paging.paging_origin.value());
  }
}

} // namespace srs_cu_cp
} // namespace srsran
