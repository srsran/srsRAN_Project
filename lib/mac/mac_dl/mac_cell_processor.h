/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../mac_config.h"
#include "dl_sch_pdu_assembler.h"
#include "mac_dl_ue_manager.h"
#include "rar_pdu_assembler.h"
#include "sib_pdu_assembler.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/scheduler/mac_scheduler.h"
#include "srsgnb/support/memory_pool/ring_buffer_pool.h"
#include "ssb_assembler.h"

namespace srsgnb {

class mac_cell_processor final : public mac_cell_slot_handler,
                                 public mac_cell_controller,
                                 public mac_cell_control_information_handler
{
public:
  mac_cell_processor(mac_common_config_t&             cfg_,
                     const mac_cell_creation_request& cell_cfg_req_,
                     mac_scheduler&                   sched_,
                     mac_dl_ue_manager&               ue_mng);

  /// Starts configured cell.
  async_task<void> start() override;

  /// Stops configured cell.
  async_task<void> stop() override;

  void handle_slot_indication(slot_point sl_tx) override;

  void handle_crc(const mac_crc_indication_message& msg) override;

  void handle_uci(const mac_uci_indication_message& msg) override;

private:
  void handle_slot_indication_impl(slot_point sl_tx);

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

  mac_common_config_t&            cfg;
  srslog::basic_logger&           logger;
  const mac_cell_creation_request cell_cfg;
  task_executor&                  cell_exec;
  mac_cell_result_notifier&       phy_cell;

  ticking_ring_buffer_pool pdu_pool;

  /// ssb_helper: contains the SSB-specific parameters that are derived from those passed by the DU interface. These
  /// parameters are passed to the scheduler and also also to the PHY to generate the SSB PDU and PBCH payload.
  ssb_assembler ssb_helper;

  sib_pdu_assembler    sib_assembler;
  rar_pdu_assembler    rar_assembler;
  dl_sch_pdu_assembler dlsch_assembler;

  mac_scheduler&     sched_obj;
  mac_dl_ue_manager& ue_mng;

  /// Represents activation cell state.
  // Note: For now, cells start active.
  enum class cell_state { inactive, active } state = cell_state::active;
};

} // namespace srsgnb
