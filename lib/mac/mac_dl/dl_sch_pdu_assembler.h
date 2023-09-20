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

#include "mac_dl_ue_manager.h"
#include "srsran/mac/lcid_dl_sch.h"
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

class byte_buffer_chain;

/// \brief This class represents and encodes a MAC DL-SCH PDU that may contain multiple subPDUs.
/// Each subPDU is composed of a MAC subheader and MAC CE or MAC SDU payload.
class dl_sch_pdu
{
public:
  explicit dl_sch_pdu(span<uint8_t> pdu_buffer_) : pdu(pdu_buffer_) {}

  /// Adds a MAC SDU as a subPDU.
  unsigned add_sdu(lcid_t lcid_, byte_buffer_chain&& sdu);

  /// Adds a UE Contention Resolution CE as a subPDU.
  void add_ue_con_res_id(const ue_con_res_id_t& con_res_payload);

  /// Adds a Timing Advance Command CE as a subPDU.
  void add_tag_cmd(const ta_cmd_ce_payload& ce_payload);

  /// Adds a padding CE as a subPDU.
  void add_padding(unsigned len);

  /// Number of bytes of the MAC PDU.
  unsigned nof_bytes() const { return byte_offset; }

  /// Remaining space in number of bytes in the PDU.
  unsigned nof_empty_bytes() const { return pdu.size() - byte_offset; }

  /// Gets the held MAC PDU bytes.
  span<uint8_t> get() { return pdu.first(byte_offset); }

private:
  void encode_subheader(bool F_bit, lcid_dl_sch_t lcid, unsigned header_len, unsigned payload_len);

  span<uint8_t> pdu;
  unsigned      byte_offset = 0;
};

/// \brief Class that manages the encoding of DL-SCH MAC PDUs that will be stored in Transport Blocks.
class dl_sch_pdu_assembler
{
public:
  explicit dl_sch_pdu_assembler(mac_dl_ue_manager& ue_mng_);

  /// \brief Encodes a MAC DL-SCH PDU with the provided scheduler information.
  /// \param rnti RNTI for which the MAC PDU was allocated.
  /// \param h_id HARQ-Id of the HARQ process used for this PDU transmission.
  /// \param tb_idx Transport block index of the HARQ process used for this PDU transmission.
  /// \param tb_info The information relative to the transport block allocated by the scheduler. This class contains
  /// a list of LCIDs of the subPDUs to allocated together with how many bytes each subPDU should take.
  /// \param tb_size_bytes Number of bytes allocated for the Transport Block.
  /// \return Byte container with assembled PDU. This container length should be lower or equal to \c tb_size_bytes.
  span<const uint8_t> assemble_newtx_pdu(rnti_t                rnti,
                                         harq_id_t             h_id,
                                         unsigned              tb_idx,
                                         const dl_msg_tb_info& tb_info,
                                         unsigned              tb_size_bytes);

  /// \brief Fetches and assembles MAC DL-SCH PDU that corresponds to a HARQ retransmission.
  /// \param rnti RNTI for which the MAC PDU was allocated.
  /// \param h_id HARQ-Id of the HARQ process used for this PDU transmission.
  /// \param tb_idx Transport block index of the HARQ process used for this PDU transmission.
  /// \param tb_size_bytes Number of bytes allocated for the Transport Block.
  /// \return Byte container with assembled PDU.
  span<const uint8_t> assemble_retx_pdu(rnti_t rnti, harq_id_t harq_id, unsigned tb_idx, unsigned tbs_bytes);

private:
  class dl_sch_pdu_logger;

  /// Assemble MAC SDUs for a given LCID.
  void assemble_sdus(dl_sch_pdu& ue_pdu, rnti_t rnti, const dl_msg_lc_info& subpdu, dl_sch_pdu_logger& pdu_logger);

  /// Assemble MAC subPDU with a CE.
  void assemble_ce(dl_sch_pdu& ue_pdu, rnti_t rnti, const dl_msg_lc_info& subpdu, dl_sch_pdu_logger& pdu_logger);

  mac_dl_ue_manager&    ue_mng;
  srslog::basic_logger& logger;
};

} // namespace srsran
