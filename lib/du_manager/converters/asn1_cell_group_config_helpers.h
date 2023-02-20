/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ran_resource_management/cell_group_config.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"

namespace srsgnb {
namespace srs_du {

asn1::rrc_nr::coreset_s make_asn1_rrc_coreset(const coreset_configuration& cfg);

asn1::rrc_nr::search_space_s make_asn1_rrc_search_space(const search_space_configuration& cfg);

asn1::rrc_nr::pucch_res_set_s make_asn1_rrc_pucch_resource_set(const pucch_resource_set& cfg);

asn1::rrc_nr::sched_request_res_cfg_s make_asn1_rrc_sr_resource(const scheduling_request_resource_config& cfg);

asn1::rrc_nr::pucch_res_s make_asn1_rrc_pucch_resource(const pucch_resource& cfg);

asn1::rrc_nr::tci_state_s make_asn1_rrc_tci_state(const tci_state& cfg);

asn1::rrc_nr::pusch_pathloss_ref_rs_s
make_asn1_rrc_pusch_pathloss_ref_rs(const pusch_config::pusch_power_control::pusch_pathloss_ref_rs& cfg);

asn1::rrc_nr::sri_pusch_pwr_ctrl_s
make_asn1_rrc_sri_pusch_pwr_ctrl(const pusch_config::pusch_power_control::sri_pusch_pwr_ctrl& cfg);

asn1::rrc_nr::srs_res_set_s make_asn1_rrc_srs_res_set(const srs_config::srs_resource_set& cfg);

asn1::rrc_nr::srs_res_s make_asn1_rrc_srs_res(const srs_config::srs_resource& cfg);

asn1::rrc_nr::sched_request_to_add_mod_s make_asn1_rrc_scheduling_request(const scheduling_request_to_addmod& cfg);

asn1::rrc_nr::tag_s make_asn1_rrc_tag_config(const tag& cfg);

/// \brief Fills ASN.1 CellGroupConfig struct.
/// \param[out] out The ASN.1 CellGroupConfig struct to fill.
/// \param[in] src Previous cell group configuration of UE.
/// \param[in] dest Next cell group configuration of UE.
void calculate_cell_group_config_diff(asn1::rrc_nr::cell_group_cfg_s& out,
                                      const cell_group_config&        src,
                                      const cell_group_config&        dest);

} // namespace srs_du
} // namespace srsgnb