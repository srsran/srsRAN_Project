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

#include "srsran/asn1/e2sm/e2sm_ccc.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace asn1;
using namespace asn1::e2sm_ccc;
using namespace srsran;

class asn1_e2sm_ccc_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    // Start the log backend.
    srslog::init();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

static std::string remove_whitespace(const std::string& str)
{
  std::string result = str;
  result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
  return result;
}

const char simple_struct_plmn_info_json[] = R"json(
{
    "plmnId": {
        "mcc": "999",
        "mnc": "01"
    },
    "snssai": {
        "sst": 33,
        "sd": "00000a"
    }
}
)json";

TEST_F(asn1_e2sm_ccc_test, simple_struct_plmn_info_s)
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  plmn_info_s plmn_info;
  plmn_info.plmn_id_present = true;

  plmn_info.plmn_id.mnc_present = true;
  plmn_info.plmn_id.mnc.from_string("01");
  plmn_info.plmn_id.mcc_present = true;
  plmn_info.plmn_id.mcc.from_string("999");

  plmn_info.snssai_present    = true;
  plmn_info.snssai.sd_present = true;
  plmn_info.snssai.sd.from_number(10);
  plmn_info.snssai.sst_present = true;
  plmn_info.snssai.sst         = 33;

  // Dump and check if equal to ref.
  nlohmann::ordered_json j = plmn_info;
  test_logger.info("nlohmann::json: \n{}", j.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j.dump()), remove_whitespace(simple_struct_plmn_info_json));

  plmn_info_s plmn_info_from_json = j;
  ASSERT_EQ(plmn_info_from_json.plmn_id_present, plmn_info.plmn_id_present);
  ASSERT_EQ(plmn_info_from_json.plmn_id.mnc_present, plmn_info.plmn_id.mnc_present);
  ASSERT_EQ(plmn_info_from_json.plmn_id.mnc.to_string(), plmn_info.plmn_id.mnc.to_string());
  ASSERT_EQ(plmn_info_from_json.plmn_id.mcc_present, plmn_info.plmn_id.mcc_present);
  ASSERT_EQ(plmn_info_from_json.plmn_id.mcc.to_string(), plmn_info.plmn_id.mcc.to_string());
  ASSERT_EQ(plmn_info_from_json.snssai_present, plmn_info.snssai_present);
  ASSERT_EQ(plmn_info_from_json.snssai.sd_present, plmn_info.snssai.sd_present);
  ASSERT_EQ(plmn_info_from_json.snssai.sd.to_string(), plmn_info.snssai.sd.to_string());
  ASSERT_EQ(plmn_info_from_json.snssai.sst_present, plmn_info.snssai.sst_present);
  ASSERT_EQ(plmn_info_from_json.snssai.sst, plmn_info.snssai.sst);

  // From json string.
  plmn_info_s plmn_info_from_json_str = nlohmann::json::parse(simple_struct_plmn_info_json);
  ASSERT_EQ(plmn_info_from_json_str.plmn_id_present, true);
  ASSERT_EQ(plmn_info_from_json_str.plmn_id.mnc_present, true);
  ASSERT_EQ(plmn_info_from_json_str.plmn_id.mnc.to_string(), "01");
  ASSERT_EQ(plmn_info_from_json_str.plmn_id.mcc_present, true);
  ASSERT_EQ(plmn_info_from_json_str.plmn_id.mcc.to_string(), "999");
  ASSERT_EQ(plmn_info_from_json_str.snssai_present, true);
  ASSERT_EQ(plmn_info_from_json_str.snssai.sd_present, true);
  ASSERT_EQ(plmn_info_from_json_str.snssai.sd.to_string(), "00000a");
  ASSERT_EQ(plmn_info_from_json_str.snssai.sst_present, true);
  ASSERT_EQ(plmn_info_from_json_str.snssai.sst, 33);

  // Parse, dump again and check if equal to ref.
  nlohmann::ordered_json j2 = plmn_info_from_json_str;
  TESTASSERT_EQ(remove_whitespace(j.dump()), remove_whitespace(j2.dump()));
}

