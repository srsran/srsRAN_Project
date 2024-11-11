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
#include "srsran/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "srsran/asn1/rrc_nr/ho_prep_info.h"
#include "srsran/ran/rb_id.h"
#include <optional>

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
bool is_valid_rrc_reconfiguration(const asn1::rrc_nr::dl_dcch_msg_s&          msg,
                                  bool                                        contains_nas_pdu,
                                  const std::optional<std::vector<srb_id_t>>& expected_srbs_to_add_mod,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_add_mod,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_release);
bool is_valid_rrc_reconfiguration(const byte_buffer&                          dl_dcch_msg,
                                  bool                                        contains_nas_pdu         = true,
                                  const std::optional<std::vector<srb_id_t>>& expected_srbs_to_add_mod = std::nullopt,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_add_mod = std::nullopt,
                                  const std::optional<std::vector<drb_id_t>>& expected_drbs_to_release = std::nullopt);

bool is_valid_rrc_handover_preparation_info(const asn1::rrc_nr::ho_prep_info_s& ho_prep_info);
bool is_valid_rrc_handover_preparation_info(const byte_buffer& ho_prep_info);

} // namespace test_helpers
} // namespace srsran