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

} // namespace test_helpers
} // namespace srsran