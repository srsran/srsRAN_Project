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

/// \file
/// \brief Declaration of the validator of PRACH configurations at the PHY layer.

#pragma once

#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"

namespace srsran {

/// \brief Ensures the PRACH configuration is supported by the detector.
///
/// \param[in] format                   PRACH format.
/// \param[in] scs                      PRACH subcarrier spacing.
/// \param[in] zero_correlation_zone    PRACH zero correlation zone.
/// \param[in] nof_rx_ports             Number of receive antenna ports.
/// \return True if the PRACH configuration is support by the detector, false otherwise.
bool validate_prach_detector_phy(prach_format_type        format,
                                 prach_subcarrier_spacing scs,
                                 unsigned                 zero_correlation_zone,
                                 unsigned                 nof_rx_ports);

} // namespace srsran