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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"

namespace srsgnb {

namespace srs_cu_cp {

/// \brief Fills ASN.1 RRC Setup struct.
/// \param[out] rrc_setup The RRC Setup ASN.1 struct to fill.
/// \param[in] init_ul_rrc_transfer_msg The Init_UL_RRC_Transfer message received by the CU.
inline void
fill_asn1_rrc_setup_msg(asn1::rrc_nr::rrc_setup_s& rrc_setup, const byte_buffer& mcg, uint8_t rrc_transaction_id)
{
  using namespace asn1::rrc_nr;
  rrc_setup_ies_s& setup_ies   = rrc_setup.crit_exts.set_rrc_setup();
  rrc_setup.rrc_transaction_id = rrc_transaction_id;

  // Add SRB1
  // Note: See TS 38.331 section 5.3.5.6.3 - SRB addition/modification
  auto& radio_bearer_cfg = setup_ies.radio_bearer_cfg;
  radio_bearer_cfg.srb_to_add_mod_list.push_back(srb_to_add_mod_s{});
  radio_bearer_cfg.srb_to_add_mod_list.back().srb_id = 1; // SRB1

  // Copy cell config from DU_to_CU_RRC_Container to master cell group
  auto& master_cell_group = setup_ies.master_cell_group;
  master_cell_group.resize(mcg.length());
  std::copy(mcg.begin(), mcg.end(), master_cell_group.begin());
}

/// Extracts transaction id of RRC Setup complete message.
inline expected<uint8_t> get_transaction_id(const asn1::rrc_nr::rrc_setup_complete_s& msg)
{
  return msg.rrc_transaction_id;
}

/// Extracts transaction id of UL-DCCH message.
inline expected<uint8_t> get_transaction_id(const asn1::rrc_nr::ul_dcch_msg_s& msg)
{
  using namespace asn1::rrc_nr;
  switch (msg.msg.c1().type().value) {
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_setup_complete:
      return msg.msg.c1().rrc_setup_complete().rrc_transaction_id;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

} // namespace srs_cu_cp

} // namespace srsgnb
