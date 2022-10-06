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

namespace srsgnb {

class dl_sch_pdu
{
  // SDUs up to 256 B can use the short 8-bit L field
  constexpr static size_t MAC_SUBHEADER_LEN_THRESHOLD = 256;

public:
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
    byte_buffer subpdu;
    encode_subheader(subpdu, F_bit, lcid, header_length, sdu.length());
    pdu.push_back(std::move(subpdu));

    // Encode SDU.
    pdu.chain_after(std::move(sdu));
  }

  /// Adds a UE Contention Resolution CE as a subPDU.
  void add_ue_con_res_id(const ue_con_res_id_t& con_res_payload)
  {
    lcid_dl_sch_t lcid        = lcid_dl_sch_t::UE_CON_RES_ID;
    unsigned      header_len  = 1;
    unsigned      payload_len = lcid.sizeof_ce();

    // Encode header and payload.
    byte_buffer subpdu;
    encode_subheader(subpdu, false, lcid_dl_sch_t::UE_CON_RES_ID, header_len, payload_len);
    subpdu.append(con_res_payload);
    pdu.push_back(std::move(subpdu));
  }

  void add_padding(unsigned len)
  {
    // 1 Byte R/LCID MAC subheader.
    unsigned    header_len = 1;
    unsigned    sdu_len    = len - 1;
    byte_buffer subpdu;
    encode_subheader(subpdu, false, lcid_dl_sch_t::PADDING, header_len, sdu_len);
    for (unsigned i = 0; i != sdu_len; ++i) {
      subpdu.append(0);
    }
    pdu.push_back(std::move(subpdu));
  }

  size_t nof_bytes() const { return pdu.length(); }

  byte_buffer_slice_chain pop() { return std::move(pdu); }

private:
  static void
  encode_subheader(byte_buffer& subpdu, bool F_bit, lcid_dl_sch_t lcid, unsigned header_len, unsigned payload_len)
  {
    subpdu.append(((F_bit ? 1U : 0U) << 6U) | (lcid.value() & 0x3FU));
    if (header_len == 3) {
      // 3 Byte R/F/LCID/L MAC subheader with 16-bit L field
      subpdu.append((payload_len & 0xFF00U) >> 8U);
      subpdu.append((payload_len & 0xFFU));
    } else if (header_len == 2) {
      // 2 Byte R/F/LCID/L MAC subheader with 8-bit L field
      subpdu.append((payload_len & 0xFFU));
    } else if (header_len == 1) {
      // do nothing
    } else {
      report_fatal_error("Error while packing PDU. Unsupported header length ({})", header_len);
    }
  }

  byte_buffer_slice_chain pdu;
};

/// Class that manages the encoding of DL-SCH MAC PDUs to be emplaced in Transport Blocks.
class dl_sch_pdu_assembler
{
public:
  dl_sch_pdu_assembler(mac_dl_ue_manager& ue_mng_) : ue_mng(ue_mng_), logger(srslog::fetch_basic_logger("MAC")) {}

  byte_buffer_slice_chain assemble_pdu(rnti_t rnti, const dl_msg_tb_info& tb_info, unsigned tb_size_bytes)
  {
    dl_sch_pdu ue_pdu;
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
    }

    return ue_pdu.pop();
  }

private:
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
  }

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

  mac_dl_ue_manager&    ue_mng;
  srslog::basic_logger& logger;
};

} // namespace srsgnb
