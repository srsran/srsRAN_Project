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

using namespace srsgnb;

nr_cell_global_id_t srsgnb::cgi_from_asn1(const asn1::f1ap::nr_cgi_s& asn1_cgi)
{
  nr_cell_global_id_t cgi          = {};
  uint32_t            encoded_plmn = asn1_cgi.plmn_id.to_number();
  ngap_plmn_to_mccmnc(encoded_plmn, &cgi.mcc, &cgi.mnc);

  std::string mcc_string, mnc_string;
  mcc_to_string(cgi.mcc, &mcc_string);
  mnc_to_string(cgi.mnc, &mnc_string);
  cgi.plmn = mcc_string + mnc_string;

  // Set PLMN hex string
  cgi.plmn_hex = asn1_cgi.plmn_id.to_string();

  cgi.nci.packed = asn1_cgi.nr_cell_id.to_number();

  cgi.nci.cell_idenity = (asn1_cgi.nr_cell_id.to_number() & 0xfffc0000); // bits 4 to 14
  cgi.nci.gnb_identity = (asn1_cgi.nr_cell_id.to_number() & 0X3ff0);     // bits 22 to 32

  return cgi;
}
