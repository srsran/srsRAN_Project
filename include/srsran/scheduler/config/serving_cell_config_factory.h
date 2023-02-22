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

namespace srsran {
namespace config_helpers {

carrier_configuration make_default_carrier_configuration(const cell_config_builder_params& params = {});

tdd_ul_dl_config_common make_default_tdd_ul_dl_config_common(const cell_config_builder_params& params = {});

coreset_configuration make_default_coreset_config(const cell_config_builder_params& params = {});

coreset_configuration make_default_coreset0_config(const cell_config_builder_params& params = {});

search_space_configuration make_default_search_space_zero_config();

search_space_configuration make_default_common_search_space_config();

search_space_configuration make_default_ue_search_space_config();

bwp_configuration make_default_init_bwp(const cell_config_builder_params& params = {});

dl_config_common make_default_dl_config_common(const cell_config_builder_params& params = {});

ul_config_common make_default_ul_config_common(const cell_config_builder_params& params = {});

ssb_configuration make_default_ssb_config(const cell_config_builder_params& params = {});

uplink_config make_default_ue_uplink_config(const cell_config_builder_params& params = {});

pusch_config make_default_pusch_config();

pdsch_serving_cell_config make_default_pdsch_serving_cell_config();

nzp_csi_rs_resource_set make_default_nzp_csi_rs_resource_set();

nzp_csi_rs_resource make_default_nzp_csi_rs_resource(const cell_config_builder_params& params = {});

csi_im_resource_set make_default_csi_im_resource_set();

csi_im_resource make_default_csi_im_resource(const cell_config_builder_params& params = {});

csi_resource_config make_default_csi_resource_config();

csi_report_config make_default_csi_report_config(const cell_config_builder_params& params = {});

csi_meas_config make_default_csi_meas_config(const cell_config_builder_params& params = {});

/// \brief Creates a default UE Serving Cell configuration.
serving_cell_config create_default_initial_ue_serving_cell_config(const cell_config_builder_params& params = {});

/// \brief Creates a default UE PSCell configuration.
cell_config_dedicated create_default_initial_ue_spcell_cell_config(const cell_config_builder_params& params = {});

/// \brief Computes maximum nof. candidates that can be accommodated in a CORESET for a given aggregation level.
/// \return Maximum nof. candidates for a aggregation level.
uint8_t compute_max_nof_candidates(aggregation_level aggr_lvl, const coreset_configuration& cs_cfg);

} // namespace config_helpers
} // namespace srsran
