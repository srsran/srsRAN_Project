/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_DU_MANAGER_CONVERTERS_F1C_ASN1_HELPERS_H
#define SRSGNB_LIB_DU_MANAGER_CONVERTERS_F1C_ASN1_HELPERS_H

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"

namespace srsgnb {

namespace srs_du {

/// \brief Derive packed cell MIB from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell MIB.
byte_buffer make_asn1_rrc_cell_mib_buffer(const du_cell_config& du_cfg);

/// \brief Derive packed cell SIB1 from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell SIB1.
byte_buffer make_asn1_rrc_cell_sib1_buffer(const du_cell_config& du_cfg);

/// \brief Fills ASN.1 F1SetupRequest struct.
/// \param[out] request The F1 setup request message struct to fill.
/// \param[in] setup_params DU setup parameters to add to the F1SetupRequest.
/// \param[in] cells_to_add Configurations of cells to add to F1SetupRequest.
void fill_asn1_f1_setup_request(asn1::f1ap::f1_setup_request_s& request,
                                const du_setup_params&          setup_params,
                                span<const du_cell_config*>     cells_to_add);

} // namespace srs_du

} // namespace srsgnb

#endif // SRSGNB_LIB_DU_MANAGER_CONVERTERS_F1C_ASN1_HELPERS_H
