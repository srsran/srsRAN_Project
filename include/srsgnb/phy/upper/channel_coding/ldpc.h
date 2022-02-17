/// \file
/// \brief LDPC constants, utilities and graphs.
///
/// This file defines types, constants and classes to represent an LDPC Tanner graph
/// according to TS38.212 Section 5.3.2.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC

#include <array>
#include <memory>

namespace srsgnb {

namespace ldpc {
/// Base graph types.
enum class base_graph_t { BG1, BG2 };

/// Available lifting sizes.
enum lifting_size_t { LS2 = 2, LS3 = 3, LS4 = 4 };
/// Total number of lifting sizes.
constexpr unsigned nof_lifting_sizes = 3;
/// All lifting sizes are assigned an index from 0 to 7 (see TS38.212 Table 5.3.2-1).
constexpr unsigned nof_lifting_indices = 8;

/// Array of lifting sizes, for iterations.
static constexpr std::array<lifting_size_t, nof_lifting_sizes> all_lifting_sizes = {LS2, LS3, LS4};
} // namespace ldpc

} // namespace srsgnb
#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC