/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC codeblock segmentation declaration.

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_IMPL_H

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_segmenter.h"

namespace srsgnb {
namespace ldpc {
/// Maximum length of a block (number of information bits) when using base graph BG1.
constexpr unsigned MAX_BG1_BLOCK_LENGTH = (BG1_N_FULL - BG1_M) * MAX_LIFTING_SIZE;
/// Maximum length of a block (number of information bits) when using base graph BG2.
constexpr unsigned MAX_BG2_BLOCK_LENGTH = (BG2_N_FULL - BG2_M) * MAX_LIFTING_SIZE;
} // namespace ldpc

/// Maximum accepted transport block size.
static constexpr unsigned MAX_TBS = 1277992;

/// Generic implementation of LDPC segmentation.
class ldpc_segmenter_impl : public ldpc_segmenter
{
public:
  /// CRC calculators used in shared channels.
  struct sch_crc {
    /// For short TB checksums.
    std::unique_ptr<crc_calculator> crc16;
    /// For long TB checksums.
    std::unique_ptr<crc_calculator> crc24A;
    /// For segment-specific checksums.
    std::unique_ptr<crc_calculator> crc24B;
  };

  /// \brief Creates an LDPC segmentation object that aggregates a crc_calculator.
  ///
  /// \param[in] c The CRC calculator to aggregate. The generation polynomial must be CRC24B.
  explicit ldpc_segmenter_impl(sch_crc& c);

  // See interface for the documentation.
  void segment(static_vector<described_segment, MAX_NOF_SEGMENTS>& described_segments,
               span<const uint8_t>                                 transport_block,
               const segment_config&                               cfg) override;

private:
  /// Computes the number of segments the transport block is split into, as per TS38.212 Section 5.2.2.
  void compute_nof_segments();

  /// Computes the lifting size used to encode/decode the current transport block, as per TS38.212 Section 5.2.2.
  void compute_lifting_size();

  /// Computes the length of each segment, as per TS38.212 Section 5.2.2.
  void compute_segment_length();

  /// Computes the length of the rate-matched codeblock corresponding to each segment, as per TS38.212 Section 5.4.2.1.
  unsigned compute_rm_length(unsigned i_seg, modulation_scheme mod, unsigned nof_layers) const;

  // Data members.
  /// Internal temporary buffer.
  static_vector<uint8_t, MAX_TBS> buffer;

  /// Base graph used for encoding/decoding the current transport block.
  ldpc::base_graph_t base_graph = ldpc::base_graph_t::BG1;
  /// Lifting size used for encoding/decoding the current transport block.
  unsigned lifting_size = 0;

  /// \name Attributes relative to TS38.212 Section 5.2.2.
  ///@{

  /// Maximum length of a segment (corresponds to \f$K_{cb}\f$ in ).
  unsigned max_segment_length = 0;
  /// Final length of a segment (corresponds to \f$K\f$).
  unsigned segment_length = 0;
  /// Number of bits in the transport block (corresponds to \f$B\f$).
  unsigned nof_tb_bits_in = 0;
  /// Augmented number of bits in the transport block, including new CRCs (corresponds to \f$B'\f$).
  unsigned nof_tb_bits_out = 0;
  /// Number of segments resulting from the transport block (corresponds to \f$C\f$).
  unsigned nof_segments = 0;
  ///@}

  /// \name Attributes relative to TS38.212 Section 5.4.2.1.
  ///@{

  /// Number of symbols per transmission layer (corresponds to \f$G / (N_L Q_m)\f$).
  unsigned nof_symbols_per_layer = 0;
  /// \brief Number of segments of short rate-matched length (corresponds to \f$C - \bigr(\bigl(G / (N_L Q_m)\bigr)
  /// \bmod C\bigr)\f$).
  unsigned nof_short_segments = 0;
  ///@}

  /// CRC calculators for transport-block and segment-specific checksums.
  sch_crc crc_set;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_IMPL_H
