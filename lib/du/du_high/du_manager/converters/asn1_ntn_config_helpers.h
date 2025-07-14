/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/rrc_nr/sys_info.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {
namespace srs_du {

/// Creates and fills an ASN.1 SIB19 message structure for NTN cell configuration.
/// \param[in] sib19_params The SIB19 configuration parameters to encode.
/// \return The filled ASN.1 SIB19 structure.
asn1::rrc_nr::sib19_r17_s make_asn1_rrc_cell_sib19(const sib19_info& sib19_params);

/// Fills advanced cell NTN-config parameters in ASN.1 SIB19 message.
/// \param[in]  sib19_params content of SIB19 msg.
/// \param[out] out The ASN.1 SIB19 struct to fill.
/// \return A buffer with packed SIB19 as BCCH-DL-SCH message.
void make_asn1_rrc_advanced_cell_sib19(const sib19_info& sib19_params, asn1::rrc_nr::sib19_r17_s& out);

} // namespace srs_du
} // namespace srsran
