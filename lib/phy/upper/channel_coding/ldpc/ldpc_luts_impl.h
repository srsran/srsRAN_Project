/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Lookup tables for srsgnb::ldpc_impl -- access function declarations.
#pragma once

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"

namespace srsgnb {
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
} // namespace srsgnb
