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

#include "srsran/asn1/rrc_nr/dl_ccch_msg.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"

namespace srsran {
namespace test_helpers {

/// \brief Check if DL-CCCH message is a valid RRC Setup message.
bool is_valid_rrc_setup(const asn1::rrc_nr::dl_ccch_msg_s& msg);
bool is_valid_rrc_setup(const byte_buffer& dl_ccch_msg);

/// \brief Check if DL-DCCH message is a valid RRC Reestablishment message.
bool is_valid_rrc_reestablishment(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_reestablishment(const byte_buffer& dl_dcch_msg);

/// \brief Check if DL-DCCH message is a valid RRC Security Mode Command message.
bool is_valid_rrc_security_mode_command(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_security_mode_command(const byte_buffer& dl_dcch_msg);

/// \brief Check if DL-DCCH message is a valid RRC UE Capability Enquiry message.
bool is_valid_rrc_ue_capability_enquiry(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_ue_capability_enquiry(const byte_buffer& dl_dcch_msg);

/// \brief Check if DL-DCCH message is a valid RRC Reconfiguration message.
bool is_valid_rrc_reconfiguration(const asn1::rrc_nr::dl_dcch_msg_s& msg);
bool is_valid_rrc_reconfiguration(const byte_buffer& dl_dcch_msg);

} // namespace test_helpers
} // namespace srsran