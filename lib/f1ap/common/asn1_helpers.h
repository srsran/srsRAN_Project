/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/ran/bcd_helpers.h"
#include "srsgnb/ran/nr_cgi.h"
#include "srsgnb/ran/up_transport_layer_info.h"

namespace srsgnb {

/// \brief Converts ASN.1 CGI typo into internal struct. It also performs the byte to MCC/MNC conversion.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
nr_cell_global_id_t cgi_from_asn1(const asn1::f1ap::nrcgi_s& asn1_cgi);

} // namespace srsgnb
