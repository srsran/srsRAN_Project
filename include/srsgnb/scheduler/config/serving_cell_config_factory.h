/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_cell_config_master_params.h"
#include "serving_cell_config.h"

namespace srsgnb {
namespace config_helpers {

carrier_configuration make_default_carrier_configuration(const du_cell_config_master_params& params = {});

tdd_ul_dl_config_common make_default_tdd_ul_dl_config_common(const du_cell_config_master_params& params = {});

coreset_configuration make_default_coreset_config(const du_cell_config_master_params& params = {});

coreset_configuration make_default_coreset0_config(const du_cell_config_master_params& params = {});

search_space_configuration make_default_search_space_zero_config();

search_space_configuration make_default_common_search_space_config();

search_space_configuration make_default_ue_search_space_config();

bwp_configuration make_default_init_bwp(const du_cell_config_master_params& params = {});

dl_config_common make_default_dl_config_common(const du_cell_config_master_params& params = {});

ul_config_common make_default_ul_config_common(const du_cell_config_master_params& params = {});

ssb_configuration make_default_ssb_config(const du_cell_config_master_params& params = {});

uplink_config make_default_ue_uplink_config();

pdsch_serving_cell_config make_default_pdsch_serving_cell_config();

serving_cell_config make_default_initial_ue_serving_cell_config();

} // namespace config_helpers
} // namespace srsgnb
