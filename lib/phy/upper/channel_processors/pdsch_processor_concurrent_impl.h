/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "pdsch_codeblock_processor.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

/// \brief Implements a PDSCH processor with concurrent codeblock processing.
/// \remark The number of PDSCH codeblock processor instances contained in \ref cb_processor_pool must be equal to or
/// greater than the number of consumers in \ref executor. Otherwise, an assertion is triggered at runtime.
class pdsch_processor_concurrent_impl : public pdsch_processor
{
public:
  /// Codeblock processor pool type.
  using codeblock_processor_pool = concurrent_thread_local_object_pool<std::unique_ptr<pdsch_codeblock_processor>>;

  /// \brief Creates a concurrent PDSCH processor with all the dependencies.
  /// \param[in] segmenter_         LDPC transmitter segmenter.
  /// \param[in] cb_processor_pool_ Codeblock processor pool.
  /// \param[in] scrambler_         Scrambling pseudo-random generator.
  /// \param[in] dmrs_              DM-RS for PDSCH generator.
  /// \param[in] executor_          Asynchronous task executor.
  pdsch_processor_concurrent_impl(std::unique_ptr<ldpc_segmenter_tx>        segmenter_,
                                  std::shared_ptr<codeblock_processor_pool> cb_processor_pool_,
                                  std::unique_ptr<pseudo_random_generator>  scrambler_,
                                  std::unique_ptr<dmrs_pdsch_processor>     dmrs_,
                                  task_executor&                            executor_) :
    segmenter(std::move(segmenter_)),
    scrambler(std::move(scrambler_)),
    cb_processor_pool(std::move(cb_processor_pool_)),
    dmrs(std::move(dmrs_)),
    executor(executor_),
    temp_codeword(pdsch_constants::CODEWORD_MAX_SYMBOLS)
  {
    srsran_assert(segmenter != nullptr, "Invalid segmenter pointer.");
    srsran_assert(scrambler != nullptr, "Invalid scrambler pointer.");
    srsran_assert(dmrs != nullptr, "Invalid dmrs pointer.");
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
  void fork_cb_batches(span<ci8_t> codeword);

  /// Processes PDSCH DM-RS.
  void process_dmrs();

  /// \brief Maps the PDSCH resource elements.
  /// \param[in]  buffer  Symbols after modulation mapping.
  /// \param[in]  config  Necessary parameters to process the PDSCH transmission.
  void map(span<const ci8_t> codeword);

  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;
  /// Pool of code block processors.
  std::shared_ptr<codeblock_processor_pool> cb_processor_pool;
  /// DM-RS processor.
  std::unique_ptr<dmrs_pdsch_processor> dmrs;
  /// Asynchronous task executor.
  task_executor& executor;

  resource_grid_mapper*     mapper;
  pdsch_processor_notifier* notifier;
  span<const uint8_t>       data;
  pdsch_processor::pdu_t    config;

  /// Buffer for storing data segments obtained after transport block segmentation.
  static_vector<described_segment, MAX_NOF_SEGMENTS> d_segments = {};
  /// Buffer for storing the modulated codeword.
  std::vector<ci8_t> temp_codeword;
  /// Pending code block batch counter.
  std::atomic<unsigned> cb_batch_counter;
  /// Pending asynchronous task counter (DM-RS and CB processing).
  std::atomic<unsigned> async_task_counter;
};

} // namespace srsran
