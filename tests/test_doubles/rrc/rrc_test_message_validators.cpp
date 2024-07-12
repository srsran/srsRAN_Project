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

bool srsran::test_helpers::is_valid_rrc_reconfiguration(const asn1::rrc_nr::dl_dcch_msg_s& msg)
{
  TRUE_OR_RETURN(msg.msg.type().value == asn1::rrc_nr::dl_dcch_msg_type_c::types_opts::c1);
  TRUE_OR_RETURN(msg.msg.c1().type().value == asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg);
  TRUE_OR_RETURN(msg.msg.c1().rrc_recfg().crit_exts.type().value ==
                 asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types_opts::rrc_recfg);
  TRUE_OR_RETURN(msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().non_crit_ext_present);
  TRUE_OR_RETURN(msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg().non_crit_ext.ded_nas_msg_list.size() != 0);
  return true;
}

bool srsran::test_helpers::is_valid_rrc_reconfiguration(const byte_buffer& dl_dcch_msg)
{
  asn1::cbit_ref              bref{dl_dcch_msg};
  asn1::rrc_nr::dl_dcch_msg_s dcch;
  TRUE_OR_RETURN(dcch.unpack(bref) == asn1::SRSASN_SUCCESS);
  return is_valid_rrc_reconfiguration(dcch);
}