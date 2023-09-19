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

#include "../e2sm_param_provider.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/e2/e2sm/e2sm_rc.h"
#include <map>

namespace srsran {
class e2sm_rc_impl : public e2sm_interface
{
public:
  e2sm_rc_impl(srslog::basic_logger&    logger_,
               e2sm_handler&            e2sm_packer_,
               e2sm_param_configurator& param_configurator_,
               e2sm_param_provider&     param_interface_);

  e2sm_handler& get_e2sm_packer() override;

  bool action_supported(const asn1::e2ap::ri_caction_to_be_setup_item_s& ric_action) override;

  virtual std::unique_ptr<e2sm_report_service>
  get_e2sm_report_service(const srsran::byte_buffer& action_definition) override;

  e2sm_param_configurator* get_param_configurator() override;

  void process_control_header(const srsran::byte_buffer& ctrl_header_buff, ric_control_config& ctrl_config) override;

  void process_control_message(const srsran::byte_buffer& ctrl_msg_buff, ric_control_config& ctrl_config) override;

private:
  void process_control_header_format2(asn1::e2sm_rc::e2_sm_rc_ctrl_hdr_format2_s ctrl_hdr,
                                      ric_control_config&                        ctrl_config);
  void process_control_message_format1(asn1::e2sm_rc::e2_sm_rc_ctrl_msg_format1_s ctrl_msg,
                                       ric_control_config&                        ctrl_config,
                                       e2sm_action_provider                       action_provider);
  void process_control_message_format2(asn1::e2sm_rc::e2_sm_rc_ctrl_msg_format2_s ctrl_msg,
                                       ric_control_config&                        ctrl_config);
  void process_control_message_format3(asn1::e2sm_rc::e2_sm_rc_ctrl_msg_format3_s ctrl_msg,
                                       ric_control_config&                        ctrl_config);

  srslog::basic_logger&    logger;
  e2sm_handler&            e2sm_packer;
  e2sm_param_configurator& param_configurator;
  e2sm_param_provider&     param_interface;
};
} // namespace srsran
