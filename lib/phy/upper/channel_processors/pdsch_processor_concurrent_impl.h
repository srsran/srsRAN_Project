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
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/executors/task_executor.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

/// \brief Implements a PDSCH processor with concurrent codeblock processing.
/// \remark The number of PDSCH codeblock processor instances contained in \ref cb_processor_pool must be equal to or
/// greater than the number of consumers in \ref executor. Otherwise, an assertion is triggered at runtime.
class pdsch_processor_concurrent_impl : public pdsch_processor
{
public:
  /// \brief Creates a concurrent PDSCH processor with all the dependencies.
  /// \param[in] segmenter_         LDPC transmitter segmenter.
  /// \param[in] cb_processor_pool_ Codeblock processor pool, one instance per thread.
  /// \param[in] scrambler_         Scrambling pseudo-random generator.
  /// \param[in] dmrs_              DM-RS for PDSCH generator.
  /// \param[in] executor_          Asynchronous task executor.
  pdsch_processor_concurrent_impl(std::unique_ptr<ldpc_segmenter_tx>                      segmenter_,
                                  std::vector<std::unique_ptr<pdsch_codeblock_processor>> cb_processor_pool_,
                                  std::unique_ptr<pseudo_random_generator>                scrambler_,
                                  std::unique_ptr<dmrs_pdsch_processor>                   dmrs_,
                                  task_executor&                                          executor_) :
    segmenter(std::move(segmenter_)),
    scrambler(std::move(scrambler_)),
    cb_processor_pool(std::move(cb_processor_pool_)),
    dmrs(std::move(dmrs_)),
    executor(executor_)
  {
    srsran_assert(segmenter != nullptr, "Invalid segmenter pointer.");
    srsran_assert(!cb_processor_pool.empty(), "CB processor pool is empty.");
    srsran_assert(std::find(cb_processor_pool.begin(), cb_processor_pool.end(), nullptr) == cb_processor_pool.end(),
                  "Invalid CB processor in pool.");
    srsran_assert(scrambler != nullptr, "Invalid scrambler pointer.");
    srsran_assert(dmrs != nullptr, "Invalid dmrs pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_mapper&                                        mapper,
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

  /// \brief Asserts PDU.
  ///
  /// It triggers an assertion if the PDU is not valid for this processor.
  void assert_pdu(const pdu_t& pdu) const;

  /// \brief Processes PDSCH DM-RS.
  /// \param[out] mapper Resource grid mapper interface.
  /// \param[in]  pdu    Necessary parameters to process the PDSCH transmission.
  void process_dmrs(resource_grid_mapper& mapper, const pdu_t& pdu);

  /// \brief Maps the PDSCH resource elements.
  /// \param[out] mapper  Resource grid mapper interface.
  /// \param[in]  data_re Resource elements for mapping.
  /// \param[in]  config  Necessary parameters to process the PDSCH transmission.
  void map(resource_grid_mapper& mapper, const re_buffer_reader& data_re, const pdu_t& config);

  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;
  /// Pool of code block processors.
  std::vector<std::unique_ptr<pdsch_codeblock_processor>> cb_processor_pool;
  /// DM-RS processor.
  std::unique_ptr<dmrs_pdsch_processor> dmrs;
  /// Asynchronous task executor.
  task_executor& executor;

  /// Buffer for storing data segments obtained after transport block segmentation.
  static_vector<described_segment, MAX_NOF_SEGMENTS> d_segments = {};
  /// Buffer for storing modulated data ready for mapping.
  static_re_buffer<pdsch_constants::MAX_NOF_LAYERS, pdsch_constants::CODEWORD_MAX_NOF_RE> temp_re;

  /// Mutex for protecting code block counter.
  std::mutex cb_count_mutex;
  /// Condition variable for notifying the codeblock count change.
  std::condition_variable cb_count_cvar;
};

} // namespace srsran
