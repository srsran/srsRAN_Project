/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
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

  /// \brief Creates a concurrent PDSCH processor with all the dependencies.
  /// \param[in] segmenter_         LDPC transmitter segmenter.
  /// \param[in] cb_processor_pool_ Codeblock processor pool.
  /// \param[in] scrambler_         Scrambling pseudo-random generator.
  /// \param[in] dmrs_              DM-RS for PDSCH generator.
  /// \param[in] executor_          Asynchronous task executor.
  pdsch_processor_concurrent_impl(std::shared_ptr<codeblock_processor_pool>  cb_processor_pool_,
                                  std::unique_ptr<pseudo_random_generator>   scrambler_,
                                  std::shared_ptr<pdsch_dmrs_generator_pool> dmrs_generator_pool_,
                                  task_executor&                             executor_) :
    scrambler(std::move(scrambler_)),
    cb_processor_pool(std::move(cb_processor_pool_)),
    dmrs_generator_pool(std::move(dmrs_generator_pool_)),
    executor(executor_),
    temp_codeword(pdsch_constants::CODEWORD_MAX_SYMBOLS)
  {
    srsran_assert(scrambler, "Invalid scrambler pointer.");
    srsran_assert(cb_processor_pool, "Invalid CB processor pool pointer.");
    srsran_assert(dmrs_generator_pool, "Invalid DM-RS pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_mapper&                                        mapper,
               pdsch_processor_notifier&                                    notifier,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override;

private:
  /// \brief Computes the number of RE used for mapping PDSCH data.
  ///
  /// The number of RE excludes the elements described by \c pdu as reserved and the RE used for DM-RS.
  ///
  /// \param[in] pdu Describes a PDSCH transmission.
  /// \return The number of resource elements.
  static unsigned compute_nof_data_re(const pdu_t& pdu);

  /// Saves process() parameters for future uses during an asynchronous execution.
  void save_inputs(resource_grid_mapper&                                        mapper,
                   pdsch_processor_notifier&                                    notifier,
                   static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                   const pdu_t&                                                 pdu);

  /// \brief Asserts PDU.
  ///
  /// It triggers an assertion if the PDU is not valid for this processor.
  void assert_pdu() const;

  /// Creates code block processing batches and starts the asynchronous processing.
  void fork_cb_batches();

  /// Processes PDSCH DM-RS.
  void process_dmrs();

  /// \brief Maps the PDSCH resource elements.
  /// \param[in]  buffer  Symbols after modulation mapping.
  /// \param[in]  config  Necessary parameters to process the PDSCH transmission.
  void map(span<const ci8_t> codeword);

  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;
  /// Pool of code block processors.
  std::shared_ptr<codeblock_processor_pool> cb_processor_pool;
  /// DM-RS processor.
  std::shared_ptr<pdsch_dmrs_generator_pool> dmrs_generator_pool;
  /// Asynchronous task executor.
  task_executor& executor;

  resource_grid_mapper*     mapper;
  pdsch_processor_notifier* notifier;
  span<const uint8_t>       data;
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
  codeblock_metadata cb_metadata = {};

  /// Rate matching length in bits for each of the segments.
  static_vector<units::bits, MAX_NOF_SEGMENTS> rm_length;
  /// Codeblock bit offset within the codeword.
  static_vector<units::bits, MAX_NOF_SEGMENTS> cw_offset;
  /// Buffer for storing the modulated codeword.
  std::vector<ci8_t> temp_codeword;
  /// Pending code block batch counter.
  std::atomic<unsigned> cb_batch_counter;
  /// Pending asynchronous task counter (DM-RS and CB processing).
  std::atomic<unsigned> async_task_counter;
};

} // namespace srsran
