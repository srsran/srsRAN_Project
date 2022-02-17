/// \file
/// \brief Lookup tables for srsgnb::ldpc_impl -- access function declarations.
#ifndef SRSGNB_CHANNEL_CODING_LDPC_IMPL_LUTS
#define SRSGNB_CHANNEL_CODING_LDPC_IMPL_LUTS

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc.h"

srsgnb::ldpc::BG_matrix_t get_graph(srsgnb::ldpc::base_graph_t, srsgnb::ldpc::lifting_size_t);

uint8_t get_lifting_index(srsgnb::ldpc::lifting_size_t);

#endif // SRSGNB_CHANNEL_CODING_LDPC_IMPL_LUTS