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

///\file
///\brief f1ap_test_messages: Contains helper functions to generate dummy F1AP messages for testing purposes.

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/f1ap/common/interface_management.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/nr_band.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/positioning/positioning_ids.h"
#include "srsran/ran/rb_id.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/tac.h"

namespace srsran {

struct f1ap_message;

namespace test_helpers {

struct served_cell_item_info {
  struct meas_timing_config {
    uint32_t           carrier_freq = 535930;
    subcarrier_spacing scs          = subcarrier_spacing::kHz30;
  };

  nr_cell_identity   nci      = nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value();
  pci_t              pci      = 0;
  tac_t              tac      = 7;
  nr_band            band     = nr_band::n78;
  uint32_t           nr_arfcn = 626748;
  meas_timing_config meas_timing_cfg;
  std::string        sib1_str =
      "d20405c04300100e6640400000e000cd80018056109a000200046402051320c6b6c61b3704020000080800041a235241213042692f120000"
      "0464e35b63224f80b0664abff0124e9106e28dc61b8e372c6fbf56c70ea008580d003058b68228";
};

/// \brief Generates dummy F1 RESET message, sent by the CU to the DU, as per TS 38.473 section 8.2.1.1.
f1ap_message generate_f1ap_reset_message(
    const std::vector<std::pair<std::optional<gnb_du_ue_f1ap_id_t>, std::optional<gnb_cu_ue_f1ap_id_t>>>& ues_to_reset =
        {});

/// \brief Generate a dummy F1AP Served Cell Item.
asn1::f1ap::gnb_du_served_cells_item_s generate_served_cells_item(const served_cell_item_info& info);

/// \brief Generates dummy F1AP SETUP REQUEST message, sent by the DU to the CU, as per TS 38.473 section 8.2.3.1.
f1ap_message generate_f1_setup_request(gnb_du_id_t                               gnb_du_id = int_to_gnb_du_id(0x11),
                                       const std::vector<served_cell_item_info>& cells     = {served_cell_item_info{}});

/// \brief Generates dummy F1 SETUP RESPONSE message based on the request, sent by the CU to the DU, as per TS 38.473
/// section 8.2.3.2.
f1ap_message generate_f1_setup_response(const f1ap_message& f1_setup_request, bool activate_cells = true);

/// \brief Generates dummy F1 SETUP FAILURE message based on the request, sent by the CU to the DU, as per TS 38.473
/// section 8.2.3.3.
f1ap_message generate_f1_setup_failure(const f1ap_message& f1_setup_request);

/// \brief Generates dummy GNB-DU CONFIGURATION UPDATE ACKNOWLEDGE message based on the request, sent by the CU to the
/// DU, as per TS 38.473, 8.2.4.2.
f1ap_message generate_gnb_du_configuration_update_acknowledge(const f1ap_message& gnb_du_config_request);

/// \brief Generates dummy GNB-DU CONFIGURATION UPDATE FAILURE message based on the request, sent by the CU to the DU,
/// as per TS 38.473, 8.2.4.3.
f1ap_message generate_gnb_du_configuration_update_failure(const f1ap_message& gnb_du_config_request);

/// \brief Generates dummy GNB-CU CONFIGURATION UPDATE REQUEST message, sent by the CU to the DU, as per
/// TS 38.473, 8.2.5.1.
f1ap_message generate_gnb_cu_configuration_update_request(unsigned                        transaction_id,
                                                          span<const nr_cell_global_id_t> cgis_to_activate,
                                                          span<const nr_cell_global_id_t> cgis_to_deactivate = {});

/// \brief Generate a dummy GNB-CU CONFIGURATION ACKNOWLEDGEMENT message, sent by the DU to the CU, as per
/// TS 38.473, 8.2.5.2.
f1ap_message generate_gnb_cu_configuration_update_acknowledgement(
    const std::vector<f1ap_cell_failed_to_activate>& cells_failed_to_activate);

/// \brief Generate a dummy GNB-CU CONFIGURATION FAILURE message, sent by the DU to the CU, as per
/// TS 38.473, 8.2.5.3.
f1ap_message generate_gnb_cu_configuration_update_failure();

/// \brief Generates dummy F1 REMOVAL REQUEST message, as per TS 38.473 section 8.2.8.1.
f1ap_message generate_f1_removal_request(unsigned transaction_id);

/// \brief Generates dummy F1 REMOVAL RESPONSE message based on the request, as per TS 38.473 section 8.2.8.2.
f1ap_message generate_f1_removal_response(const f1ap_message& f1_removal_request);

/// \brief Generates dummy F1AP UE CONTEXT SETUP REQUEST message, sent by the CU to the DU, as per TS 38.473
/// section 8.3.1.1.
f1ap_message generate_ue_context_setup_request(gnb_cu_ue_f1ap_id_t                cu_ue_id,
                                               std::optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                               uint32_t                           rrc_container_pdcp_sn,
                                               const std::vector<drb_id_t>&       drbs_to_setup,
                                               nr_cell_global_id_t                nr_cgi);

/// \brief Generates dummy F1AP UE CONTEXT SETUP RESPONSE message, sent by the DU to the CU, as per TS 38.473
/// section 8.3.1.2.
f1ap_message generate_ue_context_setup_response(
    gnb_cu_ue_f1ap_id_t   cu_ue_id,
    gnb_du_ue_f1ap_id_t   du_ue_id,
    std::optional<rnti_t> crnti = std::nullopt,
    byte_buffer           cell_group_config =
        make_byte_buffer("5c02b091117aec701061e000b1c03544cde4a20c7c080408c008241000100000").value());

/// \brief Generates dummy F1AP UE CONTEXT SETUP FAILURE message, sent by the DU to the CU, as per TS 38.473
/// section 8.3.1.3.
f1ap_message generate_ue_context_setup_failure(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT RELEASE REQUEST message, sent by the DU to the CU, as per TS 38.473
/// section 8.3.2.1.
f1ap_message generate_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT RELEASE COMMAND message, sent by the CU to the DU, as per TS 38.473
/// section 8.3.3.1.
f1ap_message
generate_ue_context_release_command(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                    gnb_du_ue_f1ap_id_t du_ue_id,
                                    srb_id_t            srb_id        = srb_id_t::srb1,
                                    byte_buffer         rrc_container = byte_buffer::create({0x1, 0x2, 0x3}).value());

/// \brief Generates dummy F1AP UE CONTEXT RELEASE COMPLETE message, sent by the DU to the CU, as per TS 38.473
/// section 8.3.3.2.
f1ap_message generate_ue_context_release_complete(const f1ap_message& ue_ctxt_release_cmd);
f1ap_message generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION REQUEST message, sent by the CU to the DU, as per TS 38.473
/// section 8.3.4.1.
f1ap_message generate_ue_context_modification_request(gnb_du_ue_f1ap_id_t                    du_ue_id,
                                                      gnb_cu_ue_f1ap_id_t                    cu_ue_id,
                                                      const std::initializer_list<drb_id_t>& drbs_to_setup = {},
                                                      const std::initializer_list<drb_id_t>& drbs_to_mod   = {},
                                                      const std::initializer_list<drb_id_t>& drbs_to_rem   = {},
                                                      byte_buffer                            dl_dcch_msg   = {});

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION RESPONSE message, sent by the DU to the CU, as per TS 38.473
/// section 8.3.4.2.
f1ap_message
generate_ue_context_modification_response(gnb_du_ue_f1ap_id_t          du_ue_id,
                                          gnb_cu_ue_f1ap_id_t          cu_ue_id,
                                          rnti_t                       crnti               = to_rnti(0x4601),
                                          const std::vector<drb_id_t>& drbs_setup_mod_list = {drb_id_t::drb1},
                                          const std::vector<drb_id_t>& drbs_modified_list  = {});

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION FAILURE message, sent by the DU to the CU, as per TS 38.473
/// section 8.3.4.3.
f1ap_message generate_ue_context_modification_failure(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP INITIAL UL RRC Transfer message without DU to CU container, sent by the DU to the CU, as
/// per TS 38.473 section 8.4.1.1.
f1ap_message generate_init_ul_rrc_message_transfer_without_du_to_cu_container(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                              rnti_t              crnti);

/// \brief Generates dummy F1AP Initial UL RRC TRANSFER message, sent by the DU to the CU, as per TS 38.473
/// section 8.4.1.1.
f1ap_message generate_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                   rnti_t              crnti          = to_rnti(0x4601),
                                                   byte_buffer         cell_group_cfg = {},
                                                   byte_buffer         rrc_container  = {});

/// \brief Generates dummy F1AP DL RRC TRANSFER message with provided RRC container, sent by the CU to the DU, as per
/// TS 38.473 section 8.4.2.1.
f1ap_message generate_dl_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                              gnb_cu_ue_f1ap_id_t cu_ue_id,
                                              srb_id_t            srb_id,
                                              byte_buffer         rrc_container);

/// \brief Generates dummy F1AP UL RRC TRANSFER message with provided RRC container, sent by the DU to the CU, as per
/// TS 38.473 section 8.4.3.1.
f1ap_message generate_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                              gnb_cu_ue_f1ap_id_t cu_ue_id,
                                              srb_id_t            srb_id = srb_id_t::srb1,
                                              byte_buffer rrc_container = byte_buffer::create({0x1, 0x2, 0x3}).value());

/// \brief Generates dummy F1AP UL RRC TRANSFER message with provided RRC UL DCCH message and PDCP header with provided
/// SN, sent by the DU to the CU, as per TS 38.473 section 8.4.3.1.
f1ap_message generate_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                              gnb_cu_ue_f1ap_id_t cu_ue_id,
                                              srb_id_t            srb_id,
                                              uint32_t            pdcp_sn,
                                              byte_buffer         ul_dcch_msg);