const char ran_func_def_json[] = R"json(
{
    "ranFunctionName": {
        "ranFunctionShortName": "ORAN-E2SM-CCC",
        "ranFunctionServiceModelOID": "1.3.6.1.4.1.53148.1.6.2.4",
        "ranFunctionDescription": "CCC",
        "ranFunctionInstance": 0
    },
    "listOfSupportedNodeLevelConfigurationStructures": [
        {
            "ranConfigurationStructureName": "ran-config-structure",
            "listOfSupportedAttributes": [
                {
                    "attributeName": "ran-node-attribute",
                    "supportedServices": {
                        "eventTrigger": {
                            "listOfSupportedEventTriggerStyles": [
                                {
                                    "eventTriggerStyleType": 1,
                                    "eventTriggerStyleName": "node-event-trigger-style",
                                    "eventTriggerFormatType": 1
                                }
                            ]
                        },
                        "reportService": {
                            "listOfSupportedReportStyles": [
                                {
                                    "reportServiceStyleType": 1,
                                    "reportServiceStyleName": "report-service-style-1",
                                    "listOfSupportedEventTriggerStylesForReportStyle": [
                                        {
                                            "eventTriggerStyleType": 1
                                        }
                                    ],
                                    "reportServiceActionDefinitionFormatType": 1,
                                    "reportServiceIndicationHeaderFormatType": 1,
                                    "reportServiceIndicationMessageFormatType": 1
                                }
                            ]
                        },
                        "controlService": {
                            "listOfSupportedControlStyles": [
                                {
                                    "controlServiceStyleType": 1,
                                    "controlServiceStyleName": "ctrl-service-style",
                                    "controlServiceHeaderFormatType": 1,
                                    "controlServiceMessageFormatType": 1,
                                    "ricCallProcessIDFormatType": 1,
                                    "controlServiceControlOutcomeFormatType": 1
                                }
                            ]
                        }
                    }
                }
            ]
        }
    ],
    "listOfCellsForRANFunctionDefinition": [
        {
            "cellGlobalID": {
                "plmnIdentity": {
                    "mcc": "001",
                    "mnc": "001"
                },
                "nRCellIdentity": "00066C000"
            },
            "listOfSupportedCellLevelRANConfigurationStructures": [
                {
                    "ranConfigurationStructureName": "cell-cfg-structure",
                    "listOfSupportedAttributes": [
                        {
                            "attributeName": "test-attribute",
                            "supportedServices": {
                                "eventTrigger": {
                                    "listOfSupportedEventTriggerStyles": [
                                        {
                                            "eventTriggerStyleType": 1,
                                            "eventTriggerStyleName": "node-event-trigger-style",
                                            "eventTriggerFormatType": 1
                                        }
                                    ]
                                },
                                "reportService": {
                                    "listOfSupportedReportStyles": [
                                        {
                                            "reportServiceStyleType": 1,
                                            "reportServiceStyleName": "report-service-style-1",
                                            "listOfSupportedEventTriggerStylesForReportStyle": [
                                                {
                                                    "eventTriggerStyleType": 1
                                                },
                                                {
                                                    "eventTriggerStyleType": 2
                                                }
                                            ],
                                            "reportServiceActionDefinitionFormatType": 1,
                                            "reportServiceIndicationHeaderFormatType": 1,
                                            "reportServiceIndicationMessageFormatType": 1
                                        }
                                    ]
                                },
                                "controlService": {
                                    "listOfSupportedControlStyles": [
                                        {
                                            "controlServiceStyleType": 1,
                                            "controlServiceStyleName": "ctrl-service-style",
                                            "controlServiceHeaderFormatType": 1,
                                            "controlServiceMessageFormatType": 1,
                                            "ricCallProcessIDFormatType": 1,
                                            "controlServiceControlOutcomeFormatType": 1
                                        }
                                    ]
                                }
                            }
                        }
                    ]
                }
            ]
        }
    ]
}
)json";

