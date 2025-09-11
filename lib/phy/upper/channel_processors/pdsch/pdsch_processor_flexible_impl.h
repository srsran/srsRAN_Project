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

#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_block_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// \brief Implements a flexible PDSCH processor with parameterizable concurrent codeblock processing and memory
/// footprint.
///
/// \remark The number of PDSCH codeblock processor instances contained in \ref block_processor_pool must be equal to or
/// greater than the number of consumers in \ref executor. Otherwise, an assertion is triggered at runtime.
class pdsch_processor_flexible_impl : public pdsch_processor
{
public:
  /// PDSCH block processor pool type.
  using pdsch_block_processor_pool = bounded_unique_object_pool<pdsch_block_processor>;
  /// PDSCH DM-RS generator pool type.
  using pdsch_dmrs_generator_pool = bounded_unique_object_pool<dmrs_pdsch_processor>;
  /// PDSCH PT-RS generator pool type.
  using pdsch_ptrs_generator_pool = bounded_unique_object_pool<ptrs_pdsch_generator>;

  /// \brief Creates a flexible PDSCH processor with all the dependencies.
  /// \param[in] segmenter_                    LDPC segmenter.
  /// \param[in] mapper_                       Grid mapper.
  /// \param[in] block_processor_pool_         Block processor pool.
  /// \param[in] dmrs_generator_pool_          DM-RS for PDSCH generator.
  /// \param[in] ptrs_generator_pool_          PT-RS for PDSCH generator.
  /// \param[in] executor_                     Asynchronous task executor.
  /// \param[in] max_nof_codeblocks_per_batch_ Maximum number of codeblocks per processing batch.
  pdsch_processor_flexible_impl(std::unique_ptr<ldpc_segmenter_tx>          segmenter_,
                                std::unique_ptr<resource_grid_mapper>       mapper_,
                                std::shared_ptr<pdsch_block_processor_pool> block_processor_pool_,
                                std::shared_ptr<pdsch_dmrs_generator_pool>  dmrs_generator_pool_,
                                std::shared_ptr<pdsch_ptrs_generator_pool>  ptrs_generator_pool_,
                                task_executor&                              executor_,
                                unsigned                                    max_nof_codeblocks_per_batch_) :
    logger(srslog::fetch_basic_logger("PHY")),
    segmenter(std::move(segmenter_)),
    mapper(std::move(mapper_)),
    block_processor_pool(std::move(block_processor_pool_)),
    dmrs_generator_pool(std::move(dmrs_generator_pool_)),
    ptrs_generator_pool(std::move(ptrs_generator_pool_)),
    executor(executor_),
    max_nof_codeblocks_per_batch(max_nof_codeblocks_per_batch_)
  {
    srsran_assert(segmenter, "Invalid LDPC segmenter pointer.");
    srsran_assert(mapper, "Invalid resource grid mapper pointer.");
    srsran_assert(block_processor_pool, "Invalid CB processor pool pointer.");
    srsran_assert(dmrs_generator_pool, "Invalid DM-RS pointer.");
    srsran_assert(ptrs_generator_pool, "Invalid PT-RS pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override;

private:
  /// Configures a new transmission and saves process() parameters for future uses during an asynchronous execution.
  void initialize_new_transmission(resource_grid_writer&                                           grid,
                                   pdsch_processor_notifier&                                       notifier,
                                   static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
                                   const pdu_t&                                                    pdu);

  /// Synchronous CB processing.
  void sync_pdsch_cb_processing();

  /// Creates code block processing batches and starts the asynchronous processing.
  void fork_cb_batches();

  srslog::basic_logger& logger;
  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Resource grid mapper.
  std::unique_ptr<resource_grid_mapper> mapper;
  /// Pool of block processors.
  std::shared_ptr<pdsch_block_processor_pool> block_processor_pool;
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

  /// Number of codeblocks of the current transmission.
  unsigned nof_cb = 0;
  /// Maximum number of codeblocks per batch.
  unsigned max_nof_codeblocks_per_batch;
  /// Indicates whether the current transmission is concurrent (true) or not.
  bool async_proc = false;
  /// PDSCH transmission allocation pattern.
  resource_grid_mapper::allocation_configuration allocation;
  /// PDSCH transmission reserved elements pattern.
  re_pattern_list reserved;
  /// Precoding configuration scaled.
  precoding_configuration precoding;
  /// Codeblock resource block offset.
  static_vector<unsigned, MAX_NOF_SEGMENTS> re_offset;
  /// Pending code block batch counter.
  std::atomic<unsigned> cb_task_counter;
  /// Pending asynchronous task counter (DM-RS and CB processing).
  std::atomic<unsigned> async_task_counter;
};

} // namespace srsran
