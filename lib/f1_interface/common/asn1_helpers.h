/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1_COMMON_ASN1_HELPERS_H
#define SRSGNB_F1_COMMON_ASN1_HELPERS_H

#include "../../ran/bcd_helpers.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/ran/nr_cgi.h"

namespace srsgnb {

/// \brief Converts ASN.1 CGI typo into internal struct. It also performs the byte to MCC/MNC conversion.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
nr_cell_global_identity cgi_from_asn1(const asn1::f1ap::nrcgi_s& asn1_cgi)
{
  nr_cell_global_identity cgi          = {};
  uint32_t                encoded_plmn = asn1_cgi.plmn_id.to_number();
  ngap_plmn_to_mccmnc(encoded_plmn, &cgi.mcc, &cgi.mnc);

  std::string mcc_string, mnc_string;
  mcc_to_string(cgi.mcc, &mcc_string);
  mnc_to_string(cgi.mnc, &mnc_string);
  cgi.plmn = mcc_string + mnc_string;

  cgi.nci.packed = asn1_cgi.nrcell_id.to_number();
  return cgi;
}

} // namespace srsgnb

#endif // SRSGNB_F1_COMMON_ASN1_HELPERS_H