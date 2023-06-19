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
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/executors/task_executor.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

/// \brief Implements a PDSCH processor with concurrent codeblock processing.
/// \remark The number of PDSCH codeblock processor instances contained in \ref cb_processor_pool must be equal to or
/// greater than the number of consumers in \ref executor. Otherwise, an assertion is triggered in runtime.
class pdsch_processor_concurrent_impl : public pdsch_processor
{
public:
  /// \brief Creates a concurrent PDSCH processor with all the dependencies.
  /// \param[in] segmenter_         LDPC transmitter segmenter.
  /// \param[in] cb_processor_pool_ Codeblock processor pool, one instance per thread.
  /// \param[in] modulator_         PDSCH modulator.
  /// \param[in] dmrs_              DM-RS for PDSCH generator.
  /// \param[in] executor_          Asynchronous task executor.
  pdsch_processor_concurrent_impl(std::unique_ptr<ldpc_segmenter_tx>                      segmenter_,
                                  std::vector<std::unique_ptr<pdsch_codeblock_processor>> cb_processor_pool_,
                                  std::unique_ptr<pdsch_modulator>                        modulator_,
                                  std::unique_ptr<dmrs_pdsch_processor>                   dmrs_,
                                  task_executor&                                          executor_) :
    segmenter(std::move(segmenter_)),
    cb_processor_pool(std::move(cb_processor_pool_)),
    modulator(std::move(modulator_)),
    dmrs(std::move(dmrs_)),
    executor(executor_)
  {
    srsran_assert(segmenter != nullptr, "Invalid segmenter pointer.");
    srsran_assert(!cb_processor_pool.empty(), "CB processor pool is empty.");
    srsran_assert(std::find(cb_processor_pool.begin(), cb_processor_pool.end(), nullptr) == cb_processor_pool.end(),
                  "Invalid CB processor in pool.");
    srsran_assert(modulator != nullptr, "Invalid modulator pointer.");
    srsran_assert(dmrs != nullptr, "Invalid dmrs pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_mapper&                                        mapper,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override;

private:
  /// \brief Maximum codeblock length.
  ///
  /// This is the maximum length of an encoded codeblock, achievable with base graph 1 (rate 1/3).
  static constexpr units::bits MAX_CB_LENGTH{3 * MAX_SEG_LENGTH.value()};

  /// \brief Computes the number of RE used for mapping PDSCH data.
  ///
  /// The number of RE excludes the elements described by \c pdu reserved and the RE used for DMRS.
  ///
  /// \param[in] pdu Describes a PDSCH transmission.
  /// \return The number of resource elements.
  static unsigned compute_nof_data_re(const pdu_t& pdu);

  /// \brief Asserts PDU.
  ///
  /// It triggers an assertion if the PDU is not valid for this processor.
  void assert_pdu(const pdu_t& pdu) const;

  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Pointer to an LDPC encoder.
  std::vector<std::unique_ptr<pdsch_codeblock_processor>> cb_processor_pool;
  /// Buffer for storing data segments obtained after transport block segmentation.
  static_vector<described_segment, MAX_NOF_SEGMENTS> d_segments = {};

  std::mutex              cb_count_mutex;
  std::condition_variable cb_count_cvar;

  std::unique_ptr<pdsch_modulator>                                modulator;
  std::unique_ptr<dmrs_pdsch_processor>                           dmrs;
  std::array<uint8_t, pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_codeword;
  static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()>   temp_packed_codeword;
  task_executor&                                                  executor;
};

} // namespace srsran
