/// \file
/// \brief LDPC codeblock segmentation.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_codeblock_description.h"

namespace srsgnb {

/// Carries out the segmentation of a transport block into a number of codeblocks.
class ldpc_segmenter
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter() = default;

  /// \brief Maximum segment length.
  ///
  /// This is given by the maximum lifting size (i.e., 384) times the maximum number of information bits in base graph
  /// BG1 (i.e., 22), as per TS38.212 Section 5.2.2.
  static constexpr unsigned MAX_SEG_LENGTH = 22 * 384;

  /// Maximum number of segments per transport block.
  static constexpr unsigned MAX_NOF_SEGMENTS = 52;

  /// Alias for the segment data container.
  using segment_data_t = static_vector<uint8_t, MAX_SEG_LENGTH>;

  /// \brief Alias for the full segment characterization.
  ///
  /// This is a self-contained type that associates a segment of the transport block with all the metadata required by
  /// subsequent processing units (e.g., encoder and rate-matcher).
  ///   - \c described_segment_t.first()   Contains the segment data, including CRC, in unpacked format (each bit is
  ///                                      represented by a \c uint8_t entry).
  ///   - \c described_segment_t.second()  Contains the segment metadata, useful for processing the corresponding
  ///                                      codeblock.
  using described_segment_t = std::pair<segment_data_t, codeblock_description_t>;

  /// Gathers all segmentation configuration parameters.
  struct config_t {
    /// Code base graph.
    ldpc::base_graph_t base_graph = ldpc::base_graph_t::BG1;
    /// Code lifting size.
    ldpc::lifting_size_t lifting_size = ldpc::LS2;
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv{0};
    /// Modulation scheme.
    modulation_scheme mod = modulation_scheme::BPSK;
    /// \brief Limited buffer rate matching length, as per TS38.212 Section 5.4.2.
    /// \note Set to zero for unlimited buffer length.
    unsigned Nref = 0;
    /// Number of transmission layers the transport block is mapped onto.
    unsigned nof_layers = 0;
    /// Number of channel symbols (i.e., REs) the transport block is mapped to.
    unsigned nof_ch_symbols = 0;
  };

  /// \brief Adds the CRC to the a transport block, carries out segmentation and computes all codeblock metadata for
  /// later use (encoder and rate matching).
  ///
  /// First, the transport block CRC is attached, as per TS38.212 Section 7.2.1. Then, the transport block is split into
  /// a number of segments and, if needed, a CRC is attached to each segment. This is done according to TS38.212
  /// Section 5.2.2. The function also computes other segment metadata (e.g., coded and rate-matched length) according
  /// to TS38.212 Section 5.4.2.1.
  /// \param[out] described_segments    Segments (unpacked, one bit per entry) and corresponding metadata.
  /// \param[in]  transport_block       The transport block to segment (packed, one byte per entry).
  /// \param[in]  cfg                   Parameters affecting splitting and codeblock metadata.
  virtual void segment(static_vector<described_segment_t, MAX_NOF_SEGMENTS>& described_segments,
                       span<const uint8_t>                                   transport_block,
                       const config_t&                                       cfg) = 0;
};

std::unique_ptr<ldpc_segmenter> create_ldpc_segmenter();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H
