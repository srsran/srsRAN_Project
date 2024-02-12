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
/// \brief Codeblock metadata and related types and constants.
#pragma once

#include "log_likelihood_ratio.h"
#include "srsran/adt/bit_buffer.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
// TODO(david,borja): fix dependency.
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/ran/sch/modulation_scheme.h"
#include "srsran/ran/sch/sch_constants.h"

namespace srsran {

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

/// \brief Describes an LDPC segment or codeblock.
///
/// The segment description consists of a set of parameters contained in metadata attribute and a data storage for the
/// segment.
///
/// If the object is created with the default constructor, the \c get_data() methods trigger an assertion.
class described_segment
{
public:
  /// Default constructor - Creates an invalid empty segment.
  described_segment() : data(0) {}

  /// \brief Creates a description for a segment of length \c cb_size with the given metadata.
  ///
  /// This constructor does not initialize the codeblock data. It is up to the class client to fill the codeblock data
  /// after its creation.
  ///
  /// \param[in] metadata_ Codeword parameters.
  /// \param[in] cb_size_  Codeword size in bits.
  /// \remark Throws an assertion if \c cb_size is higher than the maximum supported length.
  described_segment(const codeblock_metadata& metadata_, units::bits cb_size_) : metadata(metadata_)
  {
    using namespace units::literals;
    srsran_assert((cb_size_ > 0_bits) && (cb_size_ <= MAX_SEG_LENGTH),
                  "The codeblock size (i.e., {}) is invalid (max. {}).",
                  cb_size_,
                  MAX_SEG_LENGTH);
    data.resize(cb_size_.value());
  }

  /// \brief Gets a read&ndash;write view of the segment data.
  /// \remark An assertion is triggered if the segment has not been created with parameter.
  bit_buffer& get_data()
  {
    srsran_assert(!data.is_empty(), "The segment has not been constructed.");
    return data;
  }

  /// \brief Gets a read-only view of the segment data.
  /// \remark An assertion is triggered if the segment has not been created with parameter.
  const bit_buffer& get_data() const
  {
    srsran_assert(!data.is_empty(), "The segment has not been constructed.");
    return data;
  }

  /// Gets the segment metadata.
  const codeblock_metadata& get_metadata() const { return metadata; }

private:
  /// Codeblock configuration.
  codeblock_metadata metadata;
  /// Codeblock data storage.
  static_bit_buffer<MAX_SEG_LENGTH.value()> data;
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

} // namespace srsran
