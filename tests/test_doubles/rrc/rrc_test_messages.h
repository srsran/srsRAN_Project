/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/rrc_nr/ul_ccch_msg.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"

namespace srsran {

/// \brief Generates a dummy RRC Setup Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_setup_request();

/// \brief Generates a dummy RRC Reestablishment Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_reestablishment_request(rnti_t old_crnti, pci_t old_pci);

byte_buffer pack_ul_ccch_msg(const asn1::rrc_nr::ul_ccch_msg_s& msg);

} // namespace srsran