TEST_F(asn1_e2sm_ccc_test, test_ran_func_def)
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  const std::string short_name       = "ORAN-E2SM-CCC";
  const std::string oid              = "1.3.6.1.4.1.53148.1.6.2.4";
  const std::string func_description = "CCC";
  const uint32_t    revision         = 0;

  ran_function_definition_s ran_function_desc;

  // RAN Funtion name.
  ran_function_desc.ran_function_name.ran_function_short_name.resize(short_name.size());
  ran_function_desc.ran_function_name.ran_function_short_name.from_string(short_name);
  ran_function_desc.ran_function_name.ran_function_service_model_o_id.resize(oid.size());
  ran_function_desc.ran_function_name.ran_function_service_model_o_id.from_string(oid);
  ran_function_desc.ran_function_name.ran_function_description.resize(func_description.size());
  ran_function_desc.ran_function_name.ran_function_description.from_string(func_description);
  ran_function_desc.ran_function_name.ran_function_instance_present = true;
  ran_function_desc.ran_function_name.ran_function_instance         = revision;

  // RAN function definition.
  ran_function_desc.list_of_supported_node_level_cfg_structures.resize(1);
  auto& node_cfg_struct = ran_function_desc.list_of_supported_node_level_cfg_structures.back();
  node_cfg_struct.ran_cfg_structure_name.from_string("ran-config-structure");
  node_cfg_struct.list_of_supported_attributes.resize(1);
  attribute_s& node_attribute = node_cfg_struct.list_of_supported_attributes.back();
  node_attribute.attribute_name.from_string("ran-node-attribute");
  // Supported Event Trigger.
  node_attribute.supported_services.event_trigger_present = true;
  node_attribute.supported_services.event_trigger.list_of_supported_event_trigger_styles.resize(1);
  auto& node_ev_trigger = node_attribute.supported_services.event_trigger.list_of_supported_event_trigger_styles.back();
  node_ev_trigger.event_trigger_style_name.from_string("node-event-trigger-style");
  node_ev_trigger.event_trigger_format_type = 1;
  node_ev_trigger.event_trigger_style_type  = 1;
  // Supported Control Styles.
  node_attribute.supported_services.ctrl_service_present = true;
  node_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.resize(1);
  auto& node_ctrl_style = node_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.back();
  node_ctrl_style.ric_call_process_id_format_type_present = true;
  node_ctrl_style.ctrl_service_style_type                 = 1;
  node_ctrl_style.ctrl_service_style_name.from_string("ctrl-service-style");
  node_ctrl_style.ctrl_service_hdr_format_type          = 1;
  node_ctrl_style.ctrl_service_msg_format_type          = 1;
  node_ctrl_style.ric_call_process_id_format_type       = 1;
  node_ctrl_style.ctrl_service_ctrl_outcome_format_type = 1;
  // Supported Report Styles.
  node_attribute.supported_services.report_service_present = true;
  node_attribute.supported_services.report_service.list_of_supported_report_styles.resize(1);
  auto& node_report_style = node_attribute.supported_services.report_service.list_of_supported_report_styles.back();
  node_report_style.report_service_style_type = 1;
  node_report_style.report_service_style_name.from_string("report-service-style-1");
  node_report_style.list_of_supported_event_trigger_styles_for_report_style.resize(1);
  node_report_style.report_service_action_definition_format_type = 1;
  node_report_style.report_service_ind_hdr_format_type           = 1;
  node_report_style.report_service_ind_msg_format_type           = 1;
  auto& node_ev_trig_for_report_style =
      node_report_style.list_of_supported_event_trigger_styles_for_report_style.back();
  node_ev_trig_for_report_style.event_trigger_style_type = 1;

  // Cell RAN function definition.
  ran_function_desc.list_of_cells_for_ran_function_definition.resize(1);
  auto& cell_desc = ran_function_desc.list_of_cells_for_ran_function_definition.back();
  auto& nr_cgi    = cell_desc.cell_global_id.set_nr_cgi();
  nr_cgi.nr_cell_id.from_number(6733824);
  nr_cgi.plmn_id.mcc_present = true;
  nr_cgi.plmn_id.mcc.from_string("001");
  nr_cgi.plmn_id.mnc_present = true;
  nr_cgi.plmn_id.mnc.from_string("001");

  cell_desc.list_of_supported_cell_level_ran_cfg_structures.resize(1);
  auto& cell_ran_cfg_struct = cell_desc.list_of_supported_cell_level_ran_cfg_structures.back();
  cell_ran_cfg_struct.ran_cfg_structure_name.from_string("cell-cfg-structure");
  cell_ran_cfg_struct.list_of_supported_attributes.resize(1);
  attribute_s& cell_attribute = cell_ran_cfg_struct.list_of_supported_attributes.back();
  cell_attribute.attribute_name.from_string("test-attribute");
  // Supported Event Trigger.
  cell_attribute.supported_services.event_trigger_present = true;
  cell_attribute.supported_services.event_trigger.list_of_supported_event_trigger_styles.resize(1);
  auto& cell_ev_trigger = cell_attribute.supported_services.event_trigger.list_of_supported_event_trigger_styles.back();
  cell_ev_trigger.event_trigger_style_name.from_string("node-event-trigger-style");
  cell_ev_trigger.event_trigger_format_type = 1;
  cell_ev_trigger.event_trigger_style_type  = 1;
  // Supported Control Styles.
  cell_attribute.supported_services.ctrl_service_present = true;
  cell_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.resize(1);
  auto& cell_ctrl_style = cell_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.back();
  cell_ctrl_style.ric_call_process_id_format_type_present = true;
  cell_ctrl_style.ctrl_service_style_type                 = 1;
  cell_ctrl_style.ctrl_service_style_name.from_string("ctrl-service-style");
  cell_ctrl_style.ctrl_service_hdr_format_type          = 1;
  cell_ctrl_style.ctrl_service_msg_format_type          = 1;
  cell_ctrl_style.ric_call_process_id_format_type       = 1;
  cell_ctrl_style.ctrl_service_ctrl_outcome_format_type = 1;
  // Supported Report Styles.
  cell_attribute.supported_services.report_service_present = true;
  cell_attribute.supported_services.report_service.list_of_supported_report_styles.resize(1);
  auto& cell_report_style = cell_attribute.supported_services.report_service.list_of_supported_report_styles.back();
  cell_report_style.report_service_style_type = 1;
  cell_report_style.report_service_style_name.from_string("report-service-style-1");
  cell_report_style.report_service_action_definition_format_type = 1;
  cell_report_style.report_service_ind_hdr_format_type           = 1;
  cell_report_style.report_service_ind_msg_format_type           = 1;
  cell_report_style.list_of_supported_event_trigger_styles_for_report_style.resize(2);
  cell_report_style.list_of_supported_event_trigger_styles_for_report_style[0].event_trigger_style_type = 1;
  cell_report_style.list_of_supported_event_trigger_styles_for_report_style[1].event_trigger_style_type = 2;

  // Dump and check if equal to ref.
  nlohmann::ordered_json j = ran_function_desc;
  test_logger.info("nlohmann::json: \n{}", j.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j.dump()), remove_whitespace(ran_func_def_json));

  // Parse, dump again and check if equal to ref.
  ran_function_definition_s ran_desc_from_json = nlohmann::json::parse(j.dump());
  nlohmann::ordered_json    j2                 = ran_desc_from_json;
  test_logger.info("parsed nlohmann::json: \n{}", j2.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j.dump()), remove_whitespace(j2.dump()));
}

