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

#include "mac_dl_ue_manager.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/mac/lcid_dl_sch.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/memory_pool/ring_buffer_pool.h"

namespace srsgnb {

/// \brief This class represents and encodes a MAC DL-SCH PDU that may contain multiple subPDUs.
/// Each subPDU is composed of a MAC subheader and MAC CE or MAC SDU payload.
class dl_sch_pdu
{
  // SDUs up to 256 B can use the short 8-bit L field
  constexpr static size_t MAC_SUBHEADER_LEN_THRESHOLD = 256;

public:
  /// Maximum size for a MAC PDU (implementation-defined).
  static constexpr size_t MAX_PDU_LENGTH = 32768;

  explicit dl_sch_pdu(span<uint8_t> pdu_buffer_) : pdu(pdu_buffer_) {}

  /// Adds a MAC SDU as a subPDU.
  void add_sdu(lcid_t lcid_, byte_buffer_slice_chain&& sdu)
  {
    lcid_dl_sch_t lcid          = lcid_;
    unsigned      header_length = 2;
    bool          F_bit         = false;
    if (sdu.length() >= MAC_SUBHEADER_LEN_THRESHOLD) {
      F_bit = true;
      header_length += 1;
    }

    // Encode Header.
    size_t sdu_len = sdu.length();
    encode_subheader(F_bit, lcid, header_length, sdu_len);

    // Encode Payload.
    std::copy(sdu.begin(), sdu.end(), pdu.data() + byte_offset);
    byte_offset += sdu_len;
  }

  /// Adds a UE Contention Resolution CE as a subPDU.
  void add_ue_con_res_id(const ue_con_res_id_t& con_res_payload)
  {
    lcid_dl_sch_t lcid        = lcid_dl_sch_t::UE_CON_RES_ID;
    unsigned      header_len  = 1;
    unsigned      payload_len = lcid.sizeof_ce();

    // Encode header and payload.
    encode_subheader(false, lcid_dl_sch_t::UE_CON_RES_ID, header_len, payload_len);

    // Encode UE ConRes ID.
    std::copy(con_res_payload.begin(), con_res_payload.end(), pdu.data() + byte_offset);
    byte_offset += con_res_payload.size();
  }

  /// Adds a padding CE as a subPDU.
  void add_padding(unsigned len)
  {
    // 1 Byte R/LCID MAC subheader.
    unsigned header_len = 1;
    unsigned sdu_len    = len - 1;
    encode_subheader(false, lcid_dl_sch_t::PADDING, header_len, sdu_len);

    // Padding bits.
    std::fill(pdu.data() + byte_offset, pdu.data() + byte_offset + sdu_len, 0);
    byte_offset += sdu_len;
  }

  /// Number of bytes of the MAC PDU.
  size_t nof_bytes() const { return byte_offset; }

  /// Gets the held MAC PDU bytes.
  span<uint8_t> get() { return pdu.first(byte_offset); }

private:
  void encode_subheader(bool F_bit, lcid_dl_sch_t lcid, unsigned header_len, unsigned payload_len)
  {
    pdu[byte_offset++] = ((F_bit ? 1U : 0U) << 6U) | (lcid.value() & 0x3fU);
    if (header_len == 3) {
      // 3 Byte R/F/LCID/L MAC subheader with 16-bit L field
      pdu[byte_offset++] = (payload_len & 0xff00U) >> 8U;
      pdu[byte_offset++] = (payload_len & 0xffU);
    } else if (header_len == 2) {
      // 2 Byte R/F/LCID/L MAC subheader with 8-bit L field
      pdu[byte_offset++] = payload_len & 0xffU;
    } else if (header_len == 1) {
      // do nothing
    } else {
      report_fatal_error("Error while packing PDU. Unsupported header length ({})", header_len);
    }
  }

  span<uint8_t> pdu;
  unsigned      byte_offset = 0;
};

/// \brief Class that manages the encoding of DL-SCH MAC PDUs that will be stored in Transport Blocks.
class dl_sch_pdu_assembler
{
public:
  explicit dl_sch_pdu_assembler(mac_dl_ue_manager& ue_mng_, ticking_ring_buffer_pool& pool_) :
    ue_mng(ue_mng_), pdu_pool(pool_), logger(srslog::fetch_basic_logger("MAC"))
  {
  }

