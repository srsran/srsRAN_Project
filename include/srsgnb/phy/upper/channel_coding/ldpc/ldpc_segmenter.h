/// \file
/// \brief LDPC codeblock segmentation.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_codeblock_description.h"

namespace srsgnb {

/// Carries out the segmentation of a transport block into a number of codeblocks.
class ldpc_segmenter
{
public:
  virtual ~ldpc_segmenter() = default;

  /// Alias for vector of segmented codeblocks.
  using segmented_codeblocks = std::vector<std::vector<uint8_t> >;

  /// Gathers all segmentation configuration parameters.
  struct config_t {
    /// Code base graph.
    ldpc::base_graph_t base_graph{ldpc::base_graph_t::BG1};
    /// Code lifting size.
    ldpc::lifting_size_t lifting_size{ldpc::LS2};
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv{0};
    /// Modulation scheme.
    modulation_scheme mod{modulation_scheme::BPSK};
    /// \brief Limited buffer rate matching length, as per TS38.212 Section 5.4.2.
    ///
    /// Set to zero for unlimited buffer length.
    unsigned Nref{0};
    /// Number of transmission layers the transport block is mapped onto.
    unsigned nof_layers{0};
    /// Number of channel symbols (i.e., REs) the transport block is mapped to.
    unsigned nof_ch_symbols{0};
  };

  /// \brief Carries out transport block segmentation and computes all codeblock metadata for later use (encoder and
  /// rate matching).
  ///
  /// The transport block is split into a number of segments and, if needed, a CRC is added to each segment. This is
  /// done according to TS38.212 Section 5.2.2. The function also computes other segment metadata (e.g., coded and
  /// rate-matched length) according to TS38.212 Section 5.4.2.1.
  /// \param[out] segments              The segments the transport block is split into.
  /// \param[out] segment_descriptions  Metadata relative to each segment.
  /// \param[in]  transport_block       The transport block to split.
  /// \param[in]  cfg                   Parameters affecting splitting and codeblock metadata.
  /// \remark The transport block and the resulting segments are represented by unpacked bits (each entry is a single
  ///         bit).
  virtual void segment(segmented_codeblocks&   segments,
                       tb_segment_description& segment_descriptions,
                       span<const uint8_t>     transport_block,
                       const config_t&         cfg) = 0;
};

std::unique_ptr<ldpc_segmenter> create_ldpc_segmenter();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H
