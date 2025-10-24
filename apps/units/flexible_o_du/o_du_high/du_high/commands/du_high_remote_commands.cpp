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

#include "apps/units/flexible_o_du/o_du_high/du_high/commands/du_high_remote_commands.h"
#include "nlohmann/json.hpp"

using namespace srsran;

error_type<std::string> ssb_modify_remote_command::execute(const nlohmann::json& json)
{
  auto cells_key = json.find("cells");
  if (cells_key == json.end()) {
    return make_unexpected("'cells' object is missing and it is mandatory");
  }
  if (!cells_key->is_array()) {
    return make_unexpected("'cells' object value type should be an array");
  }

  auto cells_items = cells_key->items();
  if (cells_items.begin() == cells_items.end()) {
    return make_unexpected("'cells' object does not contain any cell entries");
  }

  srs_du::du_param_config_request req;
  for (const auto& cell : cells_items) {
    auto plmn_key = cell.value().find("plmn");
    if (plmn_key == cell.value().end()) {
      return make_unexpected("'plmn' object is missing and it is mandatory");
    }
    if (!plmn_key->is_string()) {
      return make_unexpected("'plmn' object value type should be a string");
    }

    auto nci_key = cell.value().find("nci");
    if (nci_key == cell.value().end()) {
      return make_unexpected("'nci' object is missing and it is mandatory");
    }
    if (!nci_key->is_number_unsigned()) {
      return make_unexpected("'nci' object value type should be an integer");
    }

    auto plmn = plmn_identity::parse(plmn_key.value().get_ref<const nlohmann::json::string_t&>());
    if (!plmn) {
      return make_unexpected("Invalid PLMN identity value");
    }
    auto nci = nr_cell_identity::create(nci_key->get<uint64_t>());
    if (!nci) {
      return make_unexpected("Invalid NR cell identity value");
    }
    nr_cell_global_id_t nr_cgi;
    nr_cgi.nci     = nci.value();
    nr_cgi.plmn_id = plmn.value();

    auto ssb_block_power_key = cell.value().find("ssb_block_power_dbm");
    if (ssb_block_power_key == cell.value().end()) {
      return make_unexpected("'ssb_block_power_dbm' object is missing and it is mandatory");
    }
    if (!ssb_block_power_key->is_number_integer()) {
      return make_unexpected("'ssb_block_power_dbm' object value type should be an integer");
    }
    int ssb_block_power_value = ssb_block_power_key->get<int>();
    if (ssb_block_power_value < -60 || ssb_block_power_value > 50) {
      return make_unexpected(
          fmt::format("'ssb_block_power_dbm' value out of range, received '{}', valid range is from -60 to 50",
                      ssb_block_power_value));
    }
    req.cells.emplace_back(nr_cgi, ssb_block_power_value);
  }

  if (configurator.handle_operator_config_request(req).success) {
    return {};
  }

  return make_unexpected("SSB modify command procedure failed to be applied by the DU");
}

