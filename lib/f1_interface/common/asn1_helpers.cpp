/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "asn1_helpers.h"

namespace srsgnb {

nr_cell_global_identity cgi_from_asn1(const asn1::f1ap::nrcgi_s& asn1_cgi)
{
  nr_cell_global_identity cgi          = {};
  uint32_t                encoded_plmn = asn1_cgi.plmn_id.to_number();
  ngap_plmn_to_mccmnc(encoded_plmn, &cgi.mcc, &cgi.mnc);

  std::string mcc_string, mnc_string;
  mcc_to_string(cgi.mcc, &mcc_string);
  mnc_to_string(cgi.mnc, &mnc_string);
  cgi.plmn = mcc_string + mnc_string;

  // Set PLMN hex string
  // Add filler digit if MNC has only 2 digits
  if (mnc_string.size() == 2) {
    cgi.plmn_hex = mcc_string + "f" + mnc_string;
  } else {
    cgi.plmn_hex = cgi.plmn;
  }

  cgi.nci.packed = asn1_cgi.nrcell_id.to_number();
  return cgi;
}

} // namespace srsgnb
