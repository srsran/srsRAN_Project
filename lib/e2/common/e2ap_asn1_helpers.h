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

#include "e2ap_asn1_utils.h"
#include "e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "e2sm/e2sm_rc/e2sm_rc_asn1_packer.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/security/security.h"
#include <string>
#include <vector>

namespace srsran {

inline void
fill_ran_function_item(asn1::e2ap::e2setup_request_s& setup, const std::string& ran_oid, e2sm_interface* e2_iface)
{
  using namespace asn1::e2ap;
  srslog::basic_logger&                                         logger = srslog::fetch_basic_logger("E2");
  asn1::protocol_ie_single_container_s<ra_nfunction_item_ies_o> ran_func_item;
  ran_func_item.load_info_obj(ASN1_E2AP_ID_RA_NFUNCTION_ITEM);
  auto& ran_function_item = ran_func_item->ra_nfunction_item();

  ran_func_item.value().ra_nfunction_item().ran_function_id =
      (ran_oid == e2sm_kpm_asn1_packer::oid) ? e2sm_kpm_asn1_packer::ran_func_id : e2sm_rc_asn1_packer::ran_func_id;
  ran_func_item.value().ra_nfunction_item().ran_function_revision = 0;
  ran_func_item.value().ra_nfunction_item().ran_function_oid.from_string(ran_oid);

  ran_function_item.ran_function_definition = e2_iface->get_e2sm_packer().pack_ran_function_description();
  if (ran_function_item.ran_function_definition.size()) {
    setup->ra_nfunctions_added.value.push_back(ran_func_item);
  } else {
    logger.error("Failed to pack RAN function description");
  }
}

inline void fill_asn1_e2ap_setup_request(asn1::e2ap::e2setup_request_s& setup,
                                         const e2ap_configuration&      e2ap_config,
                                         e2sm_manager&                  e2sm_mngr)
{
  using namespace asn1::e2ap;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("E2");
  e2_message            e2_msg;
  init_msg_s&           initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);
  setup = initmsg.value.e2setup_request();

  //  Transaction ID
  setup->transaction_id.crit        = asn1::crit_opts::reject;
  setup->transaction_id.value.value = 1;

  // Global e2 node ID
  setup->global_e2node_id.crit = asn1::crit_opts::reject;
  auto& gnb_id                 = setup->global_e2node_id.value.set_gnb();
  gnb_id.global_g_nb_id.gnb_id.gnb_id().from_number(e2ap_config.gnb_id, 28);
  // convert PLMN to BCD
  uint32_t plmn_bcd = plmn_string_to_bcd(e2ap_config.plmn);
  gnb_id.global_g_nb_id.plmn_id.from_number(plmn_bcd);

  // RAN functions added
  setup->ra_nfunctions_added.crit = asn1::crit_opts::reject;
  setup->ra_nfunctions_added.id   = ASN1_E2AP_ID_RA_NFUNCTIONS_ADDED;
  if (e2ap_config.e2sm_kpm_enabled) {
    std::string ran_oid = e2sm_kpm_asn1_packer::oid;
    logger.info("Generate RAN function definition for OID: {}", ran_oid.c_str());
    e2sm_interface* e2_iface = e2sm_mngr.get_e2sm_interface(ran_oid);
    if (e2_iface) {
      fill_ran_function_item(setup, ran_oid, e2_iface);
    } else {
      logger.error("No E2SM interface found for RAN OID {}", ran_oid.c_str());
    }
  }
  if (e2ap_config.e2sm_rc_enabled) {
    std::string ran_oid = e2sm_rc_asn1_packer::oid;
    logger.info("Generate RAN function definition for OID: {}", ran_oid.c_str());
    e2sm_interface* e2_iface = e2sm_mngr.get_e2sm_interface(ran_oid);
    if (e2_iface) {
      fill_ran_function_item(setup, ran_oid, e2_iface);
    } else {
      logger.error("No E2SM interface found for RAN OID {}", ran_oid.c_str());
    }
  }

  // E2 node component config
  setup->e2node_component_cfg_addition.crit = asn1::crit_opts::reject;
  auto& list                                = setup->e2node_component_cfg_addition.value;
  list.resize(1);
  list[0].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
  e2node_component_cfg_addition_item_s& e2node_cfg_item = list[0].value().e2node_component_cfg_addition_item();
  e2node_cfg_item.e2node_component_interface_type       = e2node_component_interface_type_opts::ng;
  e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string("nginterf");
  e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.from_string("72657170617274");
  e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.from_string("72657370617274");
}

} // namespace srsran