const char ric_ctrl_hdr_json[] = R"json(
{
    "controlHeaderFormat": {
        "ricStyleType": 2
    }
}
)json";

const char ric_ctrl_msg_json[] = R"json(
{
    "controlMessageFormat": {
        "listOfCellsControlled": [
            {
                "cellGlobalId": {
                    "plmnIdentity": {
                        "mcc": "001",
                        "mnc": "001"
                    },
                    "nRCellIdentity": "00066C000"
                },
                "listOfConfigurationStructures": [
                    {
                        "ranConfigurationStructureName": "test_cfg_structure",
                        "oldValuesOfAttributes": {
                            "ranConfigurationStructure": {
                                "resourceType": "PRB_DL",
                                "rRMPolicyMemberList": [
                                    {
                                        "plmnId": {
                                            "mcc": "001",
                                            "mnc": "12"
                                        },
                                        "snssai": {
                                            "sst": 3,
                                            "sd": "000017"
                                        }
                                    }
                                ],
                                "rRMPolicyMaxRatio": 75,
                                "rRMPolicyMinRatio": 10,
                                "rRMPolicyDedicatedRatio": 52
                            }
                        },
                        "newValuesOfAttributes": {
                            "ranConfigurationStructure": {
                                "resourceType": "PRB_UL",
                                "rRMPolicyMemberList": [
                                    {
                                        "plmnId": {
                                            "mcc": "001",
                                            "mnc": "12"
                                        },
                                        "snssai": {
                                            "sst": 3,
                                            "sd": "000017"
                                        }
                                    }
                                ],
                                "rRMPolicyMaxRatio": 85,
                                "rRMPolicyMinRatio": 20,
                                "rRMPolicyDedicatedRatio": 72
                            }
                        }
                    }
                ]
            }
        ]
    }
}
)json";

