#include "e2sm_kpm_impl.h"
#include "srsran/asn1/asn1_utils.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

e2sm_kpm_impl::e2sm_kpm_impl(srslog::basic_logger&    logger_,
                             e2sm_handler&            e2sm_packer_,
                             e2_du_metrics_interface& du_metrics_interface_) :
  logger(logger_), e2sm_packer(e2sm_packer_), du_metrics_interface(du_metrics_interface_)
{
}

srsran::byte_buffer e2sm_kpm_impl::handle_action(uint32_t action_id, const srsran::byte_buffer& action_definition)
{
  e2_sm_kpm_ind_msg_s           ric_ind_message;
  e2_sm_kpm_action_definition_s action_def = e2sm_packer.handle_packed_e2sm_kpm_action_definition(action_definition);
  process_action_definition(ric_ind_message, action_def);
  byte_buffer   ind_msg_bytes;
  asn1::bit_ref bref_ind_msg(ind_msg_bytes);
  if (ric_ind_message.pack(bref_ind_msg) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RIC Indication Message");
  };
  generate_indication_header(action_id);
  return ind_msg_bytes;
}

srsran::byte_buffer e2sm_kpm_impl::get_indication_header(uint32_t action_id)
{
  if (ind_hdr_map.count(action_id) == 0) {
    logger.error("Failed to find indication header for action id: %d", action_id);
    return {};
  }
  auto ind_hdr = std::move(ind_hdr_map[action_id]);
  ind_hdr_map.erase(action_id);
  return ind_hdr;
}

bool e2sm_kpm_impl::check_measurement_name(meas_type_c meas_type, const char* meas)
{
  if (strcmp(meas_type.meas_name().to_string().c_str(), meas) == 0) {
    return true;
  }
  return false;
}

void e2sm_kpm_impl::add_matching_condition_item(const char*            name,
                                                meas_cond_ueid_item_s& cond_ueid_item,
                                                matching_cond_item_s&  match_cond_item)
{
  cond_ueid_item.meas_type.set_meas_name().from_string(name);
  cond_ueid_item.matching_cond.push_back(match_cond_item);
}

void e2sm_kpm_impl::process_action_definition(e2_sm_kpm_ind_msg_s&                          ric_ind_message,
                                              asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def)
{
  auto action_type = action_def.action_definition_formats.type().value;
  switch (action_type) {
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format1:
      handle_action_definition_format1(ric_ind_message,
                                       action_def.action_definition_formats.action_definition_format1());
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format2:
      logger.warning("Action type 2 not supported yet");
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format3:
      handle_action_definition_format3(ric_ind_message,
                                       action_def.action_definition_formats.action_definition_format3());
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format4:
      logger.warning("Action type 4 not supported yet");
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format5: {
      logger.warning("Action type 5 not supported yet");
      break;
    }
    default:
      logger.error("Action type not supported");
      break;
  }
}

// process action definition format 1 & uses fields to populate RIC indication message
void e2sm_kpm_impl::handle_action_definition_format1(asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s& ric_ind_message,
                                                     asn1::e2sm_kpm::e2_sm_kpm_action_definition_format1_s action_def)
{
  // Create an RIC indication message with format 1
  ric_ind_message.ind_msg_formats.set_ind_msg_format1();

  // Set the granularity period (TODO: disable as currently not supported in flexric)
  ric_ind_message.ind_msg_formats.ind_msg_format1().granul_period_present = false;
  // ric_ind_message.ind_msg_formats.ind_msg_format2().granul_period         = action_def.granul_period;
  scheduler_ue_metrics ue_metrics = {};
  du_metrics_interface.get_metrics(ue_metrics);
  // Fill indication msg
  auto& meas_info_list = action_def.meas_info_list;
  for (auto& meas_info : meas_info_list) {
    if (check_measurement_name(meas_info.meas_type, "RSRP")) {
      meas_info_item_s meas_info_item;
      meas_info_item.meas_type = meas_info.meas_type;
      label_info_item_s label_info_item;
      label_info_item.meas_label.no_label_present = true;
      label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
      meas_info_item.label_info_list.push_back(label_info_item);
      ric_ind_message.ind_msg_formats.ind_msg_format1().meas_info_list.push_back(meas_info_item);

      meas_data_item_s   meas_data_item;
      meas_record_item_c meas_record_item;
      meas_record_item.set_integer() = (int)ue_metrics.pusch_snr_db;
      meas_data_item.meas_record.push_back(meas_record_item);
      ric_ind_message.ind_msg_formats.ind_msg_format1().meas_data.push_back(meas_data_item);
    }
  }
}

