/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "mac_scheduler_ce_info_handler.h"
#include "mac_ul_sch_pdu.h"
#include "mac_ul_ue_manager.h"
#include "ul_bsr.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/du_high/rnti_value_table.h"
#include "srsran/mac/mac.h"
#include "srsran/pcap/mac_pcap.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Stores MAC RX PDU, as well as any contextual or temporary information related to the PDU decoding.
struct decoded_mac_rx_pdu {
  decoded_mac_rx_pdu() = default;
  decoded_mac_rx_pdu(slot_point      slot_rx_,
                     du_cell_index_t cell_idx_,
                     mac_rx_pdu      pdu_rx_,
                     du_ue_index_t   ue_index_ = MAX_NOF_DU_UES) :
    slot_rx(slot_rx_), cell_index_rx(cell_idx_), ue_index(ue_index_), pdu_rx(std::move(pdu_rx_))
  {
    srsran_sanity_check(not pdu_rx.pdu.empty(), "Received empty PDU");
  }

  /// Clear PDU context.
  void clear()
  {
    slot_rx       = {};
    cell_index_rx = MAX_NOF_DU_CELLS;
    ue_index      = MAX_NOF_DU_UES;
    pdu_rx.pdu.clear();
    decoded_subpdus.clear();
  }

  /// Slot when PDU was received in the PHY.
  slot_point slot_rx;

  /// Cell where PDU was decoded by the PHY.
  du_cell_index_t cell_index_rx;

  /// UE index for which PDU is directed. ue_index = MAX_NOF_UES if no UE with provided RNTI exists.
  du_ue_index_t ue_index;

  /// Received MAC PDU content.
  /// Note: C-RNTI may be later altered, depending on whether a CRNTI MAC CE is present.
  mac_rx_pdu pdu_rx;

  /// View of decoded subPDUs of the MAC PDU.
  mac_ul_sch_pdu decoded_subpdus;
};

/// Decodes MAC Rx PDUs and handles the respective CEs and SDUs by dispatching them to the Scheduler or UE UL bearers.
/// This class is thread-safe, i.e. it can be concurrently called for multiple MAC UL PDUs, iff its methods are called
/// from within the respective MAC PDU RNTI executor.
class pdu_rx_handler
{
public:
  pdu_rx_handler(mac_ul_ccch_notifier&          ccch_notifier_,
                 du_high_ue_executor_mapper&    ue_exec_mapper_,
                 mac_scheduler_ce_info_handler& sched_,
                 mac_ul_ue_manager&             ue_manager_,
                 du_rnti_table&                 rnti_table_,
                 mac_pcap&                      pcap_);

  /// Decode MAC Rx PDU, log contents and handle subPDUs.
  /// \param sl_rx Slot when MAC UL PDU was received.
  /// \param cell_index Cell index where MAC UL PDU was received.
  /// \param pdu MAC UL PDU received.
  /// \return true if all subPDUs were correctly handled.
  bool handle_rx_pdu(slot_point sl_rx, du_cell_index_t cell_index, mac_rx_pdu pdu);

  /// Called to push stored UL CCCH to upper layers, once the UE has been created in the DU manager.
  bool push_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer ul_ccch_msg);

private:
  /// Handle subPDUs contained in a MAC UL PDU.
  /// \param pdu MAC UL PDU being processed.
  /// \return true if all subPDUs were correctly handled.
  bool handle_rx_subpdus(const decoded_mac_rx_pdu& pdu);

  /// Handle UL SDU targetted at LCIDs >= 1.
  /// \param pdu MAC UL PDU being processed.
  /// \param subpdu subPDU of PDU where CRNTI CE was detected.
  /// \param ue MAC UL UE object for which the sdu is directed.
  /// \return true if correctly handled.
  bool handle_sdu(const decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& sdu, mac_ul_ue_context* ue);

  /// Handle UL MAC CE.
  /// \param pdu MAC UL PDU being processed.
  /// \param subpdu subPDU of PDU where CRNTI CE was detected.
  /// \return true if correctly handled.
  bool handle_mac_ce(const decoded_mac_rx_pdu& pdu, const mac_ul_sch_subpdu& subpdu);

  /// Handle UL CCCH Message
  /// \param pdu MAC UL PDU being processed.
  /// \param subpdu subPDU of PDU where CRNTI CE was detected.
  /// \return true if correctly handled.
  bool handle_ccch_msg(const decoded_mac_rx_pdu& pdu, const mac_ul_sch_subpdu& sdu);

  /// Handle CRNTI MAC CE
  /// \remark see TS 38.321, 6.1.3.2 - C-RNTI MAC CE
  /// \param pdu MAC UL PDU being processed.
  /// \param subpdu subPDU of PDU where CRNTI CE was detected.
  /// \return true if correctly handled.
  bool handle_crnti_ce(const decoded_mac_rx_pdu& pdu, const mac_ul_sch_subpdu& subpdu);

  /// Handle PDU to PCAP file
  void write_pcap_rx_pdu(const slot_point& sl_rx, const mac_rx_pdu& pdu);

  mac_ul_ccch_notifier&          ccch_notifier;
  du_high_ue_executor_mapper&    ue_exec_mapper;
  srslog::basic_logger&          logger;
  mac_scheduler_ce_info_handler& sched;
  mac_ul_ue_manager&             ue_manager;
  du_rnti_table&                 rnti_table;
  mac_pcap&                      pcap;
};

} // namespace srsran
