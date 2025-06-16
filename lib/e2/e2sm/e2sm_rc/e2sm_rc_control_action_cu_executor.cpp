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

#include "e2sm_rc_control_action_cu_executor.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ran/nr_cgi.h"
#include <future>

using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace srsran;

e2sm_rc_control_action_cu_executor_base::e2sm_rc_control_action_cu_executor_base(cu_configurator& cu_configurator_,
                                                                                 uint32_t         action_id_) :
  logger(srslog::fetch_basic_logger("E2SM-RC")), action_id(action_id_), cu_param_configurator(cu_configurator_)
{
}

uint32_t e2sm_rc_control_action_cu_executor_base::get_action_id()
{
  return action_id;
}

ran_function_definition_ctrl_action_item_s e2sm_rc_control_action_cu_executor_base::get_control_action_definition()
{
  ran_function_definition_ctrl_action_item_s action_item;
  action_item.ric_ctrl_action_id = action_id;
  action_item.ric_ctrl_action_name.resize(action_name.size());
  action_item.ric_ctrl_action_name.from_string(action_name);

  for (auto& ran_p : action_params) {
    ctrl_action_ran_param_item_s ctrl_action_ran_param_item;
    ctrl_action_ran_param_item.ran_param_id = ran_p.first;
    ctrl_action_ran_param_item.ran_param_name.resize(ran_p.second.size());
    ctrl_action_ran_param_item.ran_param_name.from_string(ran_p.second);
    action_item.ran_ctrl_action_params_list.push_back(ctrl_action_ran_param_item);
  }

  return action_item;
}
async_task<e2sm_ric_control_response>
e2sm_rc_control_action_cu_executor_base::return_ctrl_failure(const e2sm_ric_control_request& req)
{
  return launch_async([req](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    e2sm_ric_control_response e2sm_response;
    e2sm_response.success                = false;
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
    CORO_RETURN(e2sm_response);
  });
}

e2sm_rc_control_action_3_1_cu_executor::e2sm_rc_control_action_3_1_cu_executor(cu_configurator& cu_configurator_) :
  e2sm_rc_control_action_cu_executor_base(cu_configurator_, 1)
{
  action_name = "Handover Control";
  action_params.insert({1, "Target Primary Cell ID"});
  action_params.insert({2, ">CHOICE Target Cell"});
  action_params.insert({3, ">>NR Cell"});
  action_params.insert({4, ">>>NR CGI"});
  action_params.insert({5, ">>E-UTRA Cell"});
  action_params.insert({6, ">>>E-UTRA CGI"});
  // TODO: add parameters 7-21 (related to PDU session, DRBs, Secondary cell) when supported.
}

