/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Codeblock metadata and related types and constants.
#pragma once

#include "log_likelihood_ratio.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsgnb/ran/modulation_scheme.h"
// TODO(david,borja): fix dependency.

namespace srsgnb {

/// \brief Describes a codeblock.
///
/// Characterization of the codeblocks obtained from a single transport block with all the parameters needed by the
/// encoder/decoder and by the rate matcher/dematcher blocks.
struct codeblock_metadata {
  /// Common parameters for all codeblocks from the same transport block.
  struct tb_common_metadata {
    // TODO(david,alex): Improve name and possibly merge with segment_config below.

    /// Code base graph.
    ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
    /// Code lifting size.
    ldpc::lifting_size_t lifting_size = ldpc::LS2;
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv = 0;
    /// Modulation scheme.
    modulation_scheme mod = modulation_scheme::BPSK;
    /// \brief Limited buffer rate matching length, as per TS38.212 Section 5.4.2.
    /// \note Set to zero for unlimited buffer length.
    unsigned Nref = 0;
    /// Codeword length (after codeblock concatenation).
    unsigned cw_length = 0;
  };

  /// Parameters that are specific to a single codeblock.
  struct cb_specific_metadata {
    /// Codeblock length before rate matching.
    unsigned full_length = 0;
    /// Codeblock length after rate matching.
    unsigned rm_length = 0;
    /// Number of filler bits in the full codeblock.
    unsigned nof_filler_bits = 0;
    /// Codeblock starting index within the codeword.
    unsigned cw_offset = 0;
    /// CRC bits
    unsigned nof_crc_bits = 16;
  };

  /// Contains common transport block parameters.
  tb_common_metadata tb_common;
  /// Contains specific code block parameters.
  cb_specific_metadata cb_specific;
};

/// \brief Maximum segment length.
///
/// This is given by the maximum lifting size (i.e., 384) times the maximum number of information bits in base graph
/// BG1 (i.e., 22), as per TS38.212 Section 5.2.2.
static constexpr unsigned MAX_SEG_LENGTH = 22 * 384;

/// Maximum number of segments per transport block.
static constexpr unsigned MAX_NOF_SEGMENTS = 52;

/// Alias for the segment data container.
using segment_data = static_vector<uint8_t, MAX_SEG_LENGTH>;

/// \brief Alias for the full segment characterization.
///
///   - \c described_segment.first()   Contains the segment data, including CRC, in unpacked format (each bit is
///                                      represented by a \c uint8_t entry).
///   - \c described_segment.second()  Contains the segment metadata, useful for processing the corresponding
///                                      segment (e.g., encoding, rate-matching).
class described_segment
{
public:
  /// Default constructor - Sets the codeblock size to zero to indicate it is invalid.
  described_segment() = default;

  /// Create segment.
  described_segment(codeblock_metadata metadata_, unsigned cb_size_) : metadata(metadata_), cb_size(cb_size_)
  {
    srsgnb_assert((cb_size > 0) && (cb_size <= MAX_SEG_LENGTH),
                  "The codeblock size (i.e., {}) is invalid (max. {}).",
                  cb_size,
                  MAX_SEG_LENGTH);
  }

  /// \brief Gets a read-write view of the segment data.
  /// \remark An assertion is triggered if the segment has not been created with parameter.
  span<uint8_t> get_data()
  {
    srsgnb_assert(cb_size != 0, "The segment has not been constructed.");
    return span<uint8_t>(data).first(cb_size);
  }

  /// \brief Gets a read-only view of the segment data.
  /// \remark An assertion is triggered if the segment has not been created with parameter.
  span<const uint8_t> get_data() const
  {
    srsgnb_assert(cb_size != 0, "The segment has not been constructed.");
    return span<const uint8_t>(data).first(cb_size);
  }

  /// Gets the segment metadata.
  const codeblock_metadata& get_metadata() const { return metadata; }

private:
  codeblock_metadata                  metadata;
  unsigned                            cb_size = 0;
  std::array<uint8_t, MAX_SEG_LENGTH> data;
};

/// \brief Alias for the full codeblock characterization at the receiver.
///
///   - \c described_rx_codeblock.first()   Contains a view to the LLRs corresponding to one codeblock.
///   - \c described_rx_codeblock.second()  Contains the codeblock metadata, useful for processing the corresponding
///                                      codeblock (e.g., decoding, rate-dematching).
using described_rx_codeblock = std::pair<span<const log_likelihood_ratio>, codeblock_metadata>;

/// Gathers all segmentation configuration parameters.
struct segmenter_config {
  /// Code base graph.
  ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
  /// Redundancy version, values in {0, 1, 2, 3}.
  unsigned rv = 0;
  /// Modulation scheme.
  modulation_scheme mod = modulation_scheme::BPSK;
  /// \brief Limited buffer rate matching length in bits, as per TS38.212 Section 5.4.2.
  /// \note Set to zero for unlimited buffer length.
  unsigned Nref = 0;
  /// Number of transmission layers the transport block is mapped onto.
  unsigned nof_layers = 0;
  /// Number of channel symbols (i.e., REs) the transport block is mapped to.
  unsigned nof_ch_symbols = 0;
};

} // namespace srsgnb
