/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_F1C_ASN1_HELPERS_H
#define SRSGNB_CU_CP_F1C_ASN1_HELPERS_H

#include "../ran/bcd_helpers.h"
#include "du_context.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
namespace srsgnb {

namespace srs_cu_cp {

/// \brief Converts ASN.1 CGI typo into internal struct.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
nr_cell_global_identity cgi_from_asn1(const asn1::f1ap::nrcgi_s& asn1_cgi)
{
  nr_cell_global_identity cgi          = {};
  uint32_t                encoded_plmn = asn1_cgi.plmn_id.to_number();
  ngap_plmn_to_mccmnc(encoded_plmn, &cgi.mcc, &cgi.mnc);
  cgi.nci.packed = asn1_cgi.nrcell_id.to_number();
  return cgi;
}

/// \brief Fills ASN.1 F1SetupResponse struct.
/// \param[out] response The F1SetupResponse ASN.1 struct to fill.
/// \param[in] params DU setup parameters to add to the F1SetupRequest.
void fill_asn1_f1_setup_response(asn1::f1ap::f1_setup_resp_s&                         response,
                                 const std::string&                                   name,
                                 const uint8_t                                        rrc_version,
                                 const slot_array<du_cell_context, MAX_NOF_DU_CELLS>& du_cell_db)
{
  // fill CU common info
  response->gnb_cu_name_present = true;
  response->gnb_cu_name->from_string(name);
  response->gnb_cu_rrc_version.value.latest_rrc_version.from_number(rrc_version);

  // activate all DU cells
  response->cells_to_be_activ_list_present = true;
  for (const auto& du_cell : du_cell_db) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> resp_cell;
    resp_cell->cells_to_be_activ_list_item().nrpci_present = true;
    resp_cell->cells_to_be_activ_list_item().nrpci         = du_cell.pci;
    // TODO: write converter to asn1::f1ap::nrcgi_s
    // resp_cell->cells_to_be_activ_list_item().nrcgi = to_nrcgi_s(du_cell.cgi);
    response->cells_to_be_activ_list.value.push_back(resp_cell);
  }
}

/// \brief Fills ASN.1 RRC Setup struct.
/// \param[out] rrc_setup The RRC Setup ASN.1 struct to fill.
/// \param[in] init_ul_rrc_transfer_msg The Init_UL_RRC_Transfer message received by the CU.
void fill_asn1_rrc_setup_msg(asn1::rrc_nr::rrc_setup_s&                     rrc_setup,
                             const initial_ul_rrc_message_transfer_message& init_ul_rrc_transfer_msg)
{
  asn1::rrc_nr::rrc_setup_ies_s& setup_ies = rrc_setup.crit_exts.set_rrc_setup();

  // Add SRB1
  // Note: See TS 38.331 section 5.3.5.6.3 - SRB addition/modification
  auto& radio_bearer_cfg = setup_ies.radio_bearer_cfg;
  radio_bearer_cfg.srb_to_add_mod_list.push_back(asn1::rrc_nr::srb_to_add_mod_s{});
  radio_bearer_cfg.srb_to_add_mod_list.back().srb_id = 1; // SRB1

  // Copy cell config from DU_to_CU_RRC_Container to master cell group
  auto& master_cell_group = setup_ies.master_cell_group;
  master_cell_group.resize(init_ul_rrc_transfer_msg.msg->duto_currc_container->size());
  std::copy(init_ul_rrc_transfer_msg.msg->duto_currc_container->begin(),
            init_ul_rrc_transfer_msg.msg->duto_currc_container->end(),
            master_cell_group.begin());
}

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_F1C_ASN1_HELPERS_H