bool e2sm_rc_control_action_3_1_cu_executor::ric_control_action_supported(const e2sm_ric_control_request& req)
{
  return true;
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_3_1_cu_executor::execute_ric_control_action(const e2sm_ric_control_request& req)
{
  const e2sm_rc_ctrl_hdr_format1_s& ctrl_hdr =
      std::get<e2sm_rc_ctrl_hdr_s>(req.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format1();
  const e2sm_rc_ctrl_msg_format1_s& ctrl_msg =
      std::get<e2sm_rc_ctrl_msg_s>(req.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  if (ctrl_hdr.ue_id.type() != ue_id_c::types_opts::gnb_ue_id) {
    return return_ctrl_failure(req);
  }
  const ue_id_gnb_s& gnb_ue_id = ctrl_hdr.ue_id.gnb_ue_id();
  if (gnb_ue_id.gnb_cu_ue_f1ap_id_list.size() == 0) {
    return return_ctrl_failure(req);
  }

  // Translate UE-IDs.
  srs_cu_cp::amf_ue_id_t amf_ue_id = srs_cu_cp::uint_to_amf_ue_id(gnb_ue_id.amf_ue_ngap_id);
  srs_cu_cp::guami_t     guami;
  guami.plmn.from_bytes(gnb_ue_id.guami.plmn_id.to_bytes());
  guami.amf_set_id                             = static_cast<uint16_t>(gnb_ue_id.guami.amf_set_id.to_number());
  guami.amf_pointer                            = static_cast<uint8_t>(gnb_ue_id.guami.amf_pointer.to_number());
  guami.amf_region_id                          = static_cast<uint8_t>(gnb_ue_id.guami.amf_region_id.to_number());
  uint64_t              gnb_cu_ue_f1ap_id_uint = gnb_ue_id.gnb_cu_ue_f1ap_id_list[0].gnb_cu_ue_f1ap_id;
  gnb_cu_ue_f1ap_id_t   gnb_cu_ue_f1ap_id      = int_to_gnb_cu_ue_f1ap_id(gnb_cu_ue_f1ap_id_uint);
  srs_cu_cp::ue_index_t ue_index = cu_param_configurator.get_ue_index(amf_ue_id, guami, gnb_cu_ue_f1ap_id);

  if (ue_index == srs_cu_cp::ue_index_t::invalid) {
    logger.error("Ignoring Handover Request. Cause: Couldn't find UE index for given UE-IDs.");
    return return_ctrl_failure(req);
  }

  auto source_du_index = cu_param_configurator.get_du_index(ue_index);
  if (source_du_index == srs_cu_cp::du_index_t::invalid) {
    logger.error("Ignoring Handover Request. Cause: Couldn't find DU index for given UE-IDs.");
    return return_ctrl_failure(req);
  }

  cu_handover_control_config ho_ctrl_cfg;
  // Use a lambda to define the templete function in parse_ran_parameter_value
  auto parse_action_ran_parameter_value_lambda = [this](const ran_param_value_type_c& ran_param,
                                                        uint64_t                      ran_param_id,
                                                        uint64_t                      ue_id,
                                                        cu_handover_control_config&   ctrl_cfg) {
    this->parse_action_ran_parameter_value(ran_param, ran_param_id, ue_id, ctrl_cfg);
  };
  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) != action_params.end()) {
      parse_ran_parameter_value(ran_p.ran_param_value_type,
                                ran_p.ran_param_id,
                                srs_cu_cp::ue_index_to_uint(ue_index),
                                ho_ctrl_cfg,
                                parse_action_ran_parameter_value_lambda);
    }
  }

  srs_cu_cp::cu_cp_intra_cu_handover_request handover_req;
  handover_req.source_ue_index = ue_index;
  handover_req.target_du_index = cu_param_configurator.get_du_index(ho_ctrl_cfg.target_cell_id);
  handover_req.cgi             = ho_ctrl_cfg.target_cell_id;
  handover_req.target_pci      = cu_param_configurator.get_pci(ho_ctrl_cfg.target_cell_id);

  if (handover_req.target_du_index == srs_cu_cp::du_index_t::invalid) {
    logger.error("Ignoring Handover Request. Cause: Couldn't find DU index for CGI=[plmn: {}, nci: {}]",
                 ho_ctrl_cfg.target_cell_id.plmn_id.to_string(),
                 ho_ctrl_cfg.target_cell_id.nci.value());
    return return_ctrl_failure(req);
  }
  if (handover_req.target_pci == INVALID_PCI) {
    logger.error("ue={}: Ignoring Handover Request. Couldn't find PCI for CGI=[plmn: {}, nci: {}]",
                 ho_ctrl_cfg.target_cell_id.plmn_id.to_string(),
                 ho_ctrl_cfg.target_cell_id.nci.value());
    return return_ctrl_failure(req);
  }

  if (source_du_index == handover_req.target_du_index) {
    logger.info("Executing E2SM-RC Handover Request Trigger intra-CU (intra-DU) handover for ue={} on du={} ",
                ue_index,
                source_du_index);
  } else {
    logger.info("Executing E2SM-RC Handover Request Trigger intra-CU (inter-DU) handover for ue={} from source_du={} "
                "to target_du={}",
                ue_index,
                source_du_index,
                handover_req.target_du_index);
  }
  return launch_async([this, source_du_index, handover_req = std::move(handover_req)](
                          coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    srs_cu_cp::cu_cp_intra_cu_handover_response cu_cp_response;
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(cu_cp_response, cu_param_configurator.trigger_handover(source_du_index, handover_req));

    e2sm_ric_control_response e2sm_response;
    e2sm_response.success = cu_cp_response.success;
    if (!e2sm_response.success) {
      e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
    }
    CORO_RETURN(e2sm_response);
  });
}

void e2sm_rc_control_action_3_1_cu_executor::parse_action_ran_parameter_value(
    const asn1::e2sm::ran_param_value_type_c& ran_p,
    uint64_t                                  ran_param_id,
    uint64_t                                  ue_id,
    cu_handover_control_config&               ctrl_cfg)
{
  if (ran_param_id == 4) {
    if (ran_p.ran_p_choice_elem_false().ran_param_value_present) {
      const auto& value_oct = ran_p.ran_p_choice_elem_false().ran_param_value.value_oct_s();
      if (value_oct.size() != 9) {
        logger.error("Invalid NR CGI octet string length: {}", value_oct.size());
        return;
      }
      nr_cgi_s             nr_cgi_asn;
      std::vector<uint8_t> cgi_bytes(value_oct.size());
      std::copy(value_oct.begin(), value_oct.end(), cgi_bytes.begin());
      byte_buffer    cgi_buf = byte_buffer::create(cgi_bytes.begin(), cgi_bytes.end()).value();
      asn1::cbit_ref bref(cgi_buf);
      if (nr_cgi_asn.unpack(bref) != asn1::SRSASN_SUCCESS) {
        logger.error("Failed to unpack NR CGI");
        return;
      }
      ctrl_cfg.target_cell_id.nci = nr_cell_identity::create(nr_cgi_asn.nr_cell_id.to_number()).value();
      ctrl_cfg.target_cell_id.plmn_id.parse(nr_cgi_asn.plmn_id.to_string());
    }
  }
}
