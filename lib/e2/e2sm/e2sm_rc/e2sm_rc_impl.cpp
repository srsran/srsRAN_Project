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

#include "e2sm_rc_impl.h"
#include "e2sm_rc_control_service_impl.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2sm/e2sm.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;
using namespace srsran;

e2sm_rc_impl::e2sm_rc_impl(srslog::basic_logger& logger_, e2sm_handler& e2sm_packer_) :
  logger(logger_), e2sm_packer(e2sm_packer_)
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

bool e2sm_rc_impl::add_e2sm_control_service(std::unique_ptr<e2sm_control_service> control_service)
{
  control_services.emplace(control_service->get_style_type(), std::move(control_service));
  return true;
}

e2sm_control_service* e2sm_rc_impl::get_e2sm_control_service(const e2sm_ric_control_request& request)
{
  const e2_sm_rc_ctrl_hdr_s& ctrl_hdr = variant_get<e2_sm_rc_ctrl_hdr_s>(request.request_ctrl_hdr);

  int64_t ric_style_type = 0;
  if (ctrl_hdr.ric_ctrl_hdr_formats.type().value ==
      e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format1) {
    e2_sm_rc_ctrl_hdr_format1_s ctrl_hdr_format1 = ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1();
    ric_style_type                               = ctrl_hdr_format1.ric_style_type;
  } else if (ctrl_hdr.ric_ctrl_hdr_formats.type().value ==
             e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format2) {
    ric_style_type = 255;
  } else if (ctrl_hdr.ric_ctrl_hdr_formats.type().value ==
             e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format3) {
    e2_sm_rc_ctrl_hdr_format3_s ctrl_hdr_format3 = ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format3();
    ric_style_type                               = ctrl_hdr_format3.ric_style_type;
  } else {
    logger.error("RIC control header format not supported");
    return nullptr;
  }

  if (control_services.find(ric_style_type) != control_services.end()) {
    return control_services.at(ric_style_type).get();
  }
  return nullptr;
}