TEST_F(asn1_e2sm_ccc_test, test_ric_ctrl_msg)
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  ric_ctrl_hdr_s ric_ctrl_hdr;
  ric_ctrl_hdr.ctrl_hdr_format.ctrl_hdr_format1().ric_style_type = 2;

  ric_ctrl_msg_s ric_ctrl_msg;
  auto&          ctrl_msg = ric_ctrl_msg.ctrl_msg_format.set_ctrl_msg_format2();
  ctrl_msg.list_of_cells_ctrl.resize(1);
  auto& cell_ctrl = ctrl_msg.list_of_cells_ctrl.back();

  auto& nr_cgi = cell_ctrl.cell_global_id.set_nr_cgi();
  nr_cgi.nr_cell_id.from_number(6733824);
  nr_cgi.plmn_id.mcc_present = true;
  nr_cgi.plmn_id.mcc.from_string("001");
  nr_cgi.plmn_id.mnc_present = true;
  nr_cgi.plmn_id.mnc.from_string("001");

  cell_ctrl.list_of_cfg_structures.resize(1);
  cfg_structure_write_s& cfg_structure_write = cell_ctrl.list_of_cfg_structures.back();
  cfg_structure_write.ran_cfg_structure_name.from_string("test_cfg_structure");
  auto& rrm_policy            = cfg_structure_write.old_values_of_attributes.ran_cfg_structure.set_o_rrm_policy_ratio();
  rrm_policy.res_type_present = true;
  rrm_policy.res_type         = res_type_opts::prb_dl;
  rrm_policy.rrm_policy_min_ratio_present = true;
  rrm_policy.rrm_policy_min_ratio         = 10;
  rrm_policy.rrm_policy_max_ratio_present = true;
  rrm_policy.rrm_policy_max_ratio         = 75;
  rrm_policy.rrm_policy_ded_ratio_present = true;
  rrm_policy.rrm_policy_ded_ratio         = 52;
  rrm_policy.rrm_policy_member_list.resize(1);
  auto& member               = rrm_policy.rrm_policy_member_list.back();
  member.plmn_id_present     = true;
  member.plmn_id.mcc_present = true;
  member.plmn_id.mcc.from_string("001");
  member.plmn_id.mnc_present = true;
  member.plmn_id.mnc.from_string("12");
  member.snssai_present    = true;
  member.snssai.sd_present = true;
  member.snssai.sd.from_number(23);
  member.snssai.sst_present = true;
  member.snssai.sst         = 3;

  auto& new_policy            = cfg_structure_write.new_values_of_attributes.ran_cfg_structure.set_o_rrm_policy_ratio();
  new_policy.res_type_present = true;
  new_policy.res_type         = res_type_opts::prb_ul;
  new_policy.rrm_policy_min_ratio_present = true;
  new_policy.rrm_policy_min_ratio         = 20;
  new_policy.rrm_policy_max_ratio_present = true;
  new_policy.rrm_policy_max_ratio         = 85;
  new_policy.rrm_policy_ded_ratio_present = true;
  new_policy.rrm_policy_ded_ratio         = 72;
  new_policy.rrm_policy_member_list.resize(1);
  auto& member2               = new_policy.rrm_policy_member_list.back();
  member2.plmn_id_present     = true;
  member2.plmn_id.mcc_present = true;
  member2.plmn_id.mcc.from_string("001");
  member2.plmn_id.mnc_present = true;
  member2.plmn_id.mnc.from_string("12");
  member2.snssai_present    = true;
  member2.snssai.sd_present = true;
  member2.snssai.sd.from_number(23);
  member2.snssai.sst_present = true;
  member2.snssai.sst         = 3;

  // Dump and check if equal to ref.
  nlohmann::ordered_json j_hdr = ric_ctrl_hdr;
  test_logger.info("nlohmann::json: \n{}", j_hdr.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_hdr.dump()), remove_whitespace(ric_ctrl_hdr_json));

  nlohmann::ordered_json j_msg = ric_ctrl_msg;
  test_logger.info("nlohmann::json: \n{}", j_msg.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_msg.dump()), remove_whitespace(ric_ctrl_msg_json));

  // Parse, dump again and check if equal to ref.
  ric_ctrl_hdr_s         ric_ctrl_hdr_from_json = nlohmann::json::parse(j_hdr.dump());
  nlohmann::ordered_json j_hdr2                 = ric_ctrl_hdr_from_json;
  test_logger.info("parsed nlohmann::json: \n{}", j_hdr2.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_hdr2.dump()), remove_whitespace(ric_ctrl_hdr_json));

  ric_ctrl_msg_s         ric_ctrl_msg_from_json = nlohmann::json::parse(j_msg.dump());
  nlohmann::ordered_json j_msg2                 = ric_ctrl_msg_from_json;
  test_logger.info("parsed nlohmann::json: \n{}", j_msg2.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_msg2.dump()), remove_whitespace(ric_ctrl_msg_json));
}

