/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2sm/e2sm_rc_ies.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {
class e2sm_rc_impl : public e2sm_interface
{
public:
  e2sm_rc_impl(srslog::basic_logger& logger_, e2sm_handler& e2sm_packer_);

  e2sm_handler& get_e2sm_packer() override;

  bool action_supported(const asn1::e2ap::ric_action_to_be_setup_item_s& ric_action) override;

  std::unique_ptr<e2sm_report_service> get_e2sm_report_service(const srsran::byte_buffer& action_definition) override;
  e2sm_control_service*                get_e2sm_control_service(const e2sm_ric_control_request& request) override;

  bool add_e2sm_control_service(std::unique_ptr<e2sm_control_service> control_service) override;

private:
  srslog::basic_logger&                                     logger;
  e2sm_handler&                                             e2sm_packer;
  std::map<uint32_t, std::unique_ptr<e2sm_control_service>> control_services;
};
} // namespace srsran
