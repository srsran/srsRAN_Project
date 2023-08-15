/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2sm_kpm_report_service_impl.h"

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
}

bool e2sm_kpm_report_service_style1::collect_measurements()
{
  // Set the granularity period (TODO: disable as currently not supported in flexric).
  ric_ind_message.granul_period_present = false;
  // ric_ind_message.granul_period         = granul_period;

  // Fill indication msg.
  std::vector<meas_record_item_c> meas_records_items;

  auto& meas_info_list = ric_ind_message.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Get measurements.
    meas_records_items.clear();
    meas_provider.get_meas_data(meas_info.meas_type, meas_info.label_info_list, {}, cell_global_id, meas_records_items);

    meas_data_item_s meas_data_item;
    meas_data_item.meas_record.push_back(meas_records_items[0]);
    ric_ind_message.meas_data.push_back(meas_data_item);
  }

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
}

bool e2sm_kpm_report_service_style2::collect_measurements()
{
  // Set the granularity period.
  ric_ind_message.granul_period_present = true;
  ric_ind_message.granul_period         = granul_period;

  // Fill indication msg.
  std::vector<meas_record_item_c> meas_records_items;

  auto& meas_info_list = action_def.subscript_info.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Get measurements.
    meas_records_items.clear();
    meas_provider.get_meas_data(
        meas_info.meas_type, meas_info.label_info_list, {action_def.ue_id}, cell_global_id, meas_records_items);

    // Fill measurements data.
    meas_data_item_s meas_data_item;
    meas_data_item.meas_record.push_back(meas_records_items[0]);
    ric_ind_message.meas_data.push_back(meas_data_item);
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
      // Need to have at least one condition to pack the ASN1 msg
      // Add no_label as placeholder
      // TODO: do not accept actions without any condition for Report Service Style 3
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
}

bool e2sm_kpm_report_service_style3::collect_measurements()
{
  std::vector<ueid_c>             matching_ues;
  std::vector<meas_record_item_c> meas_records_items;

  // Copy condition list info from action.
  for (auto& meas_cond_ueid : ric_ind_message.meas_cond_ueid_list) {
    // Get UEs matching the required conditions.
    size_t num_ues = 2;
    for (unsigned int j = 0; j < num_ues; j++) {
      // Example two UEs
      ueid_c        ueid;
      ueid_gnb_du_s ueid_gnb_du;
      ueid_gnb_du.gnb_cu_ue_f1_ap_id = j;
      ueid_gnb_du.ran_ueid_present   = false;
      matching_ueid_item_s matching_ueid_item;
      matching_ueid_item.ue_id.set_gnb_du_ueid() = ueid_gnb_du;
      meas_cond_ueid.matching_ueid_list.push_back(matching_ueid_item);

      ueid.set_gnb_du_ueid() = ueid_gnb_du;
      matching_ues.push_back(ueid);
    }
  }

  // Loop over UEs that match required condition and get a measurement for them
  for (auto& meas_cond_ueid : ric_ind_message.meas_cond_ueid_list) {
    auto& meas_cond = meas_cond_ueid;
    meas_records_items.clear();
    meas_provider.get_meas_data(meas_cond.meas_type, {}, matching_ues, cell_global_id, meas_records_items);
    // Fill measurements data.
    meas_data_item_s meas_data_item;
    for (auto& record_item : meas_records_items) {
      meas_data_item.meas_record.push_back(record_item);
    }
    ric_ind_message.meas_data.push_back(meas_data_item);
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

  // Note: no need to initialize RIC indication as each time different nof UEs.
}

void e2sm_kpm_report_service_style4::clear_collect_measurements()
{
  // Clear everything as new indication message probably contains different number of UEs.
  ric_ind_message.ue_meas_report_list.clear();

  // Save timestamp of measurement collection start.
  ric_ind_header.collet_start_time.from_number(std::time(0));
}

bool e2sm_kpm_report_service_style4::collect_measurements()
{
  // Example Indication content with 1 UE.
  ric_ind_message.ue_meas_report_list.resize(1);
  e2_sm_kpm_ind_msg_format1_s& meas_report = ric_ind_message.ue_meas_report_list[0].meas_report;

  // Set the granularity period.
  meas_report.granul_period_present = true;
  meas_report.granul_period         = granul_period;

  // Fill UE ID.
  ueid_enb_s& ueid_gnb    = ric_ind_message.ue_meas_report_list[0].ue_id.set_enb_ueid();
  ueid_gnb.mme_ue_s1ap_id = 0;
  ueid_gnb.gummei.plmn_id.from_number(123);
  ueid_gnb.gummei.mme_code.from_number(1);
  ueid_gnb.gummei.mme_group_id.from_number(2);

  // Fill indication msg.
  std::vector<meas_record_item_c> meas_records_items;

  auto& meas_info_list = action_def.subscription_info.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Fill label info.
    meas_info_item_s meas_info_item;
    meas_info_item.meas_type = meas_info.meas_type;
    label_info_item_s label_info_item;
    label_info_item.meas_label.no_label_present = true;
    label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
    meas_info_item.label_info_list.push_back(label_info_item);
    meas_report.meas_info_list.push_back(meas_info_item);

    // Get measurements.
    meas_records_items.clear();
    ueid_c& ueid = ric_ind_message.ue_meas_report_list[0].ue_id;
    meas_provider.get_meas_data(
        meas_info.meas_type, meas_info.label_info_list, {ueid}, cell_global_id, meas_records_items);

    // Fill measurements data.
    meas_data_item_s meas_data_item;
    meas_data_item.meas_record.push_back(meas_records_items[0]);
    meas_report.meas_data.push_back(meas_data_item);
  }
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

  // Initialize RIC indication metadata (Indication format 1 for each UE)
  meas_info_list_l action_meas_info_list = subscription_info.meas_info_list;
  for (uint32_t i = 0; i < action_def.matching_ueid_list.size(); ++i) {
    ue_meas_report_item_s ue_meas_report_item;
    ue_meas_report_item.ue_id = action_def.matching_ueid_list[i].ue_id;
    initialize_ric_ind_msg_format_1(action_meas_info_list, ue_meas_report_item.meas_report);
    ric_ind_message.ue_meas_report_list.push_back(ue_meas_report_item);
  }
}