const char ric_ind_hdr_json[] = R"json(
{
    "indicationHeaderFormat": {
        "indicationReason": "periodic",
        "eventTime": "ec2c83fdb48863be"
    }
}
)json";

const char ric_ind_msg_json[] = R"json(
{
    "indicationMessageFormat": {
        "listOfCellsReported": [
            {
                "cellGlobalId": {
                    "plmnIdentity": {
                        "mcc": "001",
                        "mnc": "12"
                    },
                    "nRCellIdentity": "00066C000"
                },
                "listOfConfigurationStructuresReported": [
                    {
                        "changeType": "modification",
                        "ranConfigurationStructureName": "test_structure",
                        "valuesOfAttributes": {
                            "ranConfigurationStructure": {
                                "resourceType": "PRB_DL",
                                "rRMPolicyMemberList": [
                                    {
                                        "plmnId": {
                                            "mcc": "001",
                                            "mnc": "12"
                                        },
                                        "snssai": {
                                            "sst": 3,
                                            "sd": "000017"
                                        }
                                    }
                                ],
                                "rRMPolicyMaxRatio": 75,
                                "rRMPolicyMinRatio": 10,
                                "rRMPolicyDedicatedRatio": 52
                            }
                        }
                    }
                ]
            }
        ]
    }
}
)json";

