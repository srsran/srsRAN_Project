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

#pragma once

#include "e2sm_kpm_utils.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/e2/e2sm/e2sm_kpm.h"
#include <map>

namespace srsran {

class e2sm_kpm_impl : public e2sm_interface
{
public:
  e2sm_kpm_impl(srslog::basic_logger& logger_, e2sm_handler& e2sm_packer_, e2sm_kpm_meas_provider& du_meas_provider_);

  e2sm_handler& get_e2sm_packer() override;

  bool action_supported(const asn1::e2ap::ri_caction_to_be_setup_item_s& ric_action) override;

  std::unique_ptr<e2sm_report_service> get_e2sm_report_service(const srsran::byte_buffer& action_definition) override;
  e2sm_control_service*                get_e2sm_control_service(const e2sm_ric_control_request& request) override;

  bool add_e2sm_control_service(std::unique_ptr<e2sm_control_service> control_service) override;

private:
  /// Helper functions to check whether subscription actions are supported.
  bool process_action_def_meas_info_list(const asn1::e2sm_kpm::meas_info_list_l& meas_info_list,
                                         const e2sm_kpm_metric_level_enum&       level,
                                         const bool&                             cell_scope);

  bool process_action_definition_format1(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_format1_s& action_definition,
                                         const e2sm_kpm_metric_level_enum                             level);

  bool process_action_definition_format1(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format2(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format3(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format4(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format5(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);

  srslog::basic_logger&   logger;
  e2sm_handler&           e2sm_packer;
  e2sm_kpm_meas_provider& du_meas_provider;
};

} // namespace srsran
