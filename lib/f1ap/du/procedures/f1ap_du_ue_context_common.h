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

#include "../../common/asn1_helpers.h"
#include "srsran/asn1/f1ap/f1ap_pdu_items.h"
#include "srsran/f1ap/du/f1ap_du_ue_context_update.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {
namespace srs_du {

/// \brief Creates a \c srb_id_t from ASN.1 type.
///
/// This function is used by the following procedures:
/// - f1ap_du_ue_context_setup_procedure
/// - f1ap_du_ue_context_modification_procedure
///
/// \param srb_item ASN.1 item with SRB-specific parameters to be setup.
/// \return srb_id_t object.
template <typename Asn1Type>
srb_id_t make_srb_id(const Asn1Type& srb_item)
{
  return static_cast<srb_id_t>(srb_item.srb_id);
}

/// Convert 3GPP TS 38.473, DRBs-ToBeSetup-Item ASN.1 type into f1ap_drb_config_request.
f1ap_drb_config_request make_drb_config_request(const asn1::f1ap::drbs_to_be_setup_item_s& drb_item);

/// Convert 3GPP TS 38.473, DRBs-ToBeSetupMod-Item ASN.1 type into f1ap_drb_config_request.
f1ap_drb_config_request make_drb_config_request(const asn1::f1ap::drbs_to_be_setup_mod_item_s& drb_item);

/// Convert 3GPP TS 38.473, DRBs-ToBeModified-Item ASN.1 type into f1ap_drb_config_request.
f1ap_drb_config_request make_drb_config_request(const asn1::f1ap::drbs_to_be_modified_item_s& drb_item);

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

/// \brief Creates a \c drb_id_t from ASN.1 type.
///
/// This function is used by the following procedures:
/// - f1ap_du_ue_context_modification_procedure
///
/// \param drb_item ASN.1 item with DRB-specific parameters to be removed.
/// \return drb_id_t object.
template <typename Asn1Type>
drb_id_t make_drb_id(const Asn1Type& drb_item)
{
  return static_cast<drb_id_t>(drb_item.drb_id);
}

} // namespace srs_du
} // namespace srsran
