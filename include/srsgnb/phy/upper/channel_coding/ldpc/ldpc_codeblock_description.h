/// \file
/// \brief Codeblock metadata.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_CODEBLOCK_DESCRIPTION_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_CODEBLOCK_DESCRIPTION_H

#include "srsgnb/phy/modulation_scheme.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"

namespace srsgnb {

/// \brief Describes a codeblock.
///
/// Characterization of the codeblocks obtained from a single transport block with all the parameters needed by the
/// encoder/decoder and by the rate matcher/dematcher blocks.
struct codeblock_description_t {
  /// Common parameters for all codeblocks from the same transport block.
  struct tb_common {
    /// Code base graph.
    ldpc::base_graph_t base_graph = ldpc::base_graph_t::BG1;
    /// Code lifting size.
    ldpc::lifting_size_t lifting_size = ldpc::LS2;
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv = 0;
    /// Modulation scheme.
    modulation_scheme mod = modulation_scheme::BPSK;
    /// \brief Limited buffer rate matching length, as per TS38.212 Section 5.4.2.
    /// \note Set to zero for unlimited buffer length.
    unsigned Nref = 0;
  } tb_common;
  /// Parameters that are specific to a single codeblock.
  struct cb_specific {
    /// Codeblock length before rate matching.
    unsigned full_length = 0;
    /// Codeblock length after rate matching.
    unsigned rm_length = 0;
    /// Number of filler bits in the full codeblock.
    unsigned nof_filler_bits = 0;
  } cb_specific;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_CODEBLOCK_DESCRIPTION_H
