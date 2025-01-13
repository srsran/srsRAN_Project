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

#include "rrc_test_message_validators.h"

using namespace srsran;
using namespace asn1::rrc_nr;

#define TRUE_OR_RETURN(cond)                                                                                           \
  if (not(cond))                                                                                                       \
    return false;

bool srsran::test_helpers::is_valid_rrc_setup(const asn1::rrc_nr::dl_ccch_msg_s& msg)
{
  TRUE_OR_RETURN(msg.msg.type().value == asn1::rrc_nr::dl_ccch_msg_type_c::types_opts::c1);
  TRUE_OR_RETURN(msg.msg.c1().type().value == asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_setup);
  TRUE_OR_RETURN(msg.msg.c1().rrc_setup().crit_exts.type().value ==
                 asn1::rrc_nr::rrc_setup_s::crit_exts_c_::types_opts::rrc_setup);
  return true;
}

bool srsran::test_helpers::is_valid_rrc_setup(const byte_buffer& dl_ccch_msg)
{
  asn1::cbit_ref              bref{dl_ccch_msg};
  asn1::rrc_nr::dl_ccch_msg_s ccch;
  TRUE_OR_RETURN(ccch.unpack(bref) == asn1::SRSASN_SUCCESS);
  return is_valid_rrc_setup(ccch);
}

bool srsran::test_helpers::is_valid_rrc_reestablishment(const asn1::rrc_nr::dl_dcch_msg_s& msg)
{
  TRUE_OR_RETURN(msg.msg.type().value == asn1::rrc_nr::dl_dcch_msg_type_c::types_opts::c1);
  TRUE_OR_RETURN(msg.msg.c1().type().value == asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::rrc_reest);
  TRUE_OR_RETURN(msg.msg.c1().rrc_reest().crit_exts.type().value ==
                 asn1::rrc_nr::rrc_reest_s::crit_exts_c_::types_opts::rrc_reest);
  return true;
}

bool srsran::test_helpers::is_valid_rrc_reestablishment(const byte_buffer& dl_dcch_msg)
{
  asn1::cbit_ref              bref{dl_dcch_msg};
  asn1::rrc_nr::dl_dcch_msg_s dcch;
  TRUE_OR_RETURN(dcch.unpack(bref) == asn1::SRSASN_SUCCESS);
  return is_valid_rrc_reestablishment(dcch);
}

bool srsran::test_helpers::is_valid_rrc_security_mode_command(const asn1::rrc_nr::dl_dcch_msg_s& msg)
{
  TRUE_OR_RETURN(msg.msg.type().value == asn1::rrc_nr::dl_dcch_msg_type_c::types_opts::c1);
  TRUE_OR_RETURN(msg.msg.c1().type().value == asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::security_mode_cmd);
  TRUE_OR_RETURN(msg.msg.c1().security_mode_cmd().crit_exts.type().value ==
                 asn1::rrc_nr::security_mode_cmd_s::crit_exts_c_::types_opts::security_mode_cmd);
  return true;
}

bool srsran::test_helpers::is_valid_rrc_security_mode_command(const byte_buffer& dl_dcch_msg)
{
  asn1::cbit_ref              bref{dl_dcch_msg};
  asn1::rrc_nr::dl_dcch_msg_s dcch;
  TRUE_OR_RETURN(dcch.unpack(bref) == asn1::SRSASN_SUCCESS);
  return is_valid_rrc_security_mode_command(dcch);
}

bool srsran::test_helpers::is_valid_rrc_ue_capability_enquiry(const asn1::rrc_nr::dl_dcch_msg_s& msg)
{
  TRUE_OR_RETURN(msg.msg.type().value == asn1::rrc_nr::dl_dcch_msg_type_c::types_opts::c1);
  TRUE_OR_RETURN(msg.msg.c1().type().value == asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::ue_cap_enquiry);
  TRUE_OR_RETURN(msg.msg.c1().ue_cap_enquiry().crit_exts.type().value ==
                 asn1::rrc_nr::ue_cap_enquiry_s::crit_exts_c_::types_opts::ue_cap_enquiry);
  return true;
}

bool srsran::test_helpers::is_valid_rrc_ue_capability_enquiry(const byte_buffer& dl_dcch_msg)
{
  asn1::cbit_ref              bref{dl_dcch_msg};
  asn1::rrc_nr::dl_dcch_msg_s dcch;
  TRUE_OR_RETURN(dcch.unpack(bref) == asn1::SRSASN_SUCCESS);
  return is_valid_rrc_ue_capability_enquiry(dcch);
}

