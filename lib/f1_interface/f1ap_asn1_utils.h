/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_ASN1_UTILS_H
#define SRSGNB_F1AP_ASN1_UTILS_H

#include "srsgnb/asn1/f1ap.h"

namespace srsgnb {

/// Get string with F1AP error cause.
inline const char* get_cause_str(const asn1::f1ap::cause_c& cause)
{
  switch (cause.type()) {
    case asn1::f1ap::cause_c::types_opts::radio_network:
      return cause.radio_network().to_string();
    case asn1::f1ap::cause_c::types_opts::transport:
      return cause.transport().to_string();
    case asn1::f1ap::cause_c::types_opts::protocol:
      return cause.protocol().to_string();
    case asn1::f1ap::cause_c::types_opts::misc:
      return cause.misc().to_string();
    default:
      break;
  }
  return "unknown";
}

} // namespace srsgnb

#endif // SRSGNB_F1AP_ASN1_UTILS_H
