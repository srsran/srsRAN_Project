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

#include "srsran/asn1/f1ap/f1ap_pdu_items.h"
#include "srsran/f1ap/ue_context_management_configs.h"
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/ran/nr_cgi.h"

namespace srsran {

/// \brief Convert F1AP ASN.1 Cause to \c f1ap_cause_t type.
/// \param asn1_cause The F1AP Cause.
/// \return The f1ap_cause_t type.
f1ap_cause_t asn1_to_cause(asn1::f1ap::cause_c asn1_cause);

/// \brief Convert \c f1ap_cause_t type to F1AP cause.
/// \param cause The f1ap_cause_t type.
/// \return The F1AP cause.
asn1::f1ap::cause_c cause_to_asn1(f1ap_cause_t cause);

/// \brief Extracts a \c drb_id_t from ASN.1 DRB Setup/Modified/Remove type.
/// \param drb_item ASN.1 item with DRB-Id.
/// \return drb_id_t object.
template <typename Asn1Type>
drb_id_t get_drb_id(const Asn1Type& drb_item)
{
  return static_cast<drb_id_t>(drb_item.drb_id);
}

/// \brief Converts ASN.1 CGI typo into internal struct. It also performs the byte to MCC/MNC conversion.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
expected<nr_cell_global_id_t> cgi_from_asn1(const asn1::f1ap::nr_cgi_s& asn1_cgi);
asn1::f1ap::nr_cgi_s          cgi_to_asn1(const nr_cell_global_id_t& cgi);

/// \brief Convert F1AP ASN.1 NR-CGI to NR Cell Identity.
/// \param f1ap_cgi The F1AP NR-CGI.
/// \return The NR Cell Identity.
expected<nr_cell_identity> nr_cell_id_from_asn1(asn1::f1ap::nr_cgi_s& f1ap_cgi);

f1ap_srb_to_setup                       make_srb_to_setup(const asn1::f1ap::srbs_to_be_setup_item_s& srb_item);
asn1::f1ap::srbs_to_be_setup_list_l     make_srb_to_setup_list(span<const f1ap_srb_to_setup> srbs);
asn1::f1ap::srbs_to_be_setup_mod_list_l make_srb_to_setupmod_list(span<const f1ap_srb_to_setup> srbs);

/// \brief Conversion helpers between SRB setup/modified common types and respective ASN.1 TS 48.473 types.
asn1::f1ap::srbs_setup_list_l     make_srb_setup_list(span<const srb_id_t> srbs);
asn1::f1ap::srbs_setup_mod_list_l make_srb_setupmod_list(span<const srb_id_t> srbs);

f1ap_srb_failed_to_setup make_srb_failed_to_setupmod(const asn1::f1ap::srbs_failed_to_be_setup_item_s& asn1_type);
f1ap_srb_failed_to_setup make_srb_failed_to_setupmod(const asn1::f1ap::srbs_failed_to_be_setup_mod_item_s& asn1_type);

/// \brief Conversion helpers between DRB to_setup/to_modify common types and respective ASN.1 TS 48.473 types.
f1ap_drb_to_setup                       make_drb_to_setup(const asn1::f1ap::drbs_to_be_setup_item_s& drb_item);
f1ap_drb_to_setup                       make_drb_to_setup(const asn1::f1ap::drbs_to_be_setup_mod_item_s& drb_item);
f1ap_drb_to_modify                      make_drb_to_modify(const asn1::f1ap::drbs_to_be_modified_item_s& drb_item);
asn1::f1ap::drbs_to_be_setup_item_s     make_drb_to_setup(const f1ap_drb_to_setup& drb_item);
asn1::f1ap::drbs_to_be_setup_mod_item_s make_drb_to_setupmod(const f1ap_drb_to_setup& drb_item);
asn1::f1ap::drbs_to_be_modified_item_s  make_drb_to_mod(const f1ap_drb_to_modify& drb_item);
asn1::f1ap::drbs_to_be_setup_list_l     make_drb_to_setup_list(span<const f1ap_drb_to_setup> drb_list);
asn1::f1ap::drbs_to_be_setup_mod_list_l make_drb_to_setupmod_list(span<const f1ap_drb_to_setup> drb_list);
asn1::f1ap::drbs_to_be_modified_list_l  make_drb_to_modify_list(span<const f1ap_drb_to_modify> drb_list);

/// Conversion helpers between DRB setup/setupmod/modified common types and respective ASN.1 TS 48.473 types.
asn1::f1ap::drbs_setup_list_l     make_drbs_setup_list(span<const f1ap_drb_setupmod> drbs);
asn1::f1ap::drbs_setup_mod_list_l make_drbs_setup_mod_list(span<const f1ap_drb_setupmod> drbs);
asn1::f1ap::drbs_modified_list_l  make_drbs_modified_list(span<const f1ap_drb_setupmod> drbs);
f1ap_drb_setupmod                 make_drb_setupmod(const asn1::f1ap::drbs_setup_item_s& drb);
f1ap_drb_setupmod                 make_drb_setupmod(const asn1::f1ap::drbs_setup_mod_item_s& drb);
f1ap_drb_setupmod                 make_drb_setupmod(const asn1::f1ap::drbs_modified_item_s& drb);

/// Convert F1AP failed to setup/modify types to respective ASN.1 TS 48.473 types.
asn1::f1ap::drbs_failed_to_be_setup_list_l
make_drbs_failed_to_be_setup_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::drbs_failed_to_be_setup_mod_list_l
make_drbs_failed_to_be_setup_mod_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::drbs_failed_to_be_modified_list_l
make_drbs_failed_to_be_modified_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::serving_cell_mo_encoded_in_cgc_list_l
                            make_serving_cell_mo_encoded_in_cgc_list(span<const uint8_t> serving_cell_mos);
f1ap_drb_failed_to_setupmod make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_setup_item_s& asn1_type);
f1ap_drb_failed_to_setupmod
make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_setup_mod_item_s& asn1_type);
f1ap_drb_failed_to_setupmod make_drb_failed_to_setupmod(const asn1::f1ap::drbs_failed_to_be_modified_item_s& asn1_type);

} // namespace srsran
