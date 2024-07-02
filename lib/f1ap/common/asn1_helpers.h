/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/ran/nr_cgi.h"

namespace srsran {

/// \brief Converts ASN.1 CGI typo into internal struct. It also performs the byte to MCC/MNC conversion.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
expected<nr_cell_global_id_t> cgi_from_asn1(const asn1::f1ap::nr_cgi_s& asn1_cgi);

pdcp_sn_size              pdcp_sn_size_from_f1ap_asn1(const asn1::f1ap::pdcp_sn_len_e& asn1_pdcp_sn_size);
asn1::f1ap::pdcp_sn_len_e pdcp_sn_size_to_f1ap_asn1(pdcp_sn_size sn_size);

} // namespace srsran
