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

#include "pusch_processor_notifier_adaptor.h"
#include "pusch_uci_decoder_wrapper.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/ulsch_demultiplex.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include "srsran/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"
#include <memory>

namespace srsran {

/// Implements a generic software PUSCH processor.
class pusch_processor_impl : public pusch_processor
{
public:
  /// The current maximum supported number of layers.
  static constexpr unsigned max_nof_layers = 4;

  /// Groups the PUSCH processor dependencies that can be reused locally by the same processing thread.
  class concurrent_dependencies
  {
  public:
    /// Creates the dependencies instance.
    concurrent_dependencies(std::unique_ptr<dmrs_pusch_estimator>         estimator_,
                            std::unique_ptr<pusch_demodulator>            demodulator_,
                            std::unique_ptr<ulsch_demultiplex>            demultiplex_,
                            std::unique_ptr<uci_decoder>                  uci_dec_,
                            channel_estimate::channel_estimate_dimensions ce_dims) :
      estimator(std::move(estimator_)),
      demodulator(std::move(demodulator_)),
      demultiplex(std::move(demultiplex_)),
      uci_dec(std::move(uci_dec_)),
      ch_estimate(ce_dims),
      harq_ack_decoder(*uci_dec,
                       pusch_constants::get_max_codeword_size(ce_dims.nof_prb, ce_dims.nof_tx_layers).value()),
      csi_part1_decoder(*uci_dec,
                        pusch_constants::get_max_codeword_size(ce_dims.nof_prb, ce_dims.nof_tx_layers).value()),
      csi_part2_decoder(*uci_dec,
                        pusch_constants::get_max_codeword_size(ce_dims.nof_prb, ce_dims.nof_tx_layers).value())

    {
      srsran_assert(estimator, "Invalid channel estimator.");
      srsran_assert(demodulator, "Invalid demodulator.");
      srsran_assert(demultiplex, "Invalid demultiplex.");
      srsran_assert(uci_dec, "Invalid UCI decoder.");
    }

    dmrs_pusch_estimator&      get_estimator() { return *estimator; }
    pusch_demodulator&         get_demodulator() { return *demodulator; }
    ulsch_demultiplex&         get_demultiplex() { return *demultiplex; }
    pusch_uci_decoder_wrapper& get_harq_ack_decoder() { return harq_ack_decoder; }
    pusch_uci_decoder_wrapper& get_csi_part1_decoder() { return csi_part1_decoder; }
    pusch_uci_decoder_wrapper& get_csi_part2_decoder() { return csi_part2_decoder; }
    channel_estimate&          get_channel_estimate() { return ch_estimate; }

  private:
    /// Channel estimator instance.
    std::unique_ptr<dmrs_pusch_estimator> estimator;
    /// Demodulator instance.
    std::unique_ptr<pusch_demodulator> demodulator;
    /// Channel demultiplex.
    std::unique_ptr<ulsch_demultiplex> demultiplex;
    /// UCI Decoder instance.
    std::unique_ptr<uci_decoder> uci_dec;
    /// Temporal channel estimate.
    channel_estimate ch_estimate;
    /// HARQ-ACK decoder wrapper.
    pusch_uci_decoder_wrapper harq_ack_decoder;
    /// CSI Part 1 decoder wrapper.
    pusch_uci_decoder_wrapper csi_part1_decoder;
    /// CSI Part 2 decoder wrapper.
    pusch_uci_decoder_wrapper csi_part2_decoder;
  };

  /// Dependencies pool class.
  using concurrent_dependencies_pool_type = bounded_unique_object_pool<concurrent_dependencies>;

  /// Collects the necessary parameters for creating a PUSCH processor.
  struct configuration {
    /// Dependencies pool.
    std::shared_ptr<concurrent_dependencies_pool_type> dependencies_pool;
    /// Decoder instance. Ownership is transferred to the processor.
    std::unique_ptr<pusch_decoder> decoder;
    /// Selects the number of LDPC decoder iterations.
    unsigned dec_nof_iterations;
    /// Enables LDPC decoder early stop if the CRC matches before completing \c ldpc_nof_iterations iterations.
    bool dec_enable_early_stop;
    /// Set to true for decoding even if the number of valid soft bits is insufficient.
    bool dec_force_decoding;
    /// PUSCH SINR calculation method for CSI reporting.
    channel_state_information::sinr_type csi_sinr_calc_method;
  };

