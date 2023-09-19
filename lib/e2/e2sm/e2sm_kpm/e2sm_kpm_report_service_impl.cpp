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

#include "e2sm_kpm_report_service_impl.h"
#include "e2sm_kpm_utils.h"
#include <algorithm>

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

e2sm_kpm_report_service_base::e2sm_kpm_report_service_base(e2_sm_kpm_action_definition_s action_def_,
                                                           e2sm_kpm_meas_provider&       meas_provider_) :
  logger(srslog::fetch_basic_logger("E2SM-KPM")),
  action_def_generic(action_def_),
  meas_provider(meas_provider_),
  ric_ind_header(ric_ind_header_generic.ind_hdr_formats.ind_hdr_format1())
{
  // Initialize RIC indication header.
  ric_ind_header.vendor_name_present        = false;
  ric_ind_header.sender_name_present        = false;
  ric_ind_header.sender_type_present        = false;
  ric_ind_header.file_formatversion_present = false;
  ric_ind_header.collet_start_time.from_number(std::time(0));

  is_ind_msg_ready_ = false;
}

bool e2sm_kpm_report_service_base::initialize_ric_ind_msg_format_1(meas_info_list_l&            action_meas_info_list,
                                                                   e2_sm_kpm_ind_msg_format1_s& ric_ind_msg)
{
  if (granul_period) {
    ric_ind_msg.granul_period_present = true;
    ric_ind_msg.granul_period         = granul_period;
  } else {
    ric_ind_msg.granul_period = 0;
  }

  ric_ind_msg.meas_info_list = action_meas_info_list;
  return true;
}

bool e2sm_kpm_report_service_base::is_ind_msg_ready()
{
  if (not is_ind_msg_ready_) {
    // Discard the collected measurements as they are only no_values.
    clear_collect_measurements();
    return false;
  }
  return true;
}

srsran::byte_buffer e2sm_kpm_report_service_base::get_indication_message()
{
  byte_buffer   ind_msg_bytes;
  asn1::bit_ref bref_ind_msg(ind_msg_bytes);
  if (ric_ind_message_generic.pack(bref_ind_msg) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RIC Indication Message");
  };

  clear_collect_measurements();

  return ind_msg_bytes;
}

srsran::byte_buffer e2sm_kpm_report_service_base::get_indication_header()
{
  byte_buffer   ind_hdr_bytes;
  asn1::bit_ref bref_ind_hdr(ind_hdr_bytes);
  if (ric_ind_header_generic.pack(bref_ind_hdr) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RIC Indication header");
  };

  return ind_hdr_bytes;
}

e2sm_kpm_report_service_style1::e2sm_kpm_report_service_style1(e2_sm_kpm_action_definition_s action_def_,
                                                               e2sm_kpm_meas_provider&       meas_provider_) :
  e2sm_kpm_report_service_base(action_def_, meas_provider_),
  action_def(action_def_generic.action_definition_formats.action_definition_format1()),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format1())
{
  granul_period = action_def.granul_period;
  if (action_def.cell_global_id_present) {
    cell_global_id = action_def.cell_global_id;
  }

  // Initialize RIC indication metadata.
  initialize_ric_ind_msg_format_1(action_def.meas_info_list, ric_ind_message);
}

void e2sm_kpm_report_service_style1::clear_collect_measurements()
{
  ric_ind_message.meas_data.clear();
  // Save timestamp of measurement collection start.
  ric_ind_header.collet_start_time.from_number(std::time(0));
  // Reset indication msg ready flag.
  is_ind_msg_ready_ = false;
}

