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

#pragma once

#include "pdsch_codeblock_processor.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// \brief Implements a PDSCH processor with concurrent codeblock processing.
/// \remark The number of PDSCH codeblock processor instances contained in \ref cb_processor_pool must be equal to or
/// greater than the number of consumers in \ref executor. Otherwise, an assertion is triggered at runtime.
class pdsch_processor_concurrent_impl : public pdsch_processor
{
public:
  /// Codeblock processor pool type.
  using codeblock_processor_pool = concurrent_thread_local_object_pool<pdsch_codeblock_processor>;
  /// PDSCH DM-RS generator pool type.
  using pdsch_dmrs_generator_pool = concurrent_thread_local_object_pool<dmrs_pdsch_processor>;
  /// PDSCH PT-RS generator pool type.
  using pdsch_ptrs_generator_pool = concurrent_thread_local_object_pool<ptrs_pdsch_generator>;

  /// \brief Creates a concurrent PDSCH processor with all the dependencies.
  /// \param[in] segmenter_            LDPC segmenter.
  /// \param[in] cb_processor_pool_    Codeblock processor pool.
  /// \param[in] scrambler_            Scrambling pseudo-random generator.
  /// \param[in] dmrs_generator_pool_  DM-RS for PDSCH generator.
  /// \param[in] executor_             Asynchronous task executor.
  pdsch_processor_concurrent_impl(std::unique_ptr<ldpc_segmenter_tx>         segmenter_,
                                  std::shared_ptr<codeblock_processor_pool>  cb_processor_pool_,
                                  std::unique_ptr<pseudo_random_generator>   scrambler_,
                                  std::unique_ptr<resource_grid_mapper>      mapper_,
                                  std::shared_ptr<pdsch_dmrs_generator_pool> dmrs_generator_pool_,
                                  std::shared_ptr<pdsch_ptrs_generator_pool> ptrs_generator_pool_,
                                  task_executor&                             executor_) :
    segmenter(std::move(segmenter_)),
    scrambler(std::move(scrambler_)),
    mapper(std::move(mapper_)),
    cb_processor_pool(std::move(cb_processor_pool_)),
    dmrs_generator_pool(std::move(dmrs_generator_pool_)),
    ptrs_generator_pool(std::move(ptrs_generator_pool_)),
    executor(executor_)
  {
    srsran_assert(segmenter, "Invalid LDPC segmenter.");
    srsran_assert(scrambler, "Invalid scrambler pointer.");
    srsran_assert(cb_processor_pool, "Invalid CB processor pool pointer.");
    srsran_assert(dmrs_generator_pool, "Invalid DM-RS pointer.");
    srsran_assert(ptrs_generator_pool, "Invalid PT-RS pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override;

private:
  /// Saves process() parameters for future uses during an asynchronous execution.
  void save_inputs(resource_grid_writer&                                           grid,
                   pdsch_processor_notifier&                                       notifier,
                   static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
                   const pdu_t&                                                    pdu);

  /// Creates code block processing batches and starts the asynchronous processing.
  void fork_cb_batches();

  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;
  /// Resource grid mapper.
  std::unique_ptr<resource_grid_mapper> mapper;
  /// Pool of code block processors.
  std::shared_ptr<codeblock_processor_pool> cb_processor_pool;
  /// DM-RS processor.
  std::shared_ptr<pdsch_dmrs_generator_pool> dmrs_generator_pool;
  /// PT-RS processor.
  std::shared_ptr<pdsch_ptrs_generator_pool> ptrs_generator_pool;
  /// Asynchronous task executor.
  task_executor& executor;
  /// Pointer to an LDPC segmenter output buffer interface.
  const ldpc_segmenter_buffer* segment_buffer;

  resource_grid_writer*     grid;
  pdsch_processor_notifier* notifier;
  shared_transport_block    data;
  pdsch_processor::pdu_t    config;

  /// Transport block size of the current transmission.
  units::bits tbs;
  /// Number of codeblocks of the current transmission.
  unsigned nof_cb = 0;
  /// Number of modulated channel symbols.
  unsigned nof_ch_symbols = 0;
  /// Number of information bits per codeblock.
  units::bits cb_info_bits = units::bits(0);
  /// LDPC segment length.
  units::bits segment_length = units::bits(0);
  /// Number of zeros that must be applied to the transport block.
  units::bits zero_pad = units::bits(0);
  /// Base codeblock metadata.
  codeblock_metadata cb_metadata;
  /// PDSCH transmission allocation pattern.
  re_pattern_list allocation;
  /// PDSCH transmission reserved elements pattern.
  re_pattern_list reserved;
  /// Precoding configuration scaled.
  precoding_configuration precoding;
  /// Codeblock resource block offset.
  static_vector<unsigned, MAX_NOF_SEGMENTS> re_offset;
  /// Codeblock counter.
  std::atomic<unsigned> cb_counter;
  /// Pending code block batch counter.
  std::atomic<unsigned> cb_task_counter;
  /// Pending asynchronous task counter (DM-RS and CB processing).
  std::atomic<unsigned> async_task_counter;
};

} // namespace srsran
