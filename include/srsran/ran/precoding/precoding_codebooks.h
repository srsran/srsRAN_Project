/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ran/precoding/precoding_configuration.h"

namespace srsran {

/// Constructs a precoder configuration for a single transmitter port.
precoding_configuration make_single_port();

/// \brief Constructs a wideband precoder configuration for one layer mapped into one transmit port.
///
/// Creates a precoding configuration for all the channel bandwidth. The coefficients dimensions correspond to one layer
/// and a given number of ports. All weights are zero except for the indicated port.
///
/// \param[in] nof_ports Number of ports available for transmitting.
/// \param[in] i_port    Port identifier {0, ..., \c nof_ports - 1}.
/// \return A wideband precoding configuration for one layer and \c nof_ports.
/// \remark An assertion is triggered if \c i_port is equal to or greater than \c nof_ports.
precoding_configuration make_wideband_one_layer_one_port(unsigned nof_ports, unsigned i_port);

/// \brief Constructs a wideband precoder configuration for the same number of layers and ports.
///
/// Creates a precoding configuration for all the channel bandwidth. The coefficients dimensions correspond to \c
/// nof_streams layers and ports. The weights map each layer to the same port identifier.
///
/// \param[in] nof_streams Number layers and ports.
/// \return A wideband precoding configuration for \c nof_streams layers and ports.
/// \remark An assertion is triggered if \c nof_streams is out of the range {1, ..., \ref
/// precoding_constants::MAX_NOF_LAYERS}.
precoding_configuration make_wideband_identity(unsigned nof_streams);

/// \brief Constructs a wideband precoder configuration for one layer mapped into two transmit ports.
///
/// Creates a precoding configuration for all the channel bandwidth. The coefficients dimensions correspond to one layer
/// and two ports. All weights are derived from TS38.214 Table 5.2.2.2.1-1 for 1-layer CSI reporting.
///
/// \param[in] i_codebook Codebook identifier.
/// \return A wideband precoding configuration for one layer and two ports.
precoding_configuration make_wideband_one_layer_two_ports(unsigned i_codebook);

/// \brief Constructs a wideband precoder configuration for two layers mapped into two transmit ports.
///
/// Creates a precoding configuration for all the channel bandwidth. The coefficients dimensions correspond to two
/// layers and two ports. All weights are derived from TS38.214 Table 5.2.2.2.1-1 for 2-layer CSI reporting.
///
/// \param[in] i_codebook Codebook identifier.
/// \return A wideband precoding configuration for two layers and two ports.
precoding_configuration make_wideband_two_layer_two_ports(unsigned i_codebook);

} // namespace srsran