bool e2sm_kpm_report_service_style1::collect_measurements()
{
  // Set the granularity period (TODO: disable as currently not supported in flexric).
  ric_ind_message.granul_period_present = false;
  // ric_ind_message.granul_period         = granul_period;

  // Fill indication msg.
  std::vector<meas_record_item_c> meas_records_items;
  meas_data_item_s                meas_data_item;

  auto& meas_info_list = ric_ind_message.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Get measurements.
    meas_records_items.clear();
    meas_provider.get_meas_data(meas_info.meas_type, meas_info.label_info_list, {}, cell_global_id, meas_records_items);
    // Fill measurements data.
    meas_data_item.meas_record.push_back(meas_records_items[0]);
  }
  ric_ind_message.meas_data.push_back(meas_data_item);
  // As E2 node is always present, each meas_record is valid value and indication is ready.
  is_ind_msg_ready_ = true;
  return true;
}

e2sm_kpm_report_service_style2::e2sm_kpm_report_service_style2(e2_sm_kpm_action_definition_s action_def_,
                                                               e2sm_kpm_meas_provider&       meas_provider_) :
  e2sm_kpm_report_service_base(action_def_, meas_provider_),
  action_def(action_def_generic.action_definition_formats.action_definition_format2()),
  ue_id(action_def_generic.action_definition_formats.action_definition_format2().ue_id),
  subscript_info(action_def_generic.action_definition_formats.action_definition_format2().subscript_info),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format1())
{
  granul_period = subscript_info.granul_period;
  if (subscript_info.cell_global_id_present) {
    cell_global_id = subscript_info.cell_global_id;
  }

  // Initialize RIC indication metadata.
  initialize_ric_ind_msg_format_1(subscript_info.meas_info_list, ric_ind_message);
}

void e2sm_kpm_report_service_style2::clear_collect_measurements()
{
  ric_ind_message.meas_data.clear();

  // Save timestamp of measurement collection start.
  ric_ind_header.collet_start_time.from_number(std::time(0));

  // Reset indication msg ready flag.
  is_ind_msg_ready_ = false;
}

bool e2sm_kpm_report_service_style2::collect_measurements()
{
  // Fill indication msg.
  std::vector<meas_record_item_c> meas_records_items;
  meas_data_item_s                meas_data_item;

  auto& meas_info_list = action_def.subscript_info.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Get measurements.
    meas_records_items.clear();
    meas_provider.get_meas_data(
        meas_info.meas_type, meas_info.label_info_list, {ue_id}, cell_global_id, meas_records_items);
    // Fill measurements data.
    meas_data_item.meas_record.push_back(meas_records_items[0]);
  }
  ric_ind_message.meas_data.push_back(meas_data_item);
  if (not is_ind_msg_ready_) {
    // Indication is ready when filled with at least one valid value.
    if (meas_records_items[0].type() != meas_record_item_c::types_opts::no_value) {
      is_ind_msg_ready_ = true;
    }
  }
  return true;
}

e2sm_kpm_report_service_style3::e2sm_kpm_report_service_style3(e2_sm_kpm_action_definition_s action_def_,
                                                               e2sm_kpm_meas_provider&       meas_provider_) :
  e2sm_kpm_report_service_base(action_def_, meas_provider_),
  action_def(action_def_generic.action_definition_formats.action_definition_format3()),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format2())
{
  granul_period = action_def.granul_period;
  if (action_def.cell_global_id_present) {
    cell_global_id = action_def.cell_global_id;
  }

  // Initialize RIC indication metadata.
  if (granul_period) {
    ric_ind_message.granul_period_present = true;
    ric_ind_message.granul_period         = granul_period;
  } else {
    ric_ind_message.granul_period = 0;
  }

  meas_cond_list_l& meas_cond_list = action_def.meas_cond_list;
  ric_ind_message.meas_cond_ueid_list.resize(meas_cond_list.size());
  for (uint32_t i = 0; i < meas_cond_list.size(); ++i) {
    ric_ind_message.meas_cond_ueid_list[i].meas_type = meas_cond_list[i].meas_type;
    if (meas_cond_list[i].matching_cond.size()) {
      ric_ind_message.meas_cond_ueid_list[i].matching_cond = meas_cond_list[i].matching_cond;
    } else {
      // Need to have at least one condition to pack the ASN1 msg.
      // Add no_label as placeholder.
      // TODO: do not accept actions without any condition for Report Service Style 3.
      matching_cond_item_s match_cond_item;
      match_cond_item.matching_cond_choice.set_meas_label();
      match_cond_item.matching_cond_choice.meas_label().no_label_present = true;
      match_cond_item.matching_cond_choice.meas_label().no_label         = meas_label_s::no_label_opts::true_value;
      ric_ind_message.meas_cond_ueid_list[i].matching_cond.push_back(match_cond_item);
    }
  }
}

