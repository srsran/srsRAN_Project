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

/// \brief Constructs a precoding weight matrix for the one layer&ndash;one port mapping.
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

/// \brief Constructs a precoding weight matrix for one layer mapped into four transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-5, which describes one layer CSI reporting using a Type 1,
/// single panel codebook. For one layer mapped to four antenna ports, the precoding uses the same beam for both antenna
/// polarizations. The beam can be selected from eight possible beams, and the phase shift between cross-polarized
/// antenna elements can be selected in \f$\pi/2\f$-rad increments, starting from 0 rad. The generated precoding weights
/// for ports 0 and 1 correspond to the antenna elements of the first polarization, while ports 2 and 3 correspond to
/// the antenna elements of the second polarization.
///
/// \param[in] beam_azimuth_id Beam selector in the horizontal plane. Corresponds to parameter \f$i_{1,1}\f$ in TS38.214
///                            Section 5.2.2.2.1. Values: {0, 1, ..., 7}
/// \param[in] pol_shift_id    Polarization phase shift selector. Corresponds to parameter \f$i_2\f$ in TS38.214
///                            Section 5.2.2.2.1. Values: {0, 1, ..., 3}
/// \return A precoding weight matrix for one layer and four ports.
precoding_weight_matrix make_one_layer_four_ports_type1_sp_mode1(unsigned beam_azimuth_id, unsigned pol_shift_id);

/// \brief Constructs a precoding weight matrix for two layers mapped into four transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-6, which describes two layer CSI reporting using a Type 1,
/// single panel codebook. For two layers mapped to four antenna ports, the precoding uses one beam for each layer.
/// The first beam can be selected from eight possible beams, and the second beam is specified by applying an offset to
/// the direction of the first one, in \f$\pi\f$-rad increments, starting from 0 rad. The phase shift between
/// cross-polarized antenna elements can be selected in \f$\pi/2\f$-rad increments, starting from 0 rad. The generated
/// precoding weights for ports 0 and 1 correspond to the antenna elements of the first polarization, while ports 2 and
/// 3 correspond to the antenna elements of the second polarization.
///
/// \param[in] beam_azimuth_id First beam selector in the horizontal plane. Corresponds to parameter \f$i_{1,1}\f$ in
///                            TS38.214 Section 5.2.2.2.1. Values: {0, 1, ..., 7}
/// \param[in] beam_offset_id  Second beam offset selector. Corresponds to parameter \f$i_{1,3}\f$ in TS38.214 Section
///                            5.2.2.2.1. Values: {0, 1}
/// \param[in] pol_shift_id    Polarization phase shift selector. Corresponds to parameter \f$i_2\f$ in TS38.214 Section
///                            Section 5.2.2.2.1. Values: {0, 1}
/// \return A precoding weight matrix for two layers and four ports.
precoding_weight_matrix
make_two_layer_four_ports_type1_sp_mode1(unsigned beam_azimuth_id, unsigned beam_offset_id, unsigned pol_shift_id);

/// \brief Constructs a precoding weight matrix for three layers mapped into four transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-7, which describes three layer CSI reporting using a Type 1,
/// single panel codebook. For three layers mapped to four antenna ports, the precoding uses a different combination of
/// beam offsets and polarization phase shifts for each layer. The first beam can be selected from eight possible beams,
/// and the phase shift between cross-polarized antenna elements can be selected in \f$\pi/2\f$-rad increments, starting
/// from 0 rad. The generated precoding weights for ports 0 and 1 correspond to the antenna elements of the first
/// polarization, while ports 2 and 3 correspond to the antenna elements of the second polarization.
///
/// \param[in] beam_azimuth_id First beam selector in the horizontal plane. Corresponds to parameter \f$i_{1,1}\f$ in
///                            TS38.214 Section 5.2.2.2.1. Values: {0, 1, ..., 7}
/// \param[in] pol_shift_id    Polarization phase shift selector. Corresponds to parameter \f$i_2\f$ in TS38.214 Section
///                            Section 5.2.2.2.1. Values: {0, 1}
/// \return A precoding weight matrix for three layers and four ports.
precoding_weight_matrix make_three_layer_four_ports_type1_sp(unsigned beam_azimuth_id, unsigned pol_shift_id);

/// \brief Constructs a precoding weight matrix for three layers mapped into four transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-8, which describes four layer CSI reporting using a Type 1,
/// single panel codebook. For four layers mapped to four antenna ports, the precoding uses a different combination of
/// beam offsets and polarization phase shifts for each layer. The first beam can be selected from eight possible beams,
/// and the polarization phase shift can be selected in \f$\pi/2\f$-rad increments, starting from 0 rad. The generated
/// precoding weights for ports 0 and 1 correspond to the antenna elements of the first polarization, while ports 2 and
/// 3 correspond to the antenna elements of the second polarization.
///
/// \param[in] beam_azimuth_id First beam selector in the horizontal plane. Corresponds to parameter \f$i_{1,1}\f$ in
///                            TS38.214 Section 5.2.2.2.1. Values: {0, 1, ..., 7}
/// \param[in] pol_shift_id    Polarization phase shift selector. Corresponds to parameter \f$i_2\f$ in TS38.214 Section
///                            Section 5.2.2.2.1. Values: {0, 1}
/// \return A precoding weight matrix for four layers and four ports.
precoding_weight_matrix make_four_layer_four_ports_type1_sp(unsigned beam_azimuth_id, unsigned pol_shift_id);

} // namespace srsran