void e2sm_kpm_report_service_style5::clear_collect_measurements()
{
  // Clear everything as new indication message probably contains different number of UEs.
  ric_ind_message.ue_meas_report_list.clear();

  // Save timestamp of measurement collection start.
  ric_ind_header.collet_start_time.from_number(std::time(0));
}

bool e2sm_kpm_report_service_style5::collect_measurements()
{
  // Example Indication content with 1 UE.
  ric_ind_message.ue_meas_report_list.resize(1);
  e2_sm_kpm_ind_msg_format1_s& meas_report = ric_ind_message.ue_meas_report_list[0].meas_report;

  // Set the granularity period.
  meas_report.granul_period_present = true;
  meas_report.granul_period         = granul_period;

  // Fill UE ID.
  ueid_enb_s& ueid_gnb    = ric_ind_message.ue_meas_report_list[0].ue_id.set_enb_ueid();
  ueid_gnb.mme_ue_s1ap_id = 0;
  ueid_gnb.gummei.plmn_id.from_number(123);
  ueid_gnb.gummei.mme_code.from_number(1);
  ueid_gnb.gummei.mme_group_id.from_number(2);

  // Fill indication msg.
  std::vector<meas_record_item_c> meas_records_items;

  auto& meas_info_list = action_def.subscription_info.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    // Get measurements.
    meas_records_items.clear();
    ueid_c& ueid = ric_ind_message.ue_meas_report_list[0].ue_id;
    meas_provider.get_meas_data(
        meas_info.meas_type, meas_info.label_info_list, {ueid}, cell_global_id, meas_records_items);

    // Fill measurements data.
    meas_data_item_s meas_data_item;
    meas_data_item.meas_record.push_back(meas_records_items[0]);
    meas_report.meas_data.push_back(meas_data_item);
  }
  return true;
}