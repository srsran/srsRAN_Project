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

#include "e2sm_kpm_asn1_packer.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

const std::string e2sm_kpm_asn1_packer::short_name       = "ORAN-E2SM-KPM";
const std::string e2sm_kpm_asn1_packer::oid              = "1.3.6.1.4.1.53148.1.2.2.2";
const std::string e2sm_kpm_asn1_packer::func_description = "KPM Monitor";
const uint32_t    e2sm_kpm_asn1_packer::revision         = 0;

e2sm_kpm_asn1_packer::e2sm_kpm_asn1_packer()
{
  // array of supported metrics in string format
  supported_metrics = {"CQI", "RSRP", "RSRQ"};
}

e2_sm_kpm_action_definition_s
e2sm_kpm_asn1_packer::handle_packed_e2sm_kpm_action_definition(const srsran::byte_buffer& action_definition)
{
  e2_sm_kpm_action_definition_s action_def;
  asn1::cbit_ref                bref(action_definition);
  if (action_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM KPM Action Definition\n");
  }
  return action_def;
}

e2_sm_kpm_event_trigger_definition_s
e2sm_kpm_asn1_packer::handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition)
{
  e2_sm_kpm_event_trigger_definition_s event_trigger_def;
  asn1::cbit_ref                       bref(event_trigger_definition);
  if (event_trigger_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM KPM Event Trigger Definition\n");
  }
  return event_trigger_def;
}

asn1::unbounded_octstring<true> e2sm_kpm_asn1_packer::pack_ran_function_description()
{
  e2_sm_kpm_ra_nfunction_description_s ran_function_desc;
  asn1::unbounded_octstring<true>      ran_function_description;
  // Add ran_function_name item.
  ran_function_desc.ran_function_name.ran_function_short_name.resize(short_name.size());
  ran_function_desc.ran_function_name.ran_function_e2_sm_oid.resize(oid.size());
  ran_function_desc.ran_function_name.ran_function_description.resize(func_description.size());

  ran_function_desc.ran_function_name.ran_function_short_name.from_string(short_name.c_str());
  ran_function_desc.ran_function_name.ran_function_e2_sm_oid.from_string(oid.c_str());
  ran_function_desc.ran_function_name.ran_function_description.from_string(func_description.c_str());
  ran_function_desc.ext = false;
  // Add ric_event_trigger_style item.
  ric_event_trigger_style_item_s ric_event_trigger_style_item;
  ric_event_trigger_style_item.ric_event_trigger_style_type = 1;
  ric_event_trigger_style_item.ric_event_trigger_style_name.from_string("Periodic Report");
  ric_event_trigger_style_item.ric_event_trigger_format_type = 1;
  ran_function_desc.ric_event_trigger_style_list.push_back(ric_event_trigger_style_item);

  // Add ric_report_style item.
  ric_report_style_item_s ric_report_style_item;
  ric_report_style_item.ric_report_style_type = 3;
  ric_report_style_item.ric_report_style_name.from_string("Condition-based, UE-level E2 Node Measurement");
  ric_report_style_item.ric_action_format_type  = 3;
  ric_report_style_item.ric_ind_hdr_format_type = 2;
  ric_report_style_item.ric_ind_msg_format_type = 2;

  for (auto metric : supported_metrics) {
    meas_info_action_item_s meas_info_action_item;
    meas_info_action_item.meas_name.from_string(metric.c_str());
    meas_info_action_item.meas_id_present = false;
    ric_report_style_item.meas_info_action_list.push_back(meas_info_action_item);
  }
  ran_function_desc.ric_report_style_list.push_back(ric_report_style_item);

  srsran::byte_buffer buf;
  asn1::bit_ref       bref(buf);
  if (ran_function_desc.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to pack E2SM KPM RAN Function Description\n");
    return ran_function_description;
  }

  ran_function_description.resize(buf.length());
  std::copy(buf.begin(), buf.end(), ran_function_description.begin());
  return ran_function_description;
}