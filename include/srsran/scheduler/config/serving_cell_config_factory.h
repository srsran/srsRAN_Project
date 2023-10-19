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

#include "cell_config_builder_params.h"
#include "serving_cell_config.h"
#include "srsran/ran/csi_rs/csi_meas_config.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/sib/system_info_config.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace config_helpers {

/// Config struct that extends cell_config_builder_params with parameters that can be derived from the former.
struct cell_config_builder_params_extended : public cell_config_builder_params {
  cell_config_builder_params_extended(const cell_config_builder_params& source = {});

  optional<unsigned> ssb_arfcn; /// Absolute frequency of the SSB.
  unsigned           cell_nof_crbs;
  subcarrier_spacing ssb_scs;
};

static_vector<uint8_t, 8> generate_k1_candidates(const tdd_ul_dl_config_common& tdd_cfg, uint8_t min_k1 = 4);

std::vector<pusch_time_domain_resource_allocation>
generate_k2_candidates(cyclic_prefix cp, const tdd_ul_dl_config_common& tdd_cfg, uint8_t min_k2 = 4);

carrier_configuration make_default_dl_carrier_configuration(const cell_config_builder_params_extended& params = {});

carrier_configuration make_default_ul_carrier_configuration(const cell_config_builder_params_extended& params = {});

coreset_configuration make_default_coreset_config(const cell_config_builder_params_extended& params = {});

coreset_configuration make_default_coreset0_config(const cell_config_builder_params_extended& params = {});

search_space_configuration
make_default_search_space_zero_config(const cell_config_builder_params_extended& params = {});

search_space_configuration
make_default_common_search_space_config(const cell_config_builder_params_extended& params = {});

search_space_configuration make_default_ue_search_space_config(const cell_config_builder_params_extended& params = {});

bwp_configuration make_default_init_bwp(const cell_config_builder_params_extended& params = {});

dl_config_common make_default_dl_config_common(const cell_config_builder_params_extended& params = {});

ul_config_common make_default_ul_config_common(const cell_config_builder_params_extended& params = {});

ssb_configuration make_default_ssb_config(const cell_config_builder_params_extended& params = {});

uplink_config make_default_ue_uplink_config(const cell_config_builder_params_extended& params = {});

pusch_config make_default_pusch_config(const cell_config_builder_params_extended& params = {});

srs_config make_default_srs_config(const cell_config_builder_params_extended& params);

pdsch_serving_cell_config make_default_pdsch_serving_cell_config();

pdsch_config make_default_pdsch_config(const cell_config_builder_params_extended& params = {});

/// \brief Creates a default UE Serving Cell configuration.
serving_cell_config
create_default_initial_ue_serving_cell_config(const cell_config_builder_params_extended& params = {});

/// \brief Creates a default UE PSCell configuration.
cell_config_dedicated
create_default_initial_ue_spcell_cell_config(const cell_config_builder_params_extended& params = {});

/// \brief Computes maximum nof. candidates that can be accommodated in a CORESET for a given aggregation level.
/// \return Maximum nof. candidates for a aggregation level.
uint8_t compute_max_nof_candidates(aggregation_level aggr_lvl, const coreset_configuration& cs_cfg);

/// \brief Creates PDSCH Time Domain Resource allocation based on CORESET and SearchSpace configuration.
///
/// Function generates a list of PDSCH Time Domain Resource allocation over which PDSCH can be scheduled. The list is
/// generated considering the nof. DL symbols configured in a particular slot, CORESET duration and first PDCCH
/// monitoring symbol index in SearchSpace configuration. This is mainly needed in TDD usecase where TDD pattern
/// configured contains special slots where not all OFDM symbols are reserved for Downlink transmission.
///
/// \param[in] ss0_idx SearchSpace#0 index.
/// \param[in] common_pdcch_cfg Common PDCCH configuration.
/// \param[in] ded_pdcch_cfg UE dedicated PDCCH configuration.
/// \param[in] tdd_cfg TDD configuration.
/// \return List of PDSCH Time Domain Resource allocation.
std::vector<pdsch_time_domain_resource_allocation>
make_pdsch_time_domain_resource(uint8_t                           ss0_idx,
                                const pdcch_config_common&        common_pdcch_cfg,
                                optional<pdcch_config>            ded_pdcch_cfg = {},
                                optional<tdd_ul_dl_config_common> tdd_cfg       = {});

/// \brief Creates a default UE timers and constants configuration.
ue_timers_and_constants_config make_default_ue_timers_and_constants_config();

} // namespace config_helpers
} // namespace srsran
