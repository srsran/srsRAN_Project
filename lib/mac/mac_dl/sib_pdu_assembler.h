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

#include "segmented_sib_list.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/mac/cell_configuration.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Entity responsible for fetching encoded SIB1 and SI messages based on scheduled SI grants.
class sib_pdu_assembler
{
public:
  class message_handler
  {
  public:
    virtual ~message_handler() = default;

    virtual si_version_type update(si_version_type si_version, const byte_buffer& pdu) = 0;

    /// \brief Enqueue encodes SI messages at proper Tx slots.
    virtual bool enqueue_si_pdu_updates(const mac_cell_sys_info_pdu_update& pdu_update_req) = 0;

    /// Retrieve encoded SI bytes for a given SI scheduling opportunity.
    virtual span<const uint8_t> get_pdu(slot_point sl_tx, const sib_information& si_info) = 0;
  };

  sib_pdu_assembler(const mac_cell_sys_info_config& req);

  /// Update the SIB1 and SI messages.
  void handle_si_change_request(const mac_cell_sys_info_config& req);

  /// \brief Retrieve the encoded SI message.
  span<const uint8_t> encode_si_pdu(slot_point sl_tx, const sib_information& si_info);

  /// \brief Enqueue encodes SI messages at proper Tx slots.
  bool enqueue_si_message_pdu_updates(const mac_cell_sys_info_pdu_update& pdu_update_req);

private:
  using bcch_dl_sch_buffer = std::shared_ptr<const std::vector<uint8_t>>;

  /// Variant that can either hold a single BCCH payload, or multiple versions of such payload for segmented messages.
  using bcch_payload_type = std::variant<bcch_dl_sch_buffer, segmented_sib_list<bcch_dl_sch_buffer>>;

  /// A snapshot of a SIB1 and SI messages within a given SI change window.
  struct si_buffer_snapshot {
    unsigned                                                version;
    units::bytes                                            sib1_len;
    bcch_dl_sch_buffer                                      sib1_buffer;
    std::vector<std::pair<units::bytes, bcch_payload_type>> si_msg_buffers;
  };

  void save_buffers(si_version_type si_version, const mac_cell_sys_info_config& req);

  srslog::basic_logger& logger;

  // Last SI messages received by the assembler.
  mac_cell_sys_info_config last_si_cfg;

  // SI buffers of last SI message update request.
  // Note: This member is only accessed from the control executor.
  si_buffer_snapshot last_cfg_buffers;

  // Buffers being transferred from configuration plane to assembler RT path.
  lockfree_triple_buffer<si_buffer_snapshot> pending;

  // SI buffers that are being currently encoded and sent to lower layers.
  // Note: This member is only accessed from the RT path.
  si_buffer_snapshot current_buffers;

  std::unique_ptr<message_handler> message_ext_handler;
};

/// \brief Instantiates an SI message extension handler.
/// \param[in] req    Request containing System Information signalled by the cell.
/// \return A pointer to the SI message extension handler on success, otherwise \c nullptr.
std::unique_ptr<sib_pdu_assembler::message_handler>
create_si_message_extension_handler(const mac_cell_sys_info_config& req);

} // namespace srsran