bool srsran::test_helpers::is_valid_rrc_reconfiguration(
    const asn1::rrc_nr::dl_dcch_msg_s&          msg,
    bool                                        contains_nas_pdu,
    const std::optional<std::vector<srb_id_t>>& expected_srbs_to_add_mod,
    const std::optional<std::vector<drb_id_t>>& expected_drbs_to_add_mod,
    const std::optional<std::vector<drb_id_t>>& expected_drbs_to_release)
{
  TRUE_OR_RETURN(msg.msg.type().value == asn1::rrc_nr::dl_dcch_msg_type_c::types_opts::c1);
  TRUE_OR_RETURN(msg.msg.c1().type().value == asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg);
  TRUE_OR_RETURN(msg.msg.c1().rrc_recfg().crit_exts.type().value ==
                 asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types_opts::rrc_recfg);
  TRUE_OR_RETURN(msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().non_crit_ext_present);
  if (contains_nas_pdu) {
    TRUE_OR_RETURN(msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().non_crit_ext.ded_nas_msg_list.size() != 0);
  }

  if (!msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().radio_bearer_cfg_present &&
      (expected_drbs_to_add_mod.has_value() or expected_drbs_to_add_mod.has_value())) {
    return false;
  }

  if (msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().radio_bearer_cfg_present) {
    if (expected_srbs_to_add_mod.has_value()) {
      std::vector<srb_id_t> srbs_to_add_mod;
      for (const auto& srb : msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().radio_bearer_cfg.srb_to_add_mod_list) {
        srbs_to_add_mod.push_back(int_to_srb_id(srb.srb_id));
      }

      TRUE_OR_RETURN(srbs_to_add_mod.size() == expected_srbs_to_add_mod.value().size());
      TRUE_OR_RETURN(
          std::equal(srbs_to_add_mod.begin(), srbs_to_add_mod.end(), expected_srbs_to_add_mod.value().begin()))
    }

    if (expected_drbs_to_add_mod.has_value()) {
      std::vector<drb_id_t> drbs_to_add_mod;
      for (const auto& drb : msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().radio_bearer_cfg.drb_to_add_mod_list) {
        drbs_to_add_mod.push_back(uint_to_drb_id(drb.drb_id));
      }

      TRUE_OR_RETURN(drbs_to_add_mod.size() == expected_drbs_to_add_mod.value().size());
      TRUE_OR_RETURN(
          std::equal(drbs_to_add_mod.begin(), drbs_to_add_mod.end(), expected_drbs_to_add_mod.value().begin()))
    }

    if (expected_drbs_to_release.has_value()) {
      std::vector<drb_id_t> drbs_to_release;
      for (const auto& drb : msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().radio_bearer_cfg.drb_to_release_list) {
        drbs_to_release.push_back(uint_to_drb_id(drb));
      }

      TRUE_OR_RETURN(drbs_to_release.size() == expected_drbs_to_release.value().size());
      TRUE_OR_RETURN(
          std::equal(drbs_to_release.begin(), drbs_to_release.end(), expected_drbs_to_release.value().begin()))
    }
  }
  return true;
}

bool srsran::test_helpers::is_valid_rrc_reconfiguration(
    const byte_buffer&                          dl_dcch_msg,
    bool                                        contains_nas_pdu,
    const std::optional<std::vector<srb_id_t>>& expected_srbs_to_add_mod,
    const std::optional<std::vector<drb_id_t>>& expected_drbs_to_add_mod,
    const std::optional<std::vector<drb_id_t>>& expected_drbs_to_release)
{
  asn1::cbit_ref              bref{dl_dcch_msg};
  asn1::rrc_nr::dl_dcch_msg_s dcch;
  TRUE_OR_RETURN(dcch.unpack(bref) == asn1::SRSASN_SUCCESS);
  return is_valid_rrc_reconfiguration(
      dcch, contains_nas_pdu, expected_srbs_to_add_mod, expected_drbs_to_add_mod, expected_drbs_to_release);
}

bool srsran::test_helpers::is_valid_rrc_handover_preparation_info(const asn1::rrc_nr::ho_prep_info_s& ho_prep_info)
{
  TRUE_OR_RETURN(ho_prep_info.crit_exts.type().value == asn1::rrc_nr::ho_prep_info_s::crit_exts_c_::types_opts::c1);
  TRUE_OR_RETURN(ho_prep_info.crit_exts.c1().type().value ==
                 asn1::rrc_nr::ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::ho_prep_info);
  TRUE_OR_RETURN(ho_prep_info.crit_exts.c1().ho_prep_info().ue_cap_rat_list.size() > 0U);

  return true;
}

bool srsran::test_helpers::is_valid_rrc_handover_preparation_info(const byte_buffer& ho_prep_info)
{
  asn1::cbit_ref               bref{ho_prep_info};
  asn1::rrc_nr::ho_prep_info_s msg;
  TRUE_OR_RETURN(msg.unpack(bref) == asn1::SRSASN_SUCCESS);
  return is_valid_rrc_handover_preparation_info(msg);
}