void e2sm_kpm_report_service_style3::clear_collect_measurements()
{
  ric_ind_message.meas_data.clear();
  for (uint32_t i = 0; i < ric_ind_message.meas_cond_ueid_list.size(); ++i) {
    ric_ind_message.meas_cond_ueid_list[i].matching_ueid_list.clear();
  }

  // Save timestamp of measurement collection start.
  ric_ind_header.collet_start_time.from_number(std::time(0));

  // Reset indication msg ready flag.
  is_ind_msg_ready_ = false;
}

bool e2sm_kpm_report_service_style3::collect_measurements()
{
  std::vector<asn1::e2sm_kpm::ueid_c> all_matching_ues;
  std::vector<asn1::e2sm_kpm::ueid_c> cur_matching_ues;
  std::vector<meas_record_item_c>     meas_records_items;
  meas_data_item_s                    meas_data_item;

  // Copy condition list info from action.
  for (auto& meas_cond_ueid : ric_ind_message.meas_cond_ueid_list) {
    // Get UEs matching the required conditions.
    all_matching_ues.clear();
    cur_matching_ues.clear();
    meas_provider.get_ues_matching_test_conditions(meas_cond_ueid.matching_cond, cur_matching_ues);

    if (meas_cond_ueid.matching_ueid_list.size() == 0 and cur_matching_ues.size() > 0) {
      // First measurement collection is will be now.
      for (auto& ue : cur_matching_ues) {
        matching_ueid_item_s matching_ueid_item;
        matching_ueid_item.ue_id = ue;
        meas_cond_ueid.matching_ueid_list.push_back(matching_ueid_item);
      }
    } else if (meas_cond_ueid.matching_ueid_list.size() > 0 and cur_matching_ues.size() > 0) {
      // Check if some new UEs satisfy conditions and add them at the end.
      for (auto& ue : cur_matching_ues) {
        auto it = std::find_if(meas_cond_ueid.matching_ueid_list.begin(),
                               meas_cond_ueid.matching_ueid_list.end(),
                               [&ue](const matching_ueid_item_s& x) { return x.ue_id == ue; });
        if (it == meas_cond_ueid.matching_ueid_list.end()) {
          matching_ueid_item_s matching_ueid_item;
          matching_ueid_item.ue_id = ue;

          // For the new UEs, fill the previous records with NO_VALUE.
          // New UE is at the end of the matching_ueid_list, so just add NO_VALUE at the end of each meas_record.
          meas_record_item_c meas_record_item;
          meas_record_item.set_no_value();
          for (auto& meas_data : ric_ind_message.meas_data) {
            meas_data.meas_record.push_back(meas_record_item);
          }
          meas_cond_ueid.matching_ueid_list.push_back(matching_ueid_item);
        }
      }
    }

    if (meas_cond_ueid.matching_ueid_list.size() == 0) {
      // Skip metric collection as no UE satisfies measurements condition.
      continue;
    }
    // The added meas_records are for present UEs, so they have valid values and the indication is ready.
    is_ind_msg_ready_ = true;

    // Translate to std::vector<ueid_c> for we can put it to get_meas_data() func.
    all_matching_ues.resize(meas_cond_ueid.matching_ueid_list.size());
    std::transform(meas_cond_ueid.matching_ueid_list.begin(),
                   meas_cond_ueid.matching_ueid_list.end(),
                   all_matching_ues.begin(),
                   [](matching_ueid_item_s& x) { return x.ue_id; });

    // Get labels from conditions list.
    asn1::e2sm_kpm::label_info_list_l label_info_list;
    for (auto& cond : meas_cond_ueid.matching_cond) {
      if (cond.matching_cond_choice.type() == matching_cond_item_choice_c::types_opts::meas_label) {
        label_info_item_s label_info_item;
        label_info_item.meas_label = cond.matching_cond_choice.meas_label();
        label_info_list.push_back(label_info_item);
      }
    }

    // Collect measurements and fill RIC Indication Msg.
    meas_records_items.clear();
    meas_provider.get_meas_data(
        meas_cond_ueid.meas_type, label_info_list, all_matching_ues, cell_global_id, meas_records_items);
    // Fill measurements data.
    for (auto& record_item : meas_records_items) {
      meas_data_item.meas_record.push_back(record_item);
    }
    ric_ind_message.meas_data.push_back(meas_data_item);
    // TODO: currently only single meas_cond_ueid is supported. How to store meas_records of multiple?
    break;
  }
  return true;
}

