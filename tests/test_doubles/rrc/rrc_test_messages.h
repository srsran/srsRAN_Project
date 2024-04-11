/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/asn1/rrc_nr/ul_ccch_msg.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"

namespace srsran {

/// \brief Generates a dummy RRC Setup Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_setup_request();

/// \brief Generates a dummy RRC Reestablishment Request message.
asn1::rrc_nr::ul_ccch_msg_s create_rrc_reestablishment_request(rnti_t             old_crnti,
                                                               pci_t              old_pci,
                                                               const std::string& short_mac_i = "1100011101010100");

/// \brief Generates a dummy RRC Setup Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_setup_complete();

/// \brief Generates a dummy RRC Reestablishment Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_reestablishment_complete();

/// \brief Generates a dummy RRC Reconfiguration Complete message.
asn1::rrc_nr::ul_dcch_msg_s create_rrc_reconfiguration_complete(uint8_t transaction_id = 0);

/// \brief Packs an RRC UL-CCCH message into a byte buffer.
byte_buffer pack_ul_ccch_msg(const asn1::rrc_nr::ul_ccch_msg_s& msg);

/// \brief Packs an RRC UL-DCCH message into a byte buffer.
byte_buffer pack_ul_dcch_msg(const asn1::rrc_nr::ul_dcch_msg_s& msg);

} // namespace srsran