  /// \brief Encodes a MAC DL-SCH PDU with the provided scheduler information.
  /// \param rnti RNTI for which the MAC PDU was allocated.
  /// \param tb_info The information relative to the transport block allocated by the scheduler. This class contains
  /// a list of LCIDs of the subPDUs to allocated together with how many bytes each subPDU should take.
  /// \param tb_size_bytes Number of bytes allocated for the Transport Block.
  /// \return Byte container with assembled PDU. This container length should be lower or equal to \c tb_size_bytes.
  span<const uint8_t> assemble_pdu(rnti_t rnti, const dl_msg_tb_info& tb_info, unsigned tb_size_bytes)
  {
    span<uint8_t> pdu_bytes = pdu_pool.allocate_buffer(tb_size_bytes);
    if (pdu_bytes.empty()) {
      logger.warning("SCHED: Unable to allocate DL MAC PDU for rnti={:#x}.", rnti);
      return {};
    }
    dl_sch_pdu ue_pdu(pdu_bytes);

    // Encode added subPDUs.
    for (const dl_msg_lc_info& subpdu : tb_info.subpdus) {
      if (subpdu.lcid.is_sdu()) {
        assemble_sdu(ue_pdu, rnti, subpdu);
      } else {
        assemble_ce(ue_pdu, rnti, subpdu);
      }
    }

    // Add Padding if required.
    unsigned current_size = ue_pdu.nof_bytes();
    if (current_size < tb_size_bytes) {
      ue_pdu.add_padding(tb_size_bytes - current_size);
    } else if (current_size > tb_size_bytes) {
      logger.error("ERROR: Allocated subPDUs exceed TB size ({} > {})", current_size, tb_size_bytes);
      return {};
    }

    return ue_pdu.get();
  }

private:
  /// Assemble MAC subPDU with an SDU.
  void assemble_sdu(dl_sch_pdu& ue_pdu, rnti_t rnti, const dl_msg_lc_info& subpdu)
  {
    static const unsigned MIN_MAC_SDU_SIZE = 3;

    // Fetch RLC Bearer.
    mac_sdu_tx_builder* bearer = ue_mng.get_bearer(rnti, subpdu.lcid.to_lcid());
    srsgnb_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

    unsigned rem_bytes = subpdu.sched_bytes;
    while (rem_bytes > MIN_MAC_SDU_SIZE) {
      // Fetch MAC Tx SDU.
      byte_buffer_slice_chain sdu = bearer->on_new_tx_sdu(subpdu.sched_bytes);
      if (sdu.empty()) {
        // TODO: Handle.
        logger.warning("Failed to encode MAC SDU with LCID={}", subpdu.lcid);
        break;
      }

      rem_bytes -= sdu.length();

      // Add SDU as a subPDU.
      ue_pdu.add_sdu(subpdu.lcid.to_lcid(), std::move(sdu));
    }
    if (rem_bytes == subpdu.sched_bytes) {
      logger.error("ERROR: Skipping MAC subPDU encoding. Cause: Allocated SDU size={} is too small to fit MAC "
                   "subheader and payload.",
                   subpdu.sched_bytes);
    }
  }

  /// Assemble MAC subPDU with a CE.
  void assemble_ce(dl_sch_pdu& ue_pdu, rnti_t rnti, const dl_msg_lc_info& subpdu)
  {
    switch (subpdu.lcid.value()) {
      case lcid_dl_sch_t::UE_CON_RES_ID: {
        ue_con_res_id_t conres = ue_mng.get_con_res_id(rnti);
        std::copy(conres.begin(), conres.end(), conres.begin());
        ue_pdu.add_ue_con_res_id(conres);
      } break;
      default:
        report_fatal_error("Invalid MAC CE LCID={}", subpdu.lcid);
    }
  }

  mac_dl_ue_manager&        ue_mng;
  ticking_ring_buffer_pool& pdu_pool;
  srslog::basic_logger&     logger;
};

} // namespace srsgnb
