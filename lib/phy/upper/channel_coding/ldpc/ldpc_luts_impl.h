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
/// \brief Lookup tables for srsran::ldpc_impl -- access function declarations.
#pragma once

#include "ldpc_graph_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"

namespace srsran {
namespace ldpc {
/// \brief Get the LDPC Tanner graph corresponding to the given base graph and lifting size.
///
/// \param[in] bg Desired base graph.
/// \param[in] ls Desired lifting size.
/// \returns A compact parity check matrix: the value taken by entry \f$(m, n)\f$ represents the shift
///          applied when lifting the edge between check node \f$m\f$ and variable node \f$n\f$ of the
///          base graph.
BG_matrix_t get_graph(ldpc_base_graph_type bg, lifting_size_t ls);

/// Get the lifting size index for a given lifting size.
uint8_t get_lifting_index(lifting_size_t ls);

/// Get the position of the lifting size in the list of all lifting sizes.
uint8_t get_lifting_size_position(lifting_size_t ls);

/// Get the adjacency matrix of base graph \c bg (sparse representation).
const BG_adjacency_matrix_t* get_adjacency_matrix(ldpc_base_graph_type bg);
} // namespace ldpc
} // namespace srsran
