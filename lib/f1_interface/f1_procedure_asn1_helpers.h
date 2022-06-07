/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1_PROCEDURE_ASN1_HELPERS_H
#define SRSGNB_F1_PROCEDURE_ASN1_HELPERS_H

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/f1ap_du.h"

namespace srsgnb {

/// \brief Fills ASN.1 F1SetupRequest struct.
/// \param[out] request The F1SetupRequest ASN.1  struct to fill.
/// \param[in] du_params DU setup parameters to add to the F1SetupRequest.
void fill_asn1_f1_setup_request(asn1::f1ap::f1_setup_request_s&         request,
                                const srs_du::f1_setup_request_message& request_msg)
{
  // TODO: Add other inputs and set values accordingly

  request->gnb_du_id.value = request_msg.setup_params.gnb_du_id;
  request->gnb_du_name.value.from_string(request_msg.setup_params.gnb_du_name);
  request->gnb_du_rrc_version.value.latest_rrc_version.from_number(request_msg.setup_params.rrc_version);
}

} // namespace srsgnb

#endif // SRSGNB_F1_PROCEDURE_ASN1_HELPERS_H