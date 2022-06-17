/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_DU_MANAGER_CONVERTERS_F1C_ASN1_HELPERS_H
#define SRSGNB_LIB_DU_MANAGER_CONVERTERS_F1C_ASN1_HELPERS_H

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"

namespace srsgnb {

namespace srs_du {

/// \brief Fills ASN.1 F1SetupRequest struct.
/// \param[out] request The F1 setup request message struct to fill.
/// \param[in] setup_params DU setup parameters to add to the F1SetupRequest.
void fill_asn1_f1_setup_request(asn1::f1ap::f1_setup_request_s& request, const du_setup_params& setup_params)
{
  // TODO: Add other inputs and set values accordingly

  request->gnb_du_id.value = setup_params.gnb_du_id;
  request->gnb_du_name.value.from_string(setup_params.gnb_du_name);
  request->gnb_du_rrc_version.value.latest_rrc_version.from_number(setup_params.rrc_version);
}

} // namespace srs_du

} // namespace srsgnb

#endif // SRSGNB_LIB_DU_MANAGER_CONVERTERS_F1C_ASN1_HELPERS_H