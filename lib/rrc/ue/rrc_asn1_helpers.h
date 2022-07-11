/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RRC_UE_RRC_ASN1_HELPERS_H
#define SRSGNB_LIB_RRC_UE_RRC_ASN1_HELPERS_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"

namespace srsgnb {

namespace srs_cu_cp {

/// \brief Fills ASN.1 RRC Setup struct.
/// \param[out] rrc_setup The RRC Setup ASN.1 struct to fill.
/// \param[in] init_ul_rrc_transfer_msg The Init_UL_RRC_Transfer message received by the CU.
void fill_asn1_rrc_setup_msg(asn1::rrc_nr::rrc_setup_s& rrc_setup, const byte_buffer& mcg)
{
  asn1::rrc_nr::rrc_setup_ies_s& setup_ies = rrc_setup.crit_exts.set_rrc_setup();

  // Add SRB1
  // Note: See TS 38.331 section 5.3.5.6.3 - SRB addition/modification
  auto& radio_bearer_cfg = setup_ies.radio_bearer_cfg;
  radio_bearer_cfg.srb_to_add_mod_list.push_back(asn1::rrc_nr::srb_to_add_mod_s{});
  radio_bearer_cfg.srb_to_add_mod_list.back().srb_id = 1; // SRB1

  // Copy cell config from DU_to_CU_RRC_Container to master cell group
  auto& master_cell_group = setup_ies.master_cell_group;
  master_cell_group.resize(mcg.length());
  std::copy(mcg.begin(), mcg.end(), master_cell_group.begin());
}

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_LIB_RRC_UE_RRC_ASN1_HELPERS_H
