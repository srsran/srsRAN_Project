/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Declaration of the validator of PRACH configurations at the PHY layer.

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"

namespace srsran {

/// \brief Ensures the PRACH configuration is supported by the detector.
///
/// \param[in] format                   PRACH format.
/// \param[in] scs                      PRACH subcarrier spacing.
/// \param[in] zero_correlation_zone    PRACH zero correlation zone.
/// \param[in] nof_rx_ports             Number of receive antenna ports.
/// \return A success if the PRACH configuration is support by the detector, an error message otherwise.
error_type<std::string> validate_prach_detector_phy(prach_format_type        format,
                                                    prach_subcarrier_spacing scs,
                                                    unsigned                 zero_correlation_zone,
                                                    unsigned                 nof_rx_ports);

} // namespace srsran
