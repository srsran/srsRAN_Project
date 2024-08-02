/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/f1ap/f1ap_pdu_items.h"
#include "srsran/f1ap/common/ue_context_config.h"
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/ran/nr_cgi.h"

namespace srsran {

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

pdcp_sn_size              pdcp_sn_size_from_f1ap_asn1(const asn1::f1ap::pdcp_sn_len_e& asn1_pdcp_sn_size);
asn1::f1ap::pdcp_sn_len_e pdcp_sn_size_to_f1ap_asn1(pdcp_sn_size sn_size);

std::vector<up_transport_layer_info>
make_ul_up_tnl_info_list(const asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l& asn1_list);

/// Convert 3GPP TS 38.473, DRBs-ToBeModified-Item ASN.1 type into f1ap_drb_to_modify.
f1ap_drb_to_modify make_drb_to_modify(const asn1::f1ap::drbs_to_be_modified_item_s& drb_item);

/// Convert F1AP setup/modified types to respective ASN.1 TS 48.473 types.
asn1::f1ap::drbs_setup_list_l     make_drbs_setup_list(span<const f1ap_drb_setupmod> drbs);
asn1::f1ap::drbs_setup_mod_list_l make_drbs_setup_mod_list(span<const f1ap_drb_setupmod> drbs);
asn1::f1ap::drbs_modified_list_l  make_drbs_modified_list(span<const f1ap_drb_setupmod> drbs);

/// Convert F1AP failed to setup/modify types to respective ASN.1 TS 48.473 types.
asn1::f1ap::drbs_failed_to_be_setup_list_l
make_drbs_failed_to_be_setup_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::drbs_failed_to_be_setup_mod_list_l
make_drbs_failed_to_be_setup_mod_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);
asn1::f1ap::drbs_failed_to_be_modified_list_l
make_drbs_failed_to_be_modified_list(span<const f1ap_drb_failed_to_setupmod> failed_drbs);

} // namespace srsran
