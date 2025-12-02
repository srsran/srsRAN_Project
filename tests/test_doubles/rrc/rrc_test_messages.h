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

#include "srsran/asn1/rrc_nr/sys_info.h"
#include "srsran/asn1/rrc_nr/ul_ccch_msg.h"
#include "srsran/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace test_helpers {

/// \brief Generates a dummy RRC Setup Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_setup_request();

/// \brief Generates a dummy RRC Reestablishment Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_reestablishment_request(rnti_t             old_crnti,
                                                               pci_t              old_pci,
                                                               const std::string& short_mac_i = "1100011101010100");

/// \brief Generates a dummy RRC Setup Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_setup_complete(uint8_t sel_plmn_id = 1);

/// \brief Generates a dummy RRC Reestablishment Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_reestablishment_complete();

/// \brief Generates a dummy RRC Reconfiguration Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_reconfiguration_complete(uint8_t transaction_id = 0);

/// \brief Packs an RRC UL-CCCH message into a byte buffer.
byte_buffer pack_ul_ccch_msg(const asn1::rrc_nr::ul_ccch_msg_s& msg);

/// \brief Packs an RRC UL-DCCH message into a byte buffer.
byte_buffer pack_ul_dcch_msg(const asn1::rrc_nr::ul_dcch_msg_s& msg);

/// \brief Generates a dummy SIB1 message.
asn1::rrc_nr::sib1_s create_sib1(const plmn_identity& plmn = plmn_identity::test_value());

} // namespace test_helpers
} // namespace srsran
