/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {

/// \brief Converts ASN.1 CGI typo into internal struct. It also performs the byte to MCC/MNC conversion.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
nr_cell_global_id_t cgi_from_asn1(const asn1::f1ap::nr_cgi_s& asn1_cgi);

} // namespace srsran
