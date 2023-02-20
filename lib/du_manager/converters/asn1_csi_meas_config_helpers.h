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

#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/ran/csi_rs/csi_meas_config.h"

namespace srsgnb {
namespace srs_du {

/// \brief Fills ASN.1 CSI-MeasConfig struct.
/// \param[out] out The ASN.1 CSI-MeasConfig struct to fill.
/// \param[in] src Previous CSI Measurement configuration of UE.
/// \param[in] dest Next CSI Measurement configuration of UE.
void calculate_csi_meas_config_diff(asn1::rrc_nr::csi_meas_cfg_s& out,
                                    const csi_meas_config&        src,
                                    const csi_meas_config&        dest);

} // namespace srs_du
} // namespace srsgnb