e2sm_kpm_report_service_style4::e2sm_kpm_report_service_style4(e2_sm_kpm_action_definition_s action_def_,
                                                               e2sm_kpm_meas_provider&       meas_provider_) :
  e2sm_kpm_report_service_base(action_def_, meas_provider_),
  action_def(action_def_generic.action_definition_formats.action_definition_format4()),
  subscription_info(action_def.subscription_info),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format3())
{
  granul_period = subscription_info.granul_period;
  if (subscription_info.cell_global_id_present) {
    cell_global_id = subscription_info.cell_global_id;
  }
  nof_collected_meas_data = 0;
  // Note: no need to initialize RIC indication as each time different nof UEs.
}

void e2sm_kpm_report_service_style4::clear_collect_measurements()
{
  // Clear everything as new indication message probably contains different number of UEs.
  ric_ind_message.ue_meas_report_list.clear();

  // Save timestamp of measurement collection start.
  ric_ind_header.collet_start_time.from_number(std::time(0));

  // Reset measurement collection counter.
  nof_collected_meas_data = 0;

  // Reset indication msg ready flag.
  is_ind_msg_ready_ = false;
}

bool e2sm_kpm_report_service_style4::collect_measurements()
{
  std::vector<asn1::e2sm_kpm::ueid_c> all_matching_ues;
  std::vector<asn1::e2sm_kpm::ueid_c> cur_matching_ues;
  std::vector<meas_record_item_c>     meas_records_items;

  // Get UEs matching the required conditions.
  all_matching_ues.clear();
  cur_matching_ues.clear();
  meas_provider.get_ues_matching_test_conditions(action_def.matching_ue_cond_list, cur_matching_ues);

  if (ric_ind_message.ue_meas_report_list.size() == 0 and cur_matching_ues.size() > 0) {
    nof_collected_meas_data = 0;
    // First measurement collection is will be now.
    for (auto& ue : cur_matching_ues) {
      ue_meas_report_item_s ue_meas_report_item;
      ue_meas_report_item.ue_id = ue;
      initialize_ric_ind_msg_format_1(subscription_info.meas_info_list, ue_meas_report_item.meas_report);
      ric_ind_message.ue_meas_report_list.push_back(ue_meas_report_item);
    }
  } else if (ric_ind_message.ue_meas_report_list.size() > 0 and cur_matching_ues.size() > 0) {
    // Check if some new UEs satisfy conditions and add them at the end.
    for (auto& ue : cur_matching_ues) {
      auto it = std::find_if(ric_ind_message.ue_meas_report_list.begin(),
                             ric_ind_message.ue_meas_report_list.end(),
                             [&ue](const ue_meas_report_item_s& x) { return x.ue_id == ue; });
      if (it == ric_ind_message.ue_meas_report_list.end()) {
        ue_meas_report_item_s ue_meas_report_item;
        ue_meas_report_item.ue_id = ue;
        initialize_ric_ind_msg_format_1(subscription_info.meas_info_list, ue_meas_report_item.meas_report);

        // For the new UEs, fill the previous records with NO_VALUE.
        uint32_t nof_metrics = action_def.subscription_info.meas_info_list.size();
        ue_meas_report_item.meas_report.meas_data.resize(nof_collected_meas_data);
        meas_record_item_c meas_record_item;
        meas_record_item.set_no_value();
        for (auto& meas_data : ue_meas_report_item.meas_report.meas_data) {
          meas_data.meas_record.resize(nof_metrics);
          std::fill_n(meas_data.meas_record.begin(), nof_metrics, meas_record_item);
        }
        ric_ind_message.ue_meas_report_list.push_back(ue_meas_report_item);
      }
    }
  }

  if (ric_ind_message.ue_meas_report_list.size() == 0) {
    // Skip metric collection as no UE satisfies measurements condition.
    return false;
  }

  // The added meas_records are for present UEs, so they have valid values and the indication is ready.
  is_ind_msg_ready_ = true;

  // Translate to std::vector<ueid_c> for we can put it to get_meas_data() func.
  all_matching_ues.resize(ric_ind_message.ue_meas_report_list.size());
  std::transform(ric_ind_message.ue_meas_report_list.begin(),
                 ric_ind_message.ue_meas_report_list.end(),
                 all_matching_ues.begin(),
                 [](ue_meas_report_item_s& x) { return x.ue_id; });

  auto& meas_info_list = action_def.subscription_info.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Get measurements.
    meas_records_items.clear();
    meas_provider.get_meas_data(
        meas_info.meas_type, meas_info.label_info_list, all_matching_ues, cell_global_id, meas_records_items);

    // Put each measurement record into a proper place.
    for (unsigned ue_idx = 0; ue_idx < ric_ind_message.ue_meas_report_list.size(); ue_idx++) {
      if (ric_ind_message.ue_meas_report_list[ue_idx].meas_report.meas_data.size() < nof_collected_meas_data + 1) {
        meas_data_item_s meas_data_item;
        meas_data_item.meas_record.push_back(meas_records_items[ue_idx]);
        ric_ind_message.ue_meas_report_list[ue_idx].meas_report.meas_data.push_back(meas_data_item);
      } else {
        ric_ind_message.ue_meas_report_list[ue_idx]
            .meas_report.meas_data[nof_collected_meas_data]
            .meas_record.push_back(meas_records_items[ue_idx]);
      }
    }
  }
  nof_collected_meas_data++;
  return true;
}

