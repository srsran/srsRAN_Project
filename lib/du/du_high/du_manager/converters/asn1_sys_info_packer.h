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

#include "srsran/adt/byte_buffer.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/mac/cell_configuration.h"

namespace srsran {

struct sib19_info;

namespace srs_du {
namespace asn1_packer {

/// \brief Derive and pack cell MIB based on DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell MIB.
byte_buffer pack_mib(const du_cell_config& du_cfg);

/// \brief Derive and pack cell SIB1 based on DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \param[out] json_string String where the RRC ASN.1 SIB1 is stored in json format. If nullptr, no conversion takes
/// place.
/// \return byte buffer with packed cell SIB1.
byte_buffer pack_sib1(const du_cell_config& du_cfg, std::string* js_str = nullptr);

/// \brief Derive and pack cell SIB19.
byte_buffer pack_sib19(const sib19_info& sib19_params, std::string* js_str = nullptr);

/// \brief Pack all cell BCCH-DL-SCH messages (SIB1 + SI messages) from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return A list of buffers with packed cell BCCH-DL-SCH message. First buffer is SIB1, the rest are SI messages.
std::vector<bcch_dl_sch_payload_type> pack_all_bcch_dl_sch_msgs(const du_cell_config& du_cfg);

} // namespace asn1_packer
} // namespace srs_du
} // namespace srsran
