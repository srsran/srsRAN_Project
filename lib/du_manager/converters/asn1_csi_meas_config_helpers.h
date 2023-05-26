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

#include "srsran/ran/csi_rs/csi_meas_config.h"

namespace asn1 {
namespace rrc_nr {

// forward declarations.
struct csi_meas_cfg_s;

} // namespace rrc_nr
} // namespace asn1

namespace srsran {
namespace srs_du {

/// \brief Fills ASN.1 CSI-MeasConfig struct.
/// \param[out] out The ASN.1 CSI-MeasConfig struct to fill.
/// \param[in] src Previous CSI Measurement configuration of UE.
/// \param[in] dest Next CSI Measurement configuration of UE.
void calculate_csi_meas_config_diff(asn1::rrc_nr::csi_meas_cfg_s& out,
                                    const csi_meas_config&        src,
                                    const csi_meas_config&        dest);

} // namespace srs_du
} // namespace srsran
