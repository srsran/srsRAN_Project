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

#include "meas_types.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/security/security.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// Arguments for the RRC Reconfiguration procedure.

struct rrc_srb_to_add_mod {
  bool                  reestablish_pdcp_present = false;
  bool                  discard_on_pdcp_present  = false;
  srb_id_t              srb_id                   = srb_id_t::nulltype;
  optional<pdcp_config> pdcp_cfg;
};

struct rrc_cn_assoc {
  optional<uint8_t>       eps_bearer_id;
  optional<sdap_config_t> sdap_cfg;
};

struct rrc_drb_to_add_mod {
  bool                   reestablish_pdcp_present = false;
  bool                   recover_pdcp_present     = false;
  optional<rrc_cn_assoc> cn_assoc;
  drb_id_t               drb_id = drb_id_t::invalid;
  optional<pdcp_config>  pdcp_cfg;
};

struct rrc_security_algorithm_config {
  security::ciphering_algorithm           ciphering_algorithm;
  optional<security::integrity_algorithm> integrity_prot_algorithm;
};

struct rrc_security_config {
  optional<rrc_security_algorithm_config> security_algorithm_cfg;
  optional<std::string>                   key_to_use;
};

struct rrc_radio_bearer_config {
  /// \brief Returns true if at least one of the optional vectors/fields contains an element.
  bool contains_values()
  {
    return (srb_to_add_mod_list.empty() || drb_to_add_mod_list.empty() || drb_to_release_list.empty() ||
            !security_cfg.has_value());
  }
  slotted_id_vector<srb_id_t, rrc_srb_to_add_mod> srb_to_add_mod_list;
  slotted_id_vector<drb_id_t, rrc_drb_to_add_mod> drb_to_add_mod_list;
  std::vector<drb_id_t>                           drb_to_release_list;
  optional<rrc_security_config>                   security_cfg;
  bool                                            srb3_to_release_present = false;
};

struct rrc_master_key_upd {
  bool        key_set_change_ind = false;
  uint8_t     next_hop_chaining_count;
  byte_buffer nas_container;
};

struct rrc_delay_budget_report_cfg {
  std::string type;
  std::string delay_budget_report_prohibit_timer;
};

struct rrc_other_cfg {
  optional<rrc_delay_budget_report_cfg> delay_budget_report_cfg;
};

struct rrc_recfg_v1530_ies {
  bool                         full_cfg_present = false;
  byte_buffer                  master_cell_group;
  std::vector<byte_buffer>     ded_nas_msg_list;
  optional<rrc_master_key_upd> master_key_upd;
  byte_buffer                  ded_sib1_delivery;
  byte_buffer                  ded_sys_info_delivery;
  optional<rrc_other_cfg>      other_cfg;

  // TODO: Add rrc_recfg_v1540_ies_s
  // optional<rrc_recfg_v1540_ies> non_crit_ext;
};

struct rrc_reconfiguration_procedure_request {
  optional<rrc_radio_bearer_config> radio_bearer_cfg;
  byte_buffer                       secondary_cell_group;
  optional<rrc_meas_cfg>            meas_cfg;
  optional<rrc_recfg_v1530_ies>     non_crit_ext;
};

struct rrc_ue_capability_transfer_request {
  // Empty for now but should include ratType and capabilityRequestFilter, etc.
};

} // namespace srs_cu_cp
} // namespace srsran