e2sm_kpm_report_service_style5::e2sm_kpm_report_service_style5(e2_sm_kpm_action_definition_s action_def_,
                                                               e2sm_kpm_meas_provider&       meas_provider_) :
  e2sm_kpm_report_service_base(action_def_, meas_provider_),
  action_def(action_def_generic.action_definition_formats.action_definition_format5()),
  subscription_info(action_def.subscription_info),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format3())
{
  granul_period = subscription_info.granul_period;
  if (subscription_info.cell_global_id_present) {
    cell_global_id = subscription_info.cell_global_id;
  }

  // Initialize RIC indication metadata (Indication format 1 for each UE).
  meas_info_list_l action_meas_info_list = subscription_info.meas_info_list;
  for (uint32_t i = 0; i < action_def.matching_ueid_list.size(); ++i) {
    ue_meas_report_item_s ue_meas_report_item;
    ue_meas_report_item.ue_id = action_def.matching_ueid_list[i].ue_id;
    initialize_ric_ind_msg_format_1(action_meas_info_list, ue_meas_report_item.meas_report);
    ric_ind_message.ue_meas_report_list.push_back(ue_meas_report_item);
    ue_ids.push_back(action_def.matching_ueid_list[i].ue_id);
  }
  nof_collected_meas_data = 0;
}

