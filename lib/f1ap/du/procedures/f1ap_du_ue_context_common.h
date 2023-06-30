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

#include "srsran/f1ap/du/f1ap_du_ue_context_update.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {
namespace srs_du {

/// \brief Creates a \c srb_id_t from ASN.1 type
///
/// This function is used by the following procedures
/// - f1ap_du_ue_context_setup_procedure
/// - f1ap_du_ue_context_modification_procedure
///
/// \param srb_item ASN.1 item with SRB-specific parameters to be setup
/// \return srb_id_t object
template <typename Asn1Type>
srb_id_t make_srb_id(const Asn1Type& srb_item)
{
  return (srb_id_t)srb_item.srb_id;
}

/// \brief Creates a \c f1ap_drb_to_setup from ASN.1 type
///
/// This function is used by the following procedures
/// - f1ap_du_ue_context_setup_procedure
/// - f1ap_du_ue_context_modification_procedure
///
/// \param drb_item ASN.1 item with DRB-specific parameters to be setup
/// \return f1ap_drb_to_setup object
template <typename Asn1Type>
f1ap_drb_to_setup make_drb_to_setup(const Asn1Type& drb_item)
{
  f1ap_drb_to_setup drb_obj;
  drb_obj.drb_id = (drb_id_t)drb_item.drb_id;
  drb_obj.mode   = (drb_rlc_mode)(unsigned)drb_item.rlc_mode;
  drb_obj.uluptnl_info_list.resize(drb_item.ul_up_tnl_info_to_be_setup_list.size());
  for (unsigned j = 0; j != drb_obj.uluptnl_info_list.size(); ++j) {
    drb_obj.uluptnl_info_list[j] =
        asn1_to_up_transport_layer_info(drb_item.ul_up_tnl_info_to_be_setup_list[j].ul_up_tnl_info);
  }
  drb_obj.five_qi = uint_to_five_qi(
      drb_item.qos_info.choice_ext().value().drb_info().drb_qos.qos_characteristics.non_dyn_5qi().five_qi);
  return drb_obj;
}

/// \brief Creates a \c drb_id_t from ASN.1 type
///
/// This function is used by the following procedures
/// - f1ap_du_ue_context_modification_procedure
///
/// \param drb_item ASN.1 item with DRB-specific parameters to be removed
/// \return drb_id_t object
template <typename Asn1Type>
drb_id_t make_drb_id(const Asn1Type& drb_item)
{
  return (drb_id_t)drb_item.drb_id;
}

} // namespace srs_du
} // namespace srsran
