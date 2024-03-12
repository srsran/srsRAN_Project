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
/// \brief Generic hardware-accelerated PUSCH decoder functions declaration.

#pragma once

#include "srsran/hal/hw_accelerator.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

namespace srsran {

// Maximum accepted transport block size.
static constexpr unsigned MAX_TBS = 1277992;

// Number of bits in the long CRC. A CRC of this length is used either for TB CRCs, when the TB is longer than
// MAX_BITS_CRC16, or as a codeblock CRC, when the TB consists of multiple codeblocks.
static constexpr unsigned LONG_CRC_LENGTH = 24;

/// Generic hardware-accelerated implementation of the PUSCH decoder.
class pusch_decoder_hw_impl : public pusch_decoder, private pusch_decoder_buffer
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

  /// \brief PUSCH decoder constructor.
  ///
  /// Sets up the internal components, namely LDPC segmenter, all the CRC calculators and the hardware accelerator.
  ///
  /// \param[in] seg   Pointer to an LDPC segmenter object.
  /// \param[in] crcs  Structure with pointers to three CRC calculator objects, with generator polynomials of type \c
  ///                  CRC16, \c CRC24A and \c CRC24B.
  /// \param[in] hw      Unique pointer to a hardware-accelerator.
  pusch_decoder_hw_impl(std::unique_ptr<ldpc_segmenter_rx>             seg,
                        sch_crc&                                       c,
                        std::unique_ptr<hal::hw_accelerator_pusch_dec> hw) :
    segmenter(std::move(seg)),
    crc_set({std::move(c.crc16), std::move(c.crc24A), std::move(c.crc24B)}),
    decoder(std::move(hw)),
    softbits_buffer(pdsch_constants::CODEWORD_MAX_SIZE.value())
  {
    srsran_assert(segmenter, "Invalid LDPC segmenter factory.");
    srsran_assert(crc_set.crc16, "Invalid CRC16 calculator.");
    srsran_assert(crc_set.crc24A, "Invalid CRC24A calculator.");
    srsran_assert(crc_set.crc24B, "Invalid CRC24B calculator.");
    srsran_assert(crc_set.crc16->get_generator_poly() == crc_generator_poly::CRC16,
                  "Not a CRC generator of type CRC16.");
    srsran_assert(crc_set.crc24A->get_generator_poly() == crc_generator_poly::CRC24A,
                  "Not a CRC generator of type CRC24A.");
    srsran_assert(crc_set.crc24B->get_generator_poly() == crc_generator_poly::CRC24B,
                  "Not a CRC generator of type CRC24B.");
    srsran_assert(decoder, "Invalid hardware-accelerated PUSCH decoder.");
    absolute_cb_ids.resize(MAX_NOF_SEGMENTS);
  }

  // See interface for the documentation.
  pusch_decoder_buffer& new_data(span<uint8_t>           transport_block,
                                 unique_rx_buffer        softbuffer,
                                 pusch_decoder_notifier& notifier,
                                 const configuration&    cfg) override;

  // See interface for the documentation.
  void set_nof_softbits(units::bits nof_softbits) override
  {
    // Not implemented.
  }

private:
  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_rx> segmenter;
  /// \brief Pointer to a CRC calculator for TB-wise checksum.
  ///
  /// Only the CRC calculator with generator polynomial crc_generator_poly::CRC24A, used for long transport blocks, is
  /// needed. Indeed, if a transport block is short enough not to be segmented, the CRC is verified by the decoder.
  sch_crc crc_set;
  /// Pointer to a hardware-accelerated PUSCH decoder.
  std::unique_ptr<hal::hw_accelerator_pusch_dec> decoder;
  /// Soft bit buffer.
  std::vector<log_likelihood_ratio> softbits_buffer;
  /// Counts the number of soft bits in the buffer.
  unsigned softbits_count;
  /// Current transport block.
  span<uint8_t> transport_block;
  /// Current soft bits buffer.
  unique_rx_buffer softbuffer;
  /// Current notifier.
  pusch_decoder_notifier* result_notifier = nullptr;
  /// Current PUSCH decoder configuration.
  pusch_decoder::configuration current_config;
  /// Set of unique IDs associated to the CBs in the soft-buffer and HARQ ACK context entries.
  static_vector<unsigned, MAX_NOF_SEGMENTS> absolute_cb_ids;
  /// Temporary buffer to store the rate-matched codeblocks (represented by LLRs) and their metadata.
  static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS> codeblock_llrs = {};
  /// Temporary buffer to store the unpacked transport block (and, if applicable, its CRC).
  static_bit_buffer<MAX_TBS + LONG_CRC_LENGTH> tmp_tb_bits;

  // See interface for the documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override;

  // See interface for the documentation.
  void on_new_softbits(span<const log_likelihood_ratio> softbits) override;

  // See interface for the documentation.
  void on_end_softbits() override;

  /// \brief Checks the results of the decoding operation.
  /// \param[in] stats    Structure to store the PUSCH decoder results.
  /// \param[in] cb_crcs  Set of CRC flags.
  /// \param[in] cb_index Index of the CB (within the TB).
  /// \param[in] crc_type Type of the CB CRC.
  /// \param[in] data     Decoded data.
  /// \return Results of the decoding operation (CRC and number of LDPC decoder iterations).
  void check_hw_results(pusch_decoder_result&   stats,
                        span<bool>              cb_crcs,
                        unsigned                cb_id,
                        hal::hw_dec_cb_crc_type crc_type,
                        bit_buffer              data);

  /// \brief Copies the decoded bits and notifies the end of the operation.
  /// \param[in] stats    Structure to store the PUSCH decoder results.
  /// \param[in] cb_crcs  Set of CRC flags.
  void copy_tb_and_notify(pusch_decoder_result& stats, unsigned nof_cbs, span<bool> cb_crcs);

  /// \brief Sets the segmentation and decoding parameters required by the hardware-accelerated PUSCH decoder function.
  /// \param[in]  cfg                PUSCH configuration parameters.
  /// \param[in]  nof_segments       Number of segments in the transport block.
  /// \param[in]  rm_length          Length of the rate-matched codeblock in bits.
  /// \param[in]  lifting_size       Length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  /// \param[in]  nof_segment_bits   Number of information bits that is assigned to a segment.
  /// \param[in]  nof_filler_bits    Number of filler bits in a segment.
  /// \param[in]  crc_len            Length of the CB CRC in bits,
  /// \param[in]  crc_type           Type of the CB CRC.
  /// \param[in]  cb_index           Index of the CB (within the TB).
  void set_hw_dec_configuration(unsigned                nof_segments,
                                unsigned                rm_length,
                                unsigned                lifting_size,
                                unsigned                nof_segment_bits,
                                unsigned                nof_filler_bits,
                                unsigned                crc_len,
                                hal::hw_dec_cb_crc_type crc_type,
                                unsigned                cb_index);
};

} // namespace srsran