/// \brief Create RRC Container with PDCP header and DL-DCCH message.
byte_buffer create_dl_dcch_rrc_container(uint32_t pdcp_sn, const std::initializer_list<uint8_t>& dl_dcch_msg);
byte_buffer create_dl_dcch_rrc_container(uint32_t pdcp_sn, const byte_buffer& dl_dcch_msg);

/// \brief Remove PDCP header from DL-DCCH message.
byte_buffer extract_dl_dcch_msg(const byte_buffer& rrc_container);

/// \brief Generates dummy F1AP TRP INFORMATION REQUEST message.
f1ap_message generate_trp_information_request();

/// \brief Generates dummy F1AP TRP INFORMATION RESPONSE message.
f1ap_message generate_trp_information_response(const std::vector<trp_id_t>& trp_ids);

/// \brief Generates dummy F1AP TRP INFORMATION FAILURE message.
f1ap_message generate_trp_information_failure();

/// \brief Generates dummy F1AP POSITIONING INFORMATION REQUEST message.
f1ap_message generate_positioning_information_request(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING INFORMATION RESPONSE message.
f1ap_message generate_positioning_information_response(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING INFORMATION FAILURE message.
f1ap_message generate_positioning_information_failure(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING ACTIVATION RESPONSE message.
f1ap_message generate_positioning_activation_response(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING ACTIVATION FAILURE message.
f1ap_message generate_positioning_activation_failure(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING MEASUREMENT REQUEST message.
f1ap_message
generate_positioning_measurement_request(std::vector<trp_id_t>                                trp_ids,
                                         lmf_meas_id_t                                        lmf_meas_id,
                                         ran_meas_id_t                                        ran_meas_id,
                                         std::vector<asn1::f1ap::pos_meas_type_opts::options> pos_meas_type_list =
                                             {asn1::f1ap::pos_meas_type_opts::options::ul_rtoa},
                                         subcarrier_spacing scs        = subcarrier_spacing::kHz15,
                                         unsigned           srs_offset = 0U);

/// \brief Generates dummy F1AP POSITIONING MEASUREMENT RESPONSE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.3.2.
f1ap_message generate_positioning_measurement_response(lmf_meas_id_t                lmf_meas_id,
                                                       ran_meas_id_t                ran_meas_id,
                                                       const std::vector<trp_id_t>& trp_ids,
                                                       unsigned                     transaction_id = 1);

/// \brief Generates dummy F1AP POSITIONING MEASUREMENT FAILURE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.3.3.
f1ap_message generate_positioning_measurement_failure(lmf_meas_id_t lmf_meas_id, ran_meas_id_t ran_meas_id);

/// \brief Generates dummy F1AP TRP INFORMATION RESPONSE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.8.2.
f1ap_message generate_trp_information_response(const trp_id_t& trp_id);

/// \brief Generates dummy F1AP TRP INFORMATION FAILURE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.8.3.
f1ap_message generate_trp_information_failure();

/// \brief Generates dummy F1AP POSITIONING INFORMATION RESPONSE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.9.2.
f1ap_message generate_positioning_information_response(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING INFORMATION FAILURE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.9.3.
f1ap_message generate_positioning_information_failure(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING ACTIVATION RESPONSE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.10.2.
f1ap_message generate_positioning_activation_response(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

/// \brief Generates dummy F1AP POSITIONING ACTIVATION FAILURE message, sent by the DU to the CU, as per
/// TS 38.473 section 8.13.10.3.
f1ap_message generate_positioning_activation_failure(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id);

} // namespace test_helpers
} // namespace srsran
