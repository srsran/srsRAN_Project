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

#pragma once

#include "pusch_codeblock_decoder.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

// Number of bits in one byte.
static constexpr unsigned BITS_PER_BYTE = 8;

// Maximum TBS that implies a 16-bit CRC.
constexpr unsigned MAX_BITS_CRC16 = 3824;

/// Implementation of the PUSCH decoder.
class pusch_decoder_impl : public pusch_decoder, private pusch_decoder_buffer
{
public:
  /// Code block decoder pool type.
  using codeblock_decoder_pool = concurrent_thread_local_object_pool<pusch_codeblock_decoder>;

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
  /// Sets up the internal components, namely LDPC segmenter, LDPC rate dematcher, LDPC decoder and all the CRC
  /// calculators.
  ///
  /// \param[in] segmenter_    LDPC segmenter.
  /// \param[in] decoder_pool_ Codeblock decoder.
  /// \param[in] crc_set_      Structure with pointers to three CRC calculator objects, with generator
  ///                          polynomials of type \c CRC16, \c CRC24A and \c CRC24B.
  /// \param[in] executor_     Task executor for asynchronous PUSCH code block decoding.
  pusch_decoder_impl(std::unique_ptr<ldpc_segmenter_rx>      segmenter_,
                     std::shared_ptr<codeblock_decoder_pool> decoder_pool_,
                     sch_crc                                 crc_set_,
                     task_executor*                          executor_) :
    segmenter(std::move(segmenter_)),
    decoder_pool(std::move(decoder_pool_)),
    crc_set(std::move(crc_set_)),
    executor(executor_),
    softbits_buffer(pusch_constants::CODEWORD_MAX_SIZE.value()),
    cb_stats(MAX_NOF_SEGMENTS)
  {
    srsran_assert(segmenter, "Invalid segmenter.");
    srsran_assert(decoder_pool, "Invalid codeblock decoder pool.");
    srsran_assert(crc_set.crc16, "Invalid CRC16 calculator.");
    srsran_assert(crc_set.crc24A, "Invalid CRC24A calculator.");
    srsran_assert(crc_set.crc24B, "Invalid CRC24B calculator.");
    srsran_assert(crc_set.crc16->get_generator_poly() == crc_generator_poly::CRC16, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24A->get_generator_poly() == crc_generator_poly::CRC24A, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24B->get_generator_poly() == crc_generator_poly::CRC24B, "Wrong TB CRC calculator.");
    if (executor != nullptr) {
      srsran_assert(decoder_pool->capacity() > 1,
                    "The number of PUSCH code block decoder in the pool (i.e., {}) must be greater than one.",
                    decoder_pool->capacity());
    }
  };

  // See interface for the documentation.
  pusch_decoder_buffer& new_data(span<uint8_t>           transport_block,
                                 unique_rx_buffer        rm_buffer,
                                 pusch_decoder_notifier& notifier,
                                 const configuration&    cfg) override;

private:
  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_rx> segmenter;
  /// Pointer to a codeblock decoder.
  std::shared_ptr<codeblock_decoder_pool> decoder_pool;
  /// \brief Pointer to a CRC calculator for TB-wise checksum.
  ///
  /// Only the CRC calculator with generator polynomial crc_generator_poly::CRC24A, used for long transport blocks, is
  /// needed. Indeed, if a transport block is short enough not to be segmented, the CRC is verified by the decoder.
  sch_crc crc_set;
  /// Optional task executor. Used for accelerating the PUSCH decoding at code block level. Set to \c nullptr for no
  /// concurrent execution.
  task_executor* executor;
  /// Soft bit buffer.
  std::vector<log_likelihood_ratio> softbits_buffer;
  /// Counts the number of soft bits in the buffer.
  unsigned softbits_count;
  /// Current transport block.
  span<uint8_t> transport_block;
  /// Current soft bits buffer.
  unique_rx_buffer unique_rm_buffer;
  /// Current notifier.
  pusch_decoder_notifier* result_notifier = nullptr;
  /// Current PUSCH decoder configuration.
  pusch_decoder::configuration current_config;
  /// Temporary buffer to store the rate-matched codeblocks (represented by LLRs) and their metadata.
  static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS> codeblock_llrs;
  /// Counts code blocks.
  std::atomic<unsigned> cb_counter;
  /// Enqueues code block decoder statistics.
  concurrent_queue<unsigned, concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep> cb_stats;

  // See interface for the documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override;

  // See interface for the documentation.
  void on_new_softbits(span<const log_likelihood_ratio> softbits) override;

  // See interface for the documentation.
  void on_end_softbits() override;

  /// \brief Joins the multiple code block processing.
  ///
  /// Called from the last decoding code block task. It concatenates code blocks and checks the decoded transport block
  /// CRC if applicable. Also, it notifies the decoder result.
  void join_and_notify();

  /// Concatenates code blocks and returns the CRC checksum.
  unsigned concatenate_codeblocks();
};

} // namespace srsran
