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

#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/f1ap/du/f1ap_du.h"

namespace srsgnb {

namespace srs_du {

/// \brief Derive packed cell MIB from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell MIB.
byte_buffer make_asn1_rrc_cell_mib_buffer(const du_cell_config& du_cfg);

/// \brief Derive packed cell SIB1 from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \param[out] json_string String where the RRC ASN.1 SIB1 is stored in json format. If nullptr, no conversion takes
/// place.
/// \return byte buffer with packed cell SIB1.
byte_buffer make_asn1_rrc_cell_sib1_buffer(const du_cell_config& du_cfg, std::string* js_str = nullptr);

/// \brief Derive packed cell BCCH-DL-SCH message from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell BCCH-DL-SCH message.
byte_buffer make_asn1_rrc_cell_bcch_dl_sch_msg(const du_cell_config& du_cfg);

/// \brief Fills ASN.1 F1SetupRequest struct.
/// \param[out] request The F1 setup request message struct to fill.
/// \param[in] setup_params DU setup parameters to add to the F1SetupRequest.
/// \param[in] cells_to_add Configurations of cells to add to F1SetupRequest.
/// \param[out] sib_jsons logger Logger used to log RRC ASN.1 SIB1 messages of the DU cells.
void fill_asn1_f1_setup_request(asn1::f1ap::f1_setup_request_s& request,
                                const du_setup_params&          setup_params,
                                span<const du_cell_config*>     cells_to_add,
                                std::vector<std::string>*       cell_json_strs = nullptr);

/// \brief Derive packed cell PCCH-PCH Paging message.
/// \param[in] five_g_s_tmsi 5G-S-TMSI assigned by AMF to UE.
/// \return byte buffer with packed cell PCCH- message.
/// \remark Only CN Paging is supported currently.
byte_buffer make_asn1_rrc_cell_pcch_pch_msg(uint64_t five_g_s_tmsi);

} // namespace srs_du

} // namespace srsgnb