void e2sm_kpm_report_service_style5::clear_collect_measurements()
{
  // Clear only measurement data of each user, we keep the structure initialized in ctor.
  for (auto& ue_meas_data : ric_ind_message.ue_meas_report_list) {
    ue_meas_data.meas_report.meas_data.clear();
  }

  // Save timestamp of measurement collection start.
  ric_ind_header.collet_start_time.from_number(std::time(0));

  // Reset measurement collection counter.
  nof_collected_meas_data = 0;

  // Reset indication msg ready flag.
  is_ind_msg_ready_ = false;
}

bool e2sm_kpm_report_service_style5::collect_measurements()
{
  std::vector<meas_record_item_c>     meas_records_items;
  std::vector<asn1::e2sm_kpm::ueid_c> reported_ues;

  auto& meas_info_list = action_def.subscription_info.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Get measurements.
    meas_records_items.clear();
    meas_provider.get_meas_data(
        meas_info.meas_type, meas_info.label_info_list, ue_ids, cell_global_id, meas_records_items);

    // Indication is ready when filled with at least one valid value.
    if (not is_ind_msg_ready_) {
      for (auto& meas_record : meas_records_items) {
        if (meas_record.type() != meas_record_item_c::types_opts::no_value) {
          is_ind_msg_ready_ = true;
          break;
        }
      }
    }

    // Put each measurement record into a proper place.
    for (unsigned ue_idx = 0; ue_idx < ric_ind_message.ue_meas_report_list.size(); ue_idx++) {
      if (ric_ind_message.ue_meas_report_list[ue_idx].meas_report.meas_data.size() < nof_collected_meas_data + 1) {
        meas_data_item_s meas_data_item;
        meas_data_item.meas_record.push_back(meas_records_items[ue_idx]);
        ric_ind_message.ue_meas_report_list[ue_idx].meas_report.meas_data.push_back(meas_data_item);
      } else {
        ric_ind_message.ue_meas_report_list[ue_idx]
            .meas_report.meas_data[nof_collected_meas_data]
            .meas_record.push_back(meas_records_items[ue_idx]);
      }
    }
  }
  nof_collected_meas_data++;
  return true;
}

srsran::byte_buffer e2sm_kpm_report_service_style5::get_indication_message()
{
  // UEs that report only no_values should not be reported.
  // Make a new indication msg and copy UEs that do not contain only no_values.
  asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s          ric_ind_message_copy;
  asn1::e2sm_kpm::e2_sm_kpm_ind_msg_format3_s& ric_ind_message_f3_copy =
      ric_ind_message_copy.ind_msg_formats.set_ind_msg_format3();

  std::vector<uint32_t> ue_idxs;
  bool                  only_no_values;
  for (unsigned i = 0; i < ric_ind_message.ue_meas_report_list.size(); i++) {
    auto& ue_data  = ric_ind_message.ue_meas_report_list[i];
    only_no_values = true;
    for (auto& meas_data : ue_data.meas_report.meas_data) {
      for (auto& meas_record : meas_data.meas_record) {
        if (meas_record.type() != meas_record_item_c::types_opts::no_value) {
          ue_idxs.push_back(i);
          only_no_values = false;
          break;
        }
      }
      if (not only_no_values) {
        break;
      }
    }
  }

  ric_ind_message_f3_copy.ue_meas_report_list.resize(ue_idxs.size());
  for (unsigned i = 0; i < ric_ind_message_f3_copy.ue_meas_report_list.size(); i++) {
    ric_ind_message_f3_copy.ue_meas_report_list[i] = ric_ind_message.ue_meas_report_list[ue_idxs[i]];
  }

  byte_buffer   ind_msg_bytes;
  asn1::bit_ref bref_ind_msg(ind_msg_bytes);
  if (ric_ind_message_copy.pack(bref_ind_msg) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RIC Indication Message");
  };

  clear_collect_measurements();

  return ind_msg_bytes;
};
