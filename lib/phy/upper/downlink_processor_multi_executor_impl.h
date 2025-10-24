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

#include "downlink_processor_multi_executor_state.h"
#include "srsran/adt/unique_function.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class upper_phy_rg_gateway;
class task_executor;

namespace detail {

class downlink_processor_callback
{
public:
  virtual ~downlink_processor_callback() = default;

  /// Decrements the number of pending PDUs to be processed and tries to send the resource grid through the gateway.
  virtual void on_task_completion() = 0;
};

} // namespace detail

/// \brief Implementation of a downlink processor.
///
/// This implementation process the PDUs and config using one executor, which
/// hides the execution threading model. The implementation ensures that all the PDUs and configs will be processed
/// before transmitting the resource grid through the gateway, even if finish_processing_pdus() method is called before
/// every PDU is processed.
/// \note This implementation follows the same method order that the interface. Not following this order will result in
/// undefined behaviour. As a reminder, the order is the following: first configure the downlink processor by calling
/// configure_resource_grid(). After it is configured it can start to process PDUs. Finally, when no more PDUs need to
/// be processed, finish_processing_pdus() can be called so the grid can be send through the gateway. Note that
/// finish_processing_pdus() can be called before the PDUs finished processing, but the resource grid is sent through
/// the gateway as soon as every enqueued PDU before finish_processing_pdus() is processed . This is controlled counting
/// the PDUs that are processed and finished processing.
///  \note Thread safe class.
class downlink_processor_multi_executor_impl : public downlink_processor_base,
                                               private downlink_processor_controller,
                                               private unique_downlink_processor::downlink_processor_callback,
                                               private detail::downlink_processor_callback
{
public:
  /// \brief Builds a downlink processor single executor impl object with the given parameters.
  ///
  /// \param gateway_         Gateway that will be used to send the resource grids.
  /// \param pdcch_proc_      Processor for PDCCH PDUs.
  /// \param pdsch_proc_      Processor for PDSCH PDUs.
  /// \param ssb_proc_        Processor for SSB PDUs.
  /// \param csi_rs_proc_     Processor for CSI-RS processor used PDUs.
  /// \param prs_gen_         Processor for PRS PDUs.
  /// \param pdcch_executor_  Task executor that will be used to process PDCCH.
  /// \param pdsch_executor_  Task executor that will be used to process PDSCH.
  /// \param ssb_executor_    Task executor that will be used to process SS/PBCH block.
  /// \param csi_rs_executor_ Task executor that will be used to process NZP-CSI-RS.
  /// \param prs_executor_    Task executor that will be used to process PRS.
  /// \param logger_          Logger instance.
  downlink_processor_multi_executor_impl(upper_phy_rg_gateway&                 gateway_,
                                         std::unique_ptr<pdcch_processor>      pdcch_proc_,
                                         std::unique_ptr<pdsch_processor>      pdsch_proc_,
                                         std::unique_ptr<ssb_processor>        ssb_proc_,
                                         std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc_,
                                         std::unique_ptr<prs_generator>        prs_gen_,
                                         task_executor&                        pdcch_executor_,
                                         task_executor&                        pdsch_executor_,
                                         task_executor&                        ssb_executor_,
                                         task_executor&                        csi_rs_executor_,
                                         task_executor&                        prs_executor_,
                                         srslog::basic_logger&                 logger_);

  // See downlink_processor_base interface for documentation.
  downlink_processor_controller& get_controller() override { return *this; }

  // See downlink_processor_base interface for documentation.
  void stop() override { state.stop(); }

private:
  // See downlink_processor_controller interface for documentation.
  unique_downlink_processor configure_resource_grid(const resource_grid_context& context,
                                                    shared_resource_grid         grid) override;
  // See interface for documentation.
  void process_pdcch(const pdcch_processor::pdu_t& pdu) override;

  // See interface for documentation.
  void process_pdsch(static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data,
                     const pdsch_processor::pdu_t&                                                    pdu) override;

  // See interface for documentation.
  void process_ssb(const ssb_processor::pdu_t& pdu) override;

  // See interface for documentation.
  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override;

  // See interface for documentation.
  void process_prs(const prs_generator_configuration& config) override;

  // See unique_downlink_processor::downlink_processor_callback interface for documentation.
  void finish_processing_pdus() override;

  class pdsch_processor_notifier_wrapper : public pdsch_processor_notifier
  {
  public:
    pdsch_processor_notifier_wrapper(detail::downlink_processor_callback& callback_) : callback(callback_)
    {
      // Do nothing.
    }

    void on_finish_processing() override
    {
      l1_dl_tracer << instant_trace_event("pdsch_on_finish_processing", instant_trace_event::cpu_scope::thread);

      callback.on_task_completion();
    }

  private:
    detail::downlink_processor_callback& callback;
  };

  /// Wraps a task executor that forbids memory allocation by design.
  class downlink_task_executor
  {
  public:
    using task_type = unique_function<void(), default_unique_task_buffer_size, true>;

    downlink_task_executor(task_executor& executor_) : executor(executor_) {}

    bool defer(task_type&& task) { return executor.defer(std::move(task)); }

  private:
    task_executor& executor;
  };

  /// Holds the PDSCH processor arguments.
  struct pdsch_proc_args {
    pdsch_proc_args(const pdsch_processor::pdu_t&                                                    pdu_,
                    static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data_) :
      pdu(pdu_), data(std::move(data_))
    {
    }

    pdsch_processor::pdu_t                                                           pdu;
    static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data;
  };

  /// Sends the resource grid and updates the processor state to allow configuring a new resource grid.
  void send_resource_grid();

  /// Decrements the number of pending PDUs to be processed and tries to send the resource grid through the gateway.
  void on_task_completion() override;

  /// Resource grid gateway - it delivers the resource grid when the processing is finished.
  upper_phy_rg_gateway& gateway;
  /// Processing context.
  resource_grid_context rg_context;
  /// Configured resource grid.
  shared_resource_grid current_grid;
  /// \defgroup dl_pdu_list
  /// \brief Temporary storage of transmit PDUs for asynchronous processing.
  ///
  /// The downlink processor might process transmission asynchronously. As the transmission configuration structures
  /// might be large, the parameters are stored temporally in the downlink processor.
  ///
  /// The lists are cleared upon the resource grid configuration before start the execution.
  /// @{
  static_vector<pdcch_processor::pdu_t, MAX_DL_PDCCH_PDUS_PER_SLOT + MAX_UL_PDCCH_PDUS_PER_SLOT> pdcch_list;
  static_vector<pdsch_proc_args, MAX_PDSCH_PDUS_PER_SLOT>                                        pdsch_list;
  static_vector<ssb_processor::pdu_t, MAX_SSB_PER_SLOT>                                          ssb_list;
  static_vector<nzp_csi_rs_generator::config_t, MAX_CSI_RS_PDUS_PER_SLOT>                        nzp_csi_rs_list;
  static_vector<prs_generator_configuration, MAX_PRS_PDUS_PER_SLOT>                              prs_list;
  /// @}
  /// \defgroup phy_chan_processors
  /// \brief Physical channel processors.
  /// @{
  std::unique_ptr<pdcch_processor>      pdcch_proc;
  std::unique_ptr<pdsch_processor>      pdsch_proc;
  std::unique_ptr<ssb_processor>        ssb_proc;
  std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc;
  std::unique_ptr<prs_generator>        prs_gen;
  /// @}
  /// \defgroup phy_chan_executors
  /// \brief Physical channel executors.
  /// @{
  downlink_task_executor pdcch_executor;
  downlink_task_executor pdsch_executor;
  downlink_task_executor ssb_executor;
  downlink_task_executor csi_rs_executor;
  downlink_task_executor prs_executor;
  /// @}
  /// Logger.
  srslog::basic_logger& logger;
  /// DL processor internal state.
  downlink_processor_multi_executor_state state;
  /// PDSCH notifier wrapper.
  pdsch_processor_notifier_wrapper pdsch_notifier;
};
} // namespace srsran
