/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "paging_pdu_assembler.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"

using namespace srsran;

/// \brief Build RRC-NR Paging message from the Paging grant.
/// \param[in] pg Paging grant information.
/// \return RRC-NR Paging message.
static asn1::rrc_nr::paging_s make_asn1_rrc_cell_paging_msg(const dl_paging_allocation& pg)
{
  asn1::rrc_nr::paging_s rrc_pg{};
  for (const paging_ue_info& info : pg.paging_ue_list) {
    asn1::rrc_nr::paging_record_s pg_rec{};
    if (info.paging_type_indicator == paging_ue_info::cn_ue_paging_identity) {
      auto& rrc_pg_id = pg_rec.ue_id.set_ng_5_g_s_tmsi();
      rrc_pg_id       = rrc_pg_id.from_number(info.paging_identity);
    } else {
      auto& rrc_pg_id = pg_rec.ue_id.set_full_i_rnti();
      rrc_pg_id       = rrc_pg_id.from_number(info.paging_identity);
    }
    rrc_pg.paging_record_list.push_back(pg_rec);
  }

  return rrc_pg;
}

/// \brief Derive packed cell PCCH-PCH Paging message.
/// \param[in] pg Paging grant information.
/// \return byte buffer with packed cell PCCH-PCH message.
static byte_buffer make_asn1_rrc_cell_pcch_pch_msg(const dl_paging_allocation& pg)
{
  byte_buffer   buf;
  asn1::bit_ref bref{buf};

  asn1::rrc_nr::pcch_msg_s pcch_msg{};
  auto&                    pcch_c1 = pcch_msg.msg.set_c1();
  pcch_c1.set_paging()             = make_asn1_rrc_cell_paging_msg(pg);

  const asn1::SRSASN_CODE ret = pcch_msg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack PCCH-PCH Paging message");
  return buf;
}

span<const uint8_t> paging_pdu_assembler::encode_paging_pdu(const dl_paging_allocation& pg)
{
  byte_buffer payload = make_asn1_rrc_cell_pcch_pch_msg(pg);
  srsran_assert(pg.pdsch_cfg.codewords[0].tb_size_bytes >= payload.length(),
                "The TBS for Paging cannot be smaller than the Paging payload");

  std::vector<uint8_t> pcch_payload(pg.pdsch_cfg.codewords[0].tb_size_bytes);
  std::fill(pcch_payload.begin(), pcch_payload.end(), 0);
  pcch_payload.assign(payload.begin(), payload.end());

  return pcch_payload;
}
