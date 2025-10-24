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

#include "cell_dl_harq_buffer_pool.h"
#include "dl_sch_pdu_assembler.h"
#include "mac_cell_time_mapper_impl.h"
#include "mac_dl_metric_handler.h"
#include "mac_dl_ue_repository.h"
#include "mac_scheduler_cell_info_handler.h"
#include "paging_pdu_assembler.h"
#include "rar_pdu_assembler.h"
#include "sib_pdu_assembler.h"
#include "ssb_assembler.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/memory_pool/ring_buffer_pool.h"

namespace srsran {

class timer_manager;

class mac_cell_processor final : public mac_cell_slot_handler, public mac_cell_controller
{
public:
  mac_cell_processor(const mac_cell_creation_request& cell_cfg_req,
                     mac_scheduler_cell_info_handler& sched,
                     du_rnti_table&                   rnti_table,
                     mac_cell_result_notifier&        phy_notifier,
                     task_executor&                   cell_exec,
                     task_executor&                   slot_exec,
                     task_executor&                   ctrl_exec,
                     mac_pcap&                        pcap,
                     timer_manager&                   timers,
                     mac_cell_config_dependencies     dependencies);

  /// Starts configured cell.
  async_task<void> start() override;

  /// Stops configured cell.
  async_task<void> stop() override;

  async_task<mac_cell_reconfig_response> reconfigure(const mac_cell_reconfig_request& request) override;

  mac_cell_time_mapper_impl& get_time_mapper() { return slot_time_mapper; }

  void handle_slot_indication(const mac_cell_timing_context& context) noexcept override;
  void handle_error_indication(slot_point sl_tx, error_event event) noexcept override;
  void handle_stop_indication() noexcept override;

  /// Creates new UE DL context, updates logical channel MUX, adds UE in scheduler.
  async_task<bool> add_ue(const mac_ue_create_request& request);

  /// Deletes UE context in MAC MUX.
  async_task<void> remove_ue(const mac_ue_delete_request& request);

  /// Add/Modify UE bearers in the MUX.
  async_task<bool> addmod_bearers(du_ue_index_t ue_index, span<const mac_logical_channel_config> logical_channels);

  /// Remove UE bearers in the MUX.
  async_task<bool> remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem);

private:
  void handle_slot_indication_impl(slot_point sl_tx, metric_clock::time_point enqueue_slot_tp) noexcept;

  /// Assemble struct that is going to be passed down to PHY with the DL scheduling result.
  /// \remark FAPI will use this struct to generate a DL_TTI.Request.
  void assemble_dl_sched_request(mac_dl_sched_result&   mac_res,
                                 slot_point             sl_tx,
                                 du_cell_index_t        cell_index,
                                 const dl_sched_result& dl_res);

  /// Assemble struct with the MAC PDUs of a given slot and cell that is going be passed to the PHY.
  /// \remark FAPI will use this struct to generate a Tx_Data.Request.
  void assemble_dl_data_request(mac_dl_data_result&    mac_data,
                                slot_point             sl_tx,
                                du_cell_index_t        cell_index,
                                const dl_sched_result& dl_res);

  /// Update DL buffer states of the allocated DL bearers.
  void update_logical_channel_dl_buffer_states(const dl_sched_result& dl_res);

  void write_tx_pdu_pcap(slot_point sl_tx, const sched_result& sl_res, const mac_dl_data_result& dl_res);

  // Dependencies.
  srslog::basic_logger&           logger;
  const mac_cell_creation_request cell_cfg;
  task_executor&                  cell_exec;
  task_executor&                  slot_exec;
  task_executor&                  ctrl_exec;
  mac_cell_result_notifier&       phy_cell;
  timer_manager&                  timers;

  // Mapper of upper-layer bearers to MAC logical channels in the DL direction.
  mac_dl_ue_repository ue_mng;

  // Pool of DL HARQ buffers used for UE PDSCH.
  cell_dl_harq_buffer_pool dl_harq_buffers;

  ticking_ring_buffer_pool pdu_pool;

  /// ssb_helper: contains the SSB-specific parameters that are derived from those passed by the DU interface. These
  /// parameters are passed to the scheduler and also to the PHY to generate the SSB PDU and PBCH payload.
  ssb_assembler ssb_helper;

  sib_pdu_assembler    sib_assembler;
  rar_pdu_assembler    rar_assembler;
  dl_sch_pdu_assembler dlsch_assembler;
  paging_pdu_assembler paging_assembler;

  mac_scheduler_cell_info_handler& sched;

  /// Ticks the APP clock based on the received slot indications for this cell.
  std::unique_ptr<mac_cell_clock_controller> time_source;

  // Handler of cell metrics
  mac_dl_cell_metric_handler metrics;

  // Represents cell activation state.
  enum class cell_state { inactive, active } state = cell_state::inactive;
  manual_event_flag stop_completed;

  mac_pcap& pcap;

  mac_cell_time_mapper_impl slot_time_mapper;

  unsigned                              sib1_pcap_dumped_version;
  std::array<unsigned, MAX_SI_MESSAGES> si_pcap_dumped_version;
};

} // namespace srsran
