/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "asn1_helpers.h"
#include "srsran/ran/bcd_helper.h"

using namespace srsran;

expected<nr_cell_global_id_t> srsran::cgi_from_asn1(const asn1::f1ap::nr_cgi_s& asn1_cgi)
{
  auto plmn = plmn_identity::from_bytes(asn1_cgi.plmn_id.to_bytes());
  if (not plmn.has_value()) {
    return make_unexpected(plmn.error());
  }
  auto nci = nr_cell_identity::create(asn1_cgi.nr_cell_id.to_number());
  if (not nci.has_value()) {
    return make_unexpected(nci.error());
  }
  return nr_cell_global_id_t{plmn.value(), nci.value()};
}

pdcp_sn_size srsran::pdcp_sn_size_from_f1ap_asn1(const asn1::f1ap::pdcp_sn_len_e& asn1_pdcp_sn_size)
{
  switch (asn1_pdcp_sn_size) {
    case asn1::f1ap::pdcp_sn_len_opts::twelve_bits:
      return pdcp_sn_size::size12bits;
    case asn1::f1ap::pdcp_sn_len_opts::eighteen_bits:
      return pdcp_sn_size::size18bits;
    default:
      return pdcp_sn_size::invalid;
  }
}

asn1::f1ap::pdcp_sn_len_e srsran::pdcp_sn_size_to_f1ap_asn1(pdcp_sn_size sn_size)
{
  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      return asn1::f1ap::pdcp_sn_len_opts::twelve_bits;
    case pdcp_sn_size::size18bits:
      return asn1::f1ap::pdcp_sn_len_opts::eighteen_bits;
    default:
      return asn1::f1ap::pdcp_sn_len_opts::nulltype;
  }
}