TEST_F(asn1_e2sm_ccc_test, test_ric_ind_msg)
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  ric_ind_hdr_s ric_ind_hdr;
  auto&         hdr    = ric_ind_hdr.ind_hdr_format.ind_hdr_format1();
  hdr.ind_reason.value = e2_sm_ccc_ind_hdr_format1_s::ind_reason_opts::periodic;
  hdr.event_time.from_string("ec2c83fdb48863be");

  ric_ind_msg_s ric_ind_msg;
  auto&         ind_msg = ric_ind_msg.ind_msg_format.set_ind_msg_format2();
  ind_msg.list_of_cells_reported.resize(1);
  cell_reported_s& cell_reported = ind_msg.list_of_cells_reported.back();
  auto&            cell_id       = cell_reported.cell_global_id.set_nr_cgi();
  cell_id.plmn_id.mcc_present    = true;
  cell_id.plmn_id.mcc.from_string("001");
  cell_id.plmn_id.mnc_present = true;
  cell_id.plmn_id.mnc.from_string("12");
  cell_id.nr_cell_id.from_number(6733824);
  cell_reported.list_of_cfg_structures_reported.resize(1);
  auto& cfg_structure = cell_reported.list_of_cfg_structures_reported.back();
  cfg_structure.ran_cfg_structure_name.from_string("test_structure");
  cfg_structure.change_type                      = cfg_structure_s::change_type_opts::mod;
  cfg_structure.old_values_of_attributes_present = false;
  auto& rrm_policy_ratio            = cfg_structure.values_of_attributes.ran_cfg_structure.set_o_rrm_policy_ratio();
  rrm_policy_ratio.res_type_present = true;
  rrm_policy_ratio.res_type         = res_type_opts::prb_dl;
  rrm_policy_ratio.rrm_policy_min_ratio_present = true;
  rrm_policy_ratio.rrm_policy_min_ratio         = 10;
  rrm_policy_ratio.rrm_policy_max_ratio_present = true;
  rrm_policy_ratio.rrm_policy_max_ratio         = 75;
  rrm_policy_ratio.rrm_policy_ded_ratio_present = true;
  rrm_policy_ratio.rrm_policy_ded_ratio         = 52;
  rrm_policy_ratio.rrm_policy_member_list.resize(1);
  auto& member_list               = rrm_policy_ratio.rrm_policy_member_list.back();
  member_list.plmn_id_present     = true;
  member_list.plmn_id.mcc_present = true;
  member_list.plmn_id.mcc.from_string("001");
  member_list.plmn_id.mnc_present = true;
  member_list.plmn_id.mnc.from_string("12");
  member_list.snssai_present    = true;
  member_list.snssai.sd_present = true;
  member_list.snssai.sd.from_number(23);
  member_list.snssai.sst_present = true;
  member_list.snssai.sst         = 3;

  // Dump and check if equal to ref.
  nlohmann::ordered_json j_hdr = ric_ind_hdr;
  test_logger.info("nlohmann::json: \n{}", j_hdr.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_hdr.dump()), remove_whitespace(ric_ind_hdr_json));

  nlohmann::ordered_json j_msg = ric_ind_msg;
  test_logger.info("nlohmann::json: \n{}", j_msg.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_msg.dump()), remove_whitespace(ric_ind_msg_json));

  // Parse, dump again and check if equal to ref.
  ric_ind_hdr_s          hdr_from_json = nlohmann::json::parse(j_hdr.dump());
  nlohmann::ordered_json j_hdr2        = hdr_from_json;
  test_logger.info("parsed nlohmann::json: \n{}", j_hdr2.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_hdr2.dump()), remove_whitespace(ric_ind_hdr_json));

  ric_ind_msg_s          msg_from_json = nlohmann::json::parse(j_msg.dump());
  nlohmann::ordered_json j_msg2        = msg_from_json;
  test_logger.info("parsed nlohmann::json: \n{}", j_msg2.dump(4).c_str());
  TESTASSERT_EQ(remove_whitespace(j_msg2.dump()), remove_whitespace(ric_ind_msg_json));
}