  /// \brief Constructs a generic software PUSCH processor.
  /// \param[in] config PUSCH processor dependencies and configuration parameters.
  pusch_processor_impl(configuration& config);

  // See interface for documentation.
  void process(span<uint8_t>                    data,
               unique_rx_buffer                 rm_buffer,
               pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override;

private:
  /// \brief Notifier for the PUSCH channel estimator.
  ///
  /// At the notification by the DM-RS PUSCH estimator, the notified PUSCH processor recovers the PUSCH data by running
  /// the demodulation and decoding steps.
  ///
  /// The notifier is available only after calling the \c configure public method.
  class dmrs_pusch_estimator_notifier_impl : private dmrs_pusch_estimator_notifier
  {
  public:
    /// Constructor: initializes the reference to the notified PUSCH processor.
    explicit dmrs_pusch_estimator_notifier_impl(pusch_processor_impl& pp) : notified_processor(pp) {}

    /// \brief Configures the notifier for the current PUSCH transmission.
    /// \param[out]    data_        Received transport block.
    /// \param[in,out] rm_buffer_   Rate matcher buffer.
    /// \param[in,out] dependencies Pointer to the dependencies object assigned to the PUSCH processor.
    /// \param[in]     notifier_    Result notification interface.
    /// \param[in]     grid_        Source resource grid.
    /// \param[in]     pdu_         Necessary parameters to process the PUSCH transmission.
    /// \param[in]     dmrs_type_   DM-RS type used by the PUSCH transmission.
    /// \param[in]     cdm_         Number of CDM groups without data in the PUSCH transmission.
    /// \return A reference to the configured notifier.
    dmrs_pusch_estimator_notifier& configure(span<uint8_t>                          data_,
                                             unique_rx_buffer                       rm_buffer_,
                                             concurrent_dependencies_pool_type::ptr dependencies_,
                                             pusch_processor_result_notifier&       notifier_,
                                             const resource_grid_reader&            grid_,
                                             const pdu_t&                           pdu_,
                                             const dmrs_type&                       dmrs_type_,
                                             unsigned                               cdm_)
    {
      // Set new PUSCH reception parameters. Use exchange for verifying that previous receptions are not overwritten.
      [[maybe_unused]] auto prev_data         = std::exchange(data, data_);
      [[maybe_unused]] auto prev_rm_buffer    = std::exchange(rm_buffer, std::move(rm_buffer_));
      [[maybe_unused]] auto prev_notifier     = std::exchange(notifier, &notifier_);
      [[maybe_unused]] auto prev_dependencies = std::exchange(dependencies, std::move(dependencies_));
      [[maybe_unused]] auto prev_grid         = std::exchange(grid, &grid_);
      [[maybe_unused]] auto prev_pdu          = std::exchange(pdu, &pdu_);
      used_dmrs_type                          = dmrs_type_;
      nof_cdm_groups_without_data             = cdm_;

      // Ensure that no parameter was overwritten.
      srsran_assert(prev_data.data() == nullptr, "Detected data overwrite.");
      srsran_assert(!prev_rm_buffer.is_valid(), "Detected RM buffer overwrite.");
      srsran_assert(prev_notifier == nullptr, "Detected notifier overwrite.");
      srsran_assert(prev_dependencies == nullptr, "Detected dependencies overwrite.");
      srsran_assert(prev_grid == nullptr, "Detected grid overwrite.");
      srsran_assert(prev_pdu == nullptr, "Detected PDU overwrite.");

      // Return its own reference to the estimator callback interface.
      return *this;
    }

  private:
    // See interface for documentation.
    void on_estimation_complete() override
    {
      // Move current transmission parameters to the stack.
      pusch_processor_result_notifier* current_notifier = std::exchange(notifier, nullptr);
      const resource_grid_reader*      current_grid     = std::exchange(grid, nullptr);
      const pdu_t*                     current_pdu      = std::exchange(pdu, nullptr);
      span<uint8_t>                    current_data     = std::exchange(data, {});

      // Verify the parameters are valid before continuing with the processing.
      srsran_assert(current_notifier != nullptr, "Invalid notifier.");
      srsran_assert(current_grid != nullptr, "Invalid grid.");
      srsran_assert(current_pdu != nullptr, "Invalid PDU.");
      srsran_assert((current_data.data() != nullptr) && (!current_data.empty()), "Invalid data.");

      // Keep processing the PUSCH reception. The notifier can be configured for a different reception before returning.
      notified_processor.process_data(current_data,
                                      std::move(rm_buffer),
                                      std::move(dependencies),
                                      *current_notifier,
                                      *current_grid,
                                      *current_pdu,
                                      used_dmrs_type,
                                      nof_cdm_groups_without_data);
    }

    /// Reference to the PUSCH processor waiting for notification.
    pusch_processor_impl& notified_processor;
    /// Pointer to the dependencies object assigned to the PUSCH processor.
    concurrent_dependencies_pool_type::ptr dependencies = {};
    /// Buffer to store retrieved data.
    span<uint8_t> data = {};
    /// Rate matcher buffer.
    unique_rx_buffer rm_buffer = unique_rx_buffer();
    /// Pointer to the PUSCH processor notifier passed to the notified processor.
    pusch_processor_result_notifier* notifier = nullptr;
    /// Pointer to the reader of the resource grid containing the PUSCH transmission.
    const resource_grid_reader* grid = nullptr;
    /// Pointer to the PDU describing the processed PUSCH transmission.
    const pdu_t* pdu = nullptr;
    /// DM-RS type.
    dmrs_type used_dmrs_type = dmrs_type::TYPE1;
    /// Number of CDM groups without data.
    unsigned nof_cdm_groups_without_data = 2;
  };
  /// Channel estimator notifier configurator.
  dmrs_pusch_estimator_notifier_impl estimator_notifier_configurator;

  srslog::basic_logger& logger;
  /// Dependencies pool.
  std::shared_ptr<concurrent_dependencies_pool_type> dependencies_pool;
  /// UL-SCH transport block decoder.
  std::unique_ptr<pusch_decoder> decoder;
  /// Selects the number of LDPC decoder iterations.
  unsigned dec_nof_iterations;
  /// Set to true for decoding even if the number of valid soft bits is insufficient.
  bool force_decoding;
  /// Enables LDPC decoder early stop if the CRC matches before completing \c ldpc_nof_iterations iterations.
  bool dec_enable_early_stop;
  /// Selects the PUSCH SINR calculation method.
  channel_state_information::sinr_type csi_sinr_calc_method;
  /// Notifier adaptor.
  pusch_processor_notifier_adaptor notifier_adaptor;

  /// \brief Processes the data in a PUSCH transmission.
  /// \param[out]    data                         Received transport block.
  /// \param[in,out] rm_buffer                    Rate matcher buffer.
  /// \param[in,out] dependencies                 Pointer to the dependencies object assigned to the PUSCH processor.
  /// \param[in]     notifier                     Result notification interface.
  /// \param[in]     grid                         Source resource grid.
  /// \param[in]     pdu                          Necessary parameters to process the PUSCH transmission.
  /// \param[in]     dmrs_type                    DM-RS type used by the PUSCH transmission.
  /// \param[in]     nof_cdm_groups_without_data  Number of CDM groups without data in the PUSCH transmission.
  void process_data(span<uint8_t>                          data,
                    unique_rx_buffer                       rm_buffer,
                    concurrent_dependencies_pool_type::ptr dependencies,
                    pusch_processor_result_notifier&       notifier,
                    const resource_grid_reader&            grid,
                    const pdu_t&                           pdu,
                    const dmrs_type&                       dmrs_type,
                    unsigned                               nof_cdm_groups_without_data);
};

} // namespace srsran
