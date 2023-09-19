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

#include "e2sm_rc_impl.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;
using namespace srsran;

e2sm_rc_impl::e2sm_rc_impl(srslog::basic_logger&    logger_,
                           e2sm_handler&            e2sm_packer_,
                           e2sm_param_configurator& param_configurator_,
                           e2sm_param_provider&     param_interface_) :
  logger(logger_), e2sm_packer(e2sm_packer_), param_configurator(param_configurator_), param_interface(param_interface_)
{
}

bool e2sm_rc_impl::action_supported(const ri_caction_to_be_setup_item_s& ric_action)
{
  // TODO implement
  logger.debug("No action supported");
  return false;
}

e2sm_handler& e2sm_rc_impl::get_e2sm_packer()
{
  return e2sm_packer;
}

std::unique_ptr<e2sm_report_service> e2sm_rc_impl::get_e2sm_report_service(const srsran::byte_buffer& action_definition)
{
  // TODO implement
  return nullptr;
}

e2sm_param_configurator* e2sm_rc_impl::get_param_configurator()
{
  return &param_configurator;
}

void e2sm_rc_impl::process_control_header(const srsran::byte_buffer& ctrl_header_buff, ric_control_config& ctrl_config)
{
  e2_sm_rc_ctrl_hdr_s ctrl_hdr;
  asn1::cbit_ref      bref_ctrl(ctrl_header_buff);
  if (ctrl_hdr.unpack(bref_ctrl) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to unpack E2SM RC Action Definition\n");
  }

  if (ctrl_hdr.ric_ctrl_hdr_formats.type().value ==
      e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format1) {
    // TODO
  } else if (ctrl_hdr.ric_ctrl_hdr_formats.type().value ==
             e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format2) {
    e2_sm_rc_ctrl_hdr_format2_s ctrl_hdr_format2 = ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format2();
    process_control_header_format2(ctrl_hdr_format2, ctrl_config);
  } else if (ctrl_hdr.ric_ctrl_hdr_formats.type().value ==
             e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format3) {
    // TODO
  } else {
    logger.error("RIC control header format not supported");
    return;
  }
}

void e2sm_rc_impl::process_control_message(const srsran::byte_buffer& ctrl_msg_buff, ric_control_config& ctrl_config)
{
  e2_sm_rc_ctrl_msg_s ctrl_msg;
  asn1::cbit_ref      bref_msg(ctrl_msg_buff);
  if (ctrl_msg.unpack(bref_msg) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to unpack E2SM RC Action Definition\n");
  }

  if (ctrl_msg.ric_ctrl_msg_formats.type().value ==
      e2_sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::ctrl_msg_format1) {
    e2sm_service_provider& rc_provider = param_interface.service_providers.find(1)->second;
    process_control_message_format1(ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format1(),
                                    ctrl_config,
                                    rc_provider.style_providers.at(1).action_providers.at(1));
  } else if (ctrl_msg.ric_ctrl_msg_formats.type().value ==
             e2_sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::ctrl_msg_format2) {
    process_control_message_format2(ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format2(), ctrl_config);
  } else if (ctrl_msg.ric_ctrl_msg_formats.type().value ==
             e2_sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::ctrl_msg_format3) {
    process_control_message_format3(ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format3(), ctrl_config);
  } else {
    logger.error("RIC control message format not supported");
  }
}

void e2sm_rc_impl::process_control_header_format2(e2_sm_rc_ctrl_hdr_format2_s ctrl_hdr, ric_control_config& ctrl_config)
{
  if (ctrl_hdr.ue_id_present) {
    if (ctrl_hdr.ue_id.type().value == ue_id_c::types_opts::gnb_du_ue_id) {
      ctrl_config.ue_id = ctrl_hdr.ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id;
    } else {
      logger.error("UE ID not supported");
    }
  }
}

void e2sm_rc_impl::process_control_message_format1(e2_sm_rc_ctrl_msg_format1_s ctrl_msg,
                                                   ric_control_config&         ctrl_config,
                                                   e2sm_action_provider        action_provider)
{
  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_provider.action_params.count(ran_p.ran_param_id)) {
      if (ran_p.ran_param_id == 12) {
        ctrl_config.max_prb_alloc = ran_p.ran_param_value_type.ran_p_choice_elem_true().ran_param_value.value_int();
      } else if (ran_p.ran_param_id == 13) {
        ctrl_config.min_prb_alloc = ran_p.ran_param_value_type.ran_p_choice_elem_true().ran_param_value.value_int();
      }
    } else {
      logger.error("Parameter not supported");
    }
  }
}

void e2sm_rc_impl::process_control_message_format2(e2_sm_rc_ctrl_msg_format2_s ctrl_msg,
                                                   ric_control_config&         ctrl_config)
{
  e2sm_service_provider& rc_provider = param_interface.service_providers.find(1)->second;
  for (auto& style : ctrl_msg.ric_ctrl_style_list) {
    if (rc_provider.style_providers.count(style.indicated_ctrl_style_type)) {
      for (auto& action : style.ric_ctrl_action_list) {
        if (rc_provider.style_providers.at(style.indicated_ctrl_style_type)
                .action_providers.count(action.ric_ctrl_action_id)) {
          process_control_message_format1(action.ran_p_list,
                                          ctrl_config,
                                          rc_provider.style_providers.at(style.indicated_ctrl_style_type)
                                              .action_providers.at(action.ric_ctrl_action_id));
        } else {
          logger.error("Action not supported");
        }
      }
    } else {
      logger.error("Style not supported");
    }
  }
}

void e2sm_rc_impl::process_control_message_format3(e2_sm_rc_ctrl_msg_format3_s ctrl_msg,
                                                   ric_control_config&         ctrl_config)
{
  // TODO
}
