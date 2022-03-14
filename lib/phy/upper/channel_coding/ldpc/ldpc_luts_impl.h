/// \file
/// \brief Lookup tables for srsgnb::ldpc_impl -- access function declarations.
#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LUTS_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LUTS_IMPL_H

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
BG_matrix_t get_graph(base_graph_t bg, lifting_size_t ls);

/// Get the lifting size index for a given lifting size.
uint8_t get_lifting_index(lifting_size_t ls);

/// Get the position of the lifting size in the list of all lifting sizes.
uint8_t get_lifting_size_position(lifting_size_t ls);

/// Get the adjacency matrix of base graph \c bg (sparse representation).
const BG_adjacency_matrix_t* get_adjacency_matrix(base_graph_t bg);
} // namespace ldpc
} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LUTS_IMPL_H