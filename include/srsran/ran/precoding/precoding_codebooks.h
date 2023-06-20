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

#include "srsran/ran/precoding/precoding_weight_matrix.h"

namespace srsran {

/// Constructs a precoder configuration for a single transmitter port.
precoding_weight_matrix make_single_port();

/// \brief Constructs a precoding weight matrix for one layer mapped into one transmit port.
///
/// The matrix dimensions correspond to one layer and a given number of ports. All weights are zero except for the
/// indicated port.
///
/// \param[in] nof_ports Number of ports available for transmitting.
/// \param[in] i_port    Port identifier {0, ..., \c nof_ports - 1}.
/// \return A precoding weight matrix for one layer and \c nof_ports ports.
/// \remark An assertion is triggered if \c i_port is equal to or greater than \c nof_ports.
precoding_weight_matrix make_one_layer_one_port(unsigned nof_ports, unsigned i_port);

// \brief Constructs a precoding weight matrix for the one layer&ndash;one port mapping.
///
/// The precoding maps each layer to the port with the same identifier.
///
/// \param[in] nof_streams Number of layers and ports.
/// \return A scaled identity precoding weight matrix for \c nof_streams layers and ports. The constant value of the
/// entries in the main diagonal is computed to satisfy power-allocation requirements. All other entries are equal to
/// zero.
/// \remark An assertion is triggered if \c nof_streams is out of the range {1, ..., \ref
/// precoding_constants::MAX_NOF_LAYERS}.
precoding_weight_matrix make_identity(unsigned nof_streams);

/// \brief Constructs a precoding weight matrix for one layer mapped into all transmit ports.
///
/// All weights are set so that an identical signal is generated for each antenna port.
///
/// \param[in] nof_ports Number of ports available for transmitting.
/// \return A precoding weight matrix (actually, a column vector) for one layer and \c nof_ports.
/// \remark This precoding matrix is intended for testing purposes only.
precoding_weight_matrix make_one_layer_all_ports(unsigned nof_ports);

/// \brief Constructs a precoding weight matrix for one layer mapped into two transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-1 for 1-layer CSI reporting.
///
/// \param[in] i_codebook Codebook identifier.
/// \return A precoding weight matrix for one layer and two ports.
precoding_weight_matrix make_one_layer_two_ports(unsigned i_codebook);

/// \brief Constructs a precoding weight matrix for two layers mapped into two transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-1 for 2-layer CSI reporting.
///
/// \param[in] i_codebook Codebook identifier.
/// \return A precoding weight matrix for two layers and two ports.
precoding_weight_matrix make_two_layer_two_ports(unsigned i_codebook);

} // namespace srsran