error_type<std::string> rrm_policy_ratio_remote_command::execute(const nlohmann::json& json)
{
  auto policies_key = json.find("policies");
  if (policies_key == json.end()) {
    return make_unexpected("'policies' object is missing and it is mandatory");
  }

  srs_du::du_param_config_request req;

  // RRM Policy ratio group.
  rrm_policy_ratio_group rrm_policy_group;

  // Resource type.
  auto resource_type_key = policies_key.value().find("resourceType");
  if (resource_type_key == policies_key.value().end()) {
    return make_unexpected("'resourceType' object is missing and it is mandatory");
  }
  if (!resource_type_key->is_string()) {
    return make_unexpected("'resourceType' object value type should be a string");
  }
  if (resource_type_key->get<std::string>() == "PRB") {
    rrm_policy_group.resource_type = rrm_policy_ratio_group::resource_type_t::prb;
  } else {
    // TODO: Add support for "RRC" and "DRB"
    return make_unexpected("Only 'resourceType' PRB value is supported");
  }

  // RRM Policy members list.
  auto policy_members_list = policies_key.value().find("rRMPolicyMemberList");
  if (policy_members_list == policies_key.value().end()) {
    return make_unexpected("'rRMPolicyMemberList' object is missing and it is mandatory");
  }
  if (!policy_members_list->is_array()) {
    return make_unexpected("'rRMPolicyMemberList' object value type should be an array");
  }

  for (const auto& policy_member_key : policy_members_list->items()) {
    auto plmn_key = policy_member_key.value().find("plmn");
    if (plmn_key == policy_member_key.value().end()) {
      return make_unexpected("'plmn' object is missing and it is mandatory");
    }
    if (!plmn_key->is_string()) {
      return make_unexpected("'plmn' object value type should be a string");
    }
    expected<plmn_identity> plmn = plmn_identity::parse(plmn_key.value().get_ref<const nlohmann::json::string_t&>());
    if (!plmn) {
      return make_unexpected("Invalid PLMN identity value");
    }

    auto sst_key = policy_member_key.value().find("sst");
    if (sst_key == policy_member_key.value().end()) {
      return make_unexpected("'sst' object is missing and it is mandatory");
    }
    if (!sst_key->is_number_integer()) {
      return make_unexpected("'sst' object value type should be an integer");
    }
    uint8_t sst = sst_key->get<uint8_t>();

    auto sd_key = policy_member_key.value().find("sd");

    expected<slice_differentiator> sd = make_unexpected(default_error_t{});
    // SD is optional.
    if (sd_key != policy_member_key.value().end()) {
      if (!sd_key->is_number_integer()) {
        return make_unexpected("'sd' object value type should be an integer");
      }

      uint32_t sd_int = sd_key->get<uint32_t>();
      sd              = slice_differentiator::create(sd_int);
      if (!sd) {
        return make_unexpected("Invalid slice differentiator value");
      }
    }

    rrm_policy_member policy_member;
    policy_member.plmn_id     = plmn.value();
    policy_member.s_nssai.sst = slice_service_type{sst};
    policy_member.s_nssai.sd  = sd.has_value() ? sd.value() : slice_differentiator{};

    rrm_policy_group.policy_members_list.push_back(policy_member);
  }

  // Minimum percentage of PRBs to be allocated to this group.
  auto                    min_prb_policy_ratio       = policies_key.value().find("min_prb_policy_ratio");
  std::optional<unsigned> min_prb_policy_ratio_value = std::nullopt;
  if (min_prb_policy_ratio != policies_key.value().end()) {
    if (!min_prb_policy_ratio->is_number_integer()) {
      return make_unexpected("'min_prb_policy_ratio' object value type should be an integer");
    }
    min_prb_policy_ratio_value = static_cast<unsigned>(min_prb_policy_ratio->get<int>());
    if (min_prb_policy_ratio_value < 0 || min_prb_policy_ratio_value > 100) {
      return make_unexpected(
          fmt::format("'min_prb_policy_ratio' value out of range, received '{}', valid range is from 0 to 100",
                      min_prb_policy_ratio_value.value()));
    }
  }

  // Maximum percentage of PRBs to be allocated to this group.
  auto                    max_prb_policy_ratio       = policies_key.value().find("max_prb_policy_ratio");
  std::optional<unsigned> max_prb_policy_ratio_value = std::nullopt;
  if (max_prb_policy_ratio != policies_key.value().end()) {
    if (!max_prb_policy_ratio->is_number_integer()) {
      return make_unexpected("'max_prb_policy_ratio' object value type should be an integer");
    }
    max_prb_policy_ratio_value = static_cast<unsigned>(max_prb_policy_ratio->get<int>());
    if (max_prb_policy_ratio_value < 0 || max_prb_policy_ratio_value > 100) {
      return make_unexpected(
          fmt::format("'max_prb_policy_ratio' value out of range, received '{}', valid range is from 0 to 100",
                      max_prb_policy_ratio_value.value()));
    }
  }

  /// The percentage of PRBs to be allocated to this group.
  auto                    dedicated_ratio       = policies_key.value().find("dedicated_ratio");
  std::optional<unsigned> dedicated_ratio_value = std::nullopt;
  if (dedicated_ratio != policies_key.value().end()) {
    if (!dedicated_ratio->is_number_integer()) {
      return make_unexpected("'dedicated_ratio' object value type should be an integer");
    }
    dedicated_ratio_value = static_cast<unsigned>(dedicated_ratio->get<int>());
    if (dedicated_ratio_value < 0 || dedicated_ratio_value > 100) {
      return make_unexpected(
          fmt::format("'dedicated_ratio' value out of range, received '{}', valid range is from 0 to 100",
                      dedicated_ratio_value.value()));
    }
  }

  rrm_policy_group.minimum_ratio   = min_prb_policy_ratio_value;
  rrm_policy_group.maximum_ratio   = max_prb_policy_ratio_value;
  rrm_policy_group.dedicated_ratio = dedicated_ratio_value;

  req.cells.emplace_back(std::nullopt, std::nullopt, std::vector<rrm_policy_ratio_group>{rrm_policy_group});

  if (configurator.handle_operator_config_request(req).success) {
    return {};
  }

  return make_unexpected("RRM policy ratio remote command procedure failed to be applied by the DU");
}
