/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cell_sys_info_configurator.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/mac/cell_configuration.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Entity responsible for fetching encoded SIB1 and SI messages based on scheduled SI grants.
class sib_pdu_assembler : public cell_sys_info_configurator
{
public:
  class message_handler
  {
  public:
    virtual ~message_handler() = default;

    virtual si_version_type update(si_version_type si_version, const byte_buffer& pdu) = 0;

    /// Retrieve encoded SI bytes for a given SI scheduling opportunity.
    virtual span<const uint8_t> get_pdu(slot_point sl_tx, const sib_information& si_info) = 0;
  };

  sib_pdu_assembler(const mac_cell_sys_info_config& req);

  /// Update the SIB1 and SI messages.
  si_change_result handle_si_change_request(const mac_cell_sys_info_config& req) override;

  /// \brief Retrieve the encoded SI message.
  span<const uint8_t> encode_si_pdu(slot_point sl_tx, const sib_information& si_info);

private:
  using bcch_dl_sch_buffer = std::shared_ptr<const std::vector<uint8_t>>;

  /// A snapshot of a SIB1 and SI messages within a given SI change window.
  struct si_buffer_snapshot {
    unsigned                                                 version;
    units::bytes                                             sib1_len;
    bcch_dl_sch_buffer                                       sib1_buffer;
    std::vector<std::pair<units::bytes, bcch_dl_sch_buffer>> si_msg_buffers;
  };

  void save_buffers(const mac_cell_sys_info_config& req);

  srslog::basic_logger& logger;

  /// Last SI messages received by the assembler.
  mac_cell_sys_info_config last_si_cfg;

  /// SI buffers of last SI message update.
  si_buffer_snapshot last_cfg_buffers;

  /// Buffers being transferred from configuration plane to assembler RT path.
  lockfree_triple_buffer<si_buffer_snapshot> pending;

  /// SI buffers that are being currently encoded and sent to lower layers.
  si_buffer_snapshot current_buffers;
};

} // namespace srsran