void e2sm_kpm_impl::handle_action_definition_format3(e2_sm_kpm_ind_msg_s& ric_ind_message,
                                                     asn1::e2sm_kpm::e2_sm_kpm_action_definition_format3_s action_def)
{
  // Create an RIC indication message with format 2
  ric_ind_message.ind_msg_formats.set_ind_msg_format2();

  // Set the granularity period
  ric_ind_message.ind_msg_formats.ind_msg_format2().granul_period_present = true;
  ric_ind_message.ind_msg_formats.ind_msg_format2().granul_period         = action_def.granul_period;

  // Get UE metrics from the DU metrics interface
  scheduler_ue_metrics ue_metrics = {};
  du_metrics_interface.get_metrics(ue_metrics);

  // Resize the measurement data and measurement condition UE ID lists to the number of UEs
  size_t num_ues = 1; // TODO get number of UEs from DU
  ric_ind_message.ind_msg_formats.ind_msg_format2().meas_data.resize(num_ues);
  ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list.resize(num_ues);

  // Loop over each UE and each measurement condition in the action definition
  for (unsigned int j = 0; j < num_ues; j++) {
    auto& meas_cond_list = action_def.meas_cond_list;
    for (auto& meas_cond : meas_cond_list) {
      // Check the measurement name and add a matching condition item to the UE's measurement condition UE ID list
      if (check_measurement_name(meas_cond.meas_type, "CQI")) {
        matching_cond_item_s match_cond_item;
        match_cond_item.matching_cond_choice.set_test_cond_info();
        match_cond_item.matching_cond_choice.test_cond_info().test_type.set_cqi();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.set_value_int();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.value_int() = ue_metrics.cqi;
        add_matching_condition_item(
            "CQI", ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j], match_cond_item);
      } else if (check_measurement_name(meas_cond.meas_type, "RSRP")) {
        matching_cond_item_s match_cond_item;
        match_cond_item.matching_cond_choice.set_test_cond_info();
        match_cond_item.matching_cond_choice.test_cond_info().test_type.set_rsrp();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.set_value_int();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.value_int() = ue_metrics.pusch_snr_db;
        add_matching_condition_item(
            "RSRP", ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j], match_cond_item);
      } else if (check_measurement_name(meas_cond.meas_type, "RSRQ")) {
        matching_cond_item_s match_cond_item;
        match_cond_item.matching_cond_choice.set_test_cond_info();
        match_cond_item.matching_cond_choice.test_cond_info().test_type.set_rsrq();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.set_value_int();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.value_int() = ue_metrics.pusch_snr_db;
        add_matching_condition_item(
            "RSRQ", ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j], match_cond_item);
      } else {
        logger.error("Unknown meas type %s", meas_cond.meas_type.meas_name().to_string().c_str());
      }
    }
  }
}

void e2sm_kpm_impl::generate_indication_header(uint32_t action_id)
{
  byte_buffer         ind_hdr_bytes;
  e2_sm_kpm_ind_hdr_s ric_ind_header;
  ric_ind_header.ind_hdr_formats.ind_hdr_format1().vendor_name_present        = false;
  ric_ind_header.ind_hdr_formats.ind_hdr_format1().sender_name_present        = false;
  ric_ind_header.ind_hdr_formats.ind_hdr_format1().sender_type_present        = false;
  ric_ind_header.ind_hdr_formats.ind_hdr_format1().file_formatversion_present = false;
  ric_ind_header.ind_hdr_formats.ind_hdr_format1().collet_start_time.from_number(0);
  asn1::bit_ref bref_ind_hdr(ind_hdr_bytes);
  if (ric_ind_header.pack(bref_ind_hdr) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RIC Indication header");
  };
  ind_hdr_map[action_id] = std::move(ind_hdr_bytes);
}

e2sm_handler& e2sm_kpm_impl::get_e2sm_packer()
{
  return e2sm_packer;
}