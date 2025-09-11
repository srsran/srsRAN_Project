/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/mutexed_mpsc_queue.h"
#include "srsran/hal/hw_accelerator.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// Maximum accepted transport block size.
constexpr unsigned MAX_TBS = 1277992;

/// \brief Number of bits in the long CRC.
///
/// A CRC of this length is used either for TB CRCs, when the TB is longer than
/// MAX_BITS_CRC16, or as a codeblock CRC, when the TB consists of multiple codeblocks.
constexpr unsigned LONG_CRC_LENGTH = 24;

/// Generic hardware-accelerated implementation of the PUSCH decoder.
class pusch_decoder_hw_impl : public pusch_decoder, private pusch_decoder_buffer
{
public:
  /// Code block decoder pool type.
  using hw_decoder_pool = bounded_unique_object_pool<hal::hw_accelerator_pusch_dec>;

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
  /// \param[in] seg           Pointer to an LDPC segmenter object.
  /// \param[in] crcs          Structure with pointers to three CRC calculator objects, with generator polynomials of
  ///                          type \c CRC16, \c CRC24A and \c CRC24B.
  /// \param[in] decoder_pool_ Hardware decoder pool.
  /// \param[in] executor_     Optional asynchronous processing task executor. Set to \c nullptr for synchronous
  /// operation.
  pusch_decoder_hw_impl(std::unique_ptr<ldpc_segmenter_rx> seg,
                        sch_crc&                           c,
                        std::shared_ptr<hw_decoder_pool>   decoder_pool_,
                        task_executor*                     executor_) :
    logger(srslog::fetch_basic_logger("PHY")),
    segmenter(std::move(seg)),
    crc_set({std::move(c.crc16), std::move(c.crc24A), std::move(c.crc24B)}),
    decoder_pool(std::move(decoder_pool_)),
    executor(executor_),
    softbits_buffer(pdsch_constants::CODEWORD_MAX_SIZE.value()),
    cb_stats(MAX_NOF_SEGMENTS)
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
    srsran_assert(decoder_pool, "Invalid hardware-accelerated PUSCH decoder pool.");
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
  srslog::basic_logger& logger;
  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_rx> segmenter;
  /// \brief Pointer to a CRC calculator for TB-wise checksum.
  ///
  /// Only the CRC calculator with generator polynomial crc_generator_poly::CRC24A, used for long transport blocks, is
  /// needed. Indeed, if a transport block is short enough not to be segmented, the CRC is verified by the decoder.
  sch_crc crc_set;
  /// Hardware decoder pool. It provides a hardware decoder for each processing thread.
  std::shared_ptr<hw_decoder_pool> decoder_pool;
  /// Asynchronous task executor. Set it to \c nullptr for disabling the asynchronous operation.
  task_executor* executor = nullptr;
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
  /// Enqueues code block decoder statistics.
  concurrent_queue<unsigned, concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep> cb_stats;

  // See interface for the documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override;

  // See interface for the documentation.
  void on_new_softbits(span<const log_likelihood_ratio> softbits) override;

  // See interface for the documentation.
  void on_end_softbits() override;

  /// \brief Processes
  void run_asynch_hw_decoder();

  /// \brief Checks the results of a decoding operation for a certain code block.
  ///
  /// It combines the decoder statistics \c stats and updates the code block CRC flag.
  ///
  /// \param[in,out] cb_crcs  Set of CRC flags.
  /// \param[in,out] decoder  Hardware decoder used to process the codeblocks.
  /// \param[in]     cb_id    Index of the CB (within the TB).
  /// \param[in]     crc_type Type of the CB CRC.
  /// \param[in]     data     Decoded data.
  /// \return Results of the decoding operation (CRC and number of LDPC decoder iterations).
  void check_hw_results(span<bool>                     cb_crcs,
                        hal::hw_accelerator_pusch_dec& decoder,
                        unsigned                       cb_id,
                        hal::hw_dec_cb_crc_type        crc_type,
                        bit_buffer                     data);

  /// \brief Copies the decoded bits and notifies the end of the operation.
  /// \param[in,out] decoder Hardware decoder used to process the codeblocks.
  /// \param[in]     cb_crcs Set of CRC flags.
  void copy_tb_and_notify(hw_decoder_pool::ptr decoder, span<const bool> cb_crcs);

  /// \brief Sets the segmentation and decoding parameters required by the hardware-accelerated PUSCH decoder function.
  /// \param[in,out] decoder          Selected hardware decoder to configure.
  /// \param[in]     nof_segments     Number of segments in the transport block.
  /// \param[in]     rm_length        Length of the rate-matched codeblock in bits.
  /// \param[in]     lifting_size     Length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  /// \param[in]     nof_segment_bits Number of information bits that is assigned to a segment.
  /// \param[in]     nof_filler_bits  Number of filler bits in a segment.
  /// \param[in]     crc_len          Length of the CB CRC in bits,
  /// \param[in]     crc_type         Type of the CB CRC.
  /// \param[in]     cb_index         Index of the CB (within the TB).
  void set_hw_dec_configuration(hal::hw_accelerator_pusch_dec& decoder,
                                unsigned                       nof_segments,
                                unsigned                       rm_length,
                                unsigned                       lifting_size,
                                unsigned                       nof_segment_bits,
                                unsigned                       nof_filler_bits,
                                unsigned                       crc_len,
                                hal::hw_dec_cb_crc_type        crc_type,
                                unsigned                       cb_index);
};

} // namespace srsran
