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
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>
namespace srsran {

class e2sm_kpm_impl : public e2sm_interface
{
public:
  // constructor takes logger as argument
  e2sm_kpm_impl(srslog::basic_logger&    logger_,
                e2sm_handler&            e2sm_packer_,
                e2_du_metrics_interface& du_metrics_interface_);

  srsran::byte_buffer handle_action(uint32_t action_id, const srsran::byte_buffer& action_definition) override;

  srsran::byte_buffer get_indication_header(uint32_t action_id) override;

  e2sm_handler& get_e2sm_packer() override;

  static bool supported_test_cond_type(asn1::e2sm_kpm::test_cond_type_c test_cond_type)
  {
    if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::cqi) {
      return true;
    } else if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::rsrp) {
      return true;
    } else if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::rsrq) {
      return true;
    } else {
      return false;
    }
  }

private:
  void process_action_definition(asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s&          ric_ind_msg,
                                 asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def);

  bool check_measurement_name(asn1::e2sm_kpm::meas_type_c meas_type, const char* meas);

  void generate_indication_header(uint32_t action_id);

  void add_matching_condition_item(const char*                            name,
                                   asn1::e2sm_kpm::meas_cond_ueid_item_s& cond_ueid_item,
                                   asn1::e2sm_kpm::matching_cond_item_s&  match_cond_item);

  // process action definition format 1 & uses fields to populate RIC indication message
  void handle_action_definition_format1(asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s&                  ric_ind_message,
                                        asn1::e2sm_kpm::e2_sm_kpm_action_definition_format1_s action_def);

  // process action definition format 3 & uses fields to populate RIC indication message
  void handle_action_definition_format3(asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s&                  ric_ind_message,
                                        asn1::e2sm_kpm::e2_sm_kpm_action_definition_format3_s action_def);

  srslog::basic_logger&           logger;
  e2sm_handler&                   e2sm_packer;
  e2_du_metrics_interface&        du_metrics_interface;
  std::map<uint32_t, byte_buffer> ind_hdr_map;
};

} // namespace